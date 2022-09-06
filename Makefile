
all: test

test: test.c
	gcc -o $@ $< -lSDL2 -lm -Wall -Wextra

run: test
	./test

clean:
	rm -f test

.PHONY: all clean run
