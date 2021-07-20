CC=clang
INC_DIRS="."
CFLAGS=-std=c99 -g -Wall -I$(INC_DIRS)
LDFLAGS=-lpthread $(shell pkg-config --libs --cflags sdl2)

all: chip8_emulator

%.o: %.c
	$(CC) $(CFLAGS) -c $^

chip8_emulator: emulator.o main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm *.o chip8_emulator
