CC=egcc
CFLAGS=-std=c99 -g -Wall -I. -lpthread -lSDL2

all: chip8_emulator

%.o: %.c
	$(CC) $(CFLAGS) -c $^

chip8_emulator: emulator.o main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o chip8_emulator
