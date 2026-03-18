CC = cc
CFLAGS = -std=c99 -Wall -Wextra -O2
LDFLAGS = -lm

SDL2_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null)
SDL2_LDFLAGS := $(shell sdl2-config --libs 2>/dev/null)

weave: weave.c main.c weave.h
	$(CC) $(CFLAGS) $(SDL2_CFLAGS) -o $@ weave.c main.c $(LDFLAGS) $(SDL2_LDFLAGS)

clean:
	rm -f weave

.PHONY: clean
