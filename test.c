#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#define stop_timer(tid)			\
do {							\
	if (tid) {					\
		SDL_RemoveTimer (tid);	\
		tid = 0;				\
	}							\
} while (0)


static Uint32 cb_haptic (Uint32 interval, void *arg) {
	SDL_Haptic *haptic = arg;

	if (haptic) {
		SDL_HapticRumblePlay (haptic, 0.5f, 250);
	}
}

int main (void) {
	SDL_Init (SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow (
		"Test",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		720, 1440,
		SDL_WINDOW_SHOWN
	);
	if (!window)
		return 1;

	SDL_Renderer *renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
		return 1;

	SDL_Haptic *haptic = SDL_HapticOpen (0);
	if (haptic) {
		SDL_HapticRumbleInit (haptic);
	} else {

		printf ("No haptic device found.\n");
	}

	int ww, wh, num_fingers = 0;
	SDL_GetWindowSize (window, &ww, &wh);
	float x = 0.0f, y = 0.0f;
	float scale = 50.0f;
	SDL_TimerID timerID = 0;

	while (1) {
		SDL_Event e;
		while (SDL_PollEvent (&e)) {
			switch (e.type) {
			case SDL_QUIT:
				goto end;

			case SDL_MULTIGESTURE:
				if (e.mgesture.numFingers == 2) {
					stop_timer (timerID);
					scale *= 1.0f + e.mgesture.dDist * 5.0f;
				}
				/*printf ("touchId = %u\n", (unsigned)e.mgesture.touchId);
				printf ("dTheta = %f\n", e.mgesture.dTheta);
				printf ("dDist = %f\n", e.mgesture.dDist);
				printf ("x = %f\n", e.mgesture.x);
				printf ("y = %f\n", e.mgesture.y);
				printf ("num_fingers = %u\n", (unsigned)e.mgesture.numFingers);*/
				break;
			case SDL_FINGERDOWN:
				if (!timerID && num_fingers == 0)
					timerID = SDL_AddTimer (500, &cb_haptic, haptic);

				++num_fingers;
				break;
			case SDL_FINGERUP:
				stop_timer (timerID);
				--num_fingers;
				break;
			case SDL_FINGERMOTION:
				if (num_fingers == 1) {
					stop_timer (timerID);
					x += e.tfinger.dx;
					y += e.tfinger.dy;
				}
				break;
			}
		}
		SDL_SetRenderDrawColor (renderer, 255, 255, 255, 255);
		SDL_RenderClear (renderer);

		const SDL_Rect rect = { x * ww, y * wh, scale, scale };
		SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
		SDL_RenderFillRect (renderer, &rect);

		SDL_RenderPresent (renderer);
	}

end:
	if (haptic)
		SDL_HapticClose (haptic);
	SDL_DestroyRenderer (renderer);
	SDL_DestroyWindow (window);
	SDL_Quit ();
}

