CC=cc
CFLAGS=-g -Wall -I.

all: chip8_emulator

%.o: %.c
	$(CC) $(CFLAGS) -c $^

chip8_emulator: emulator.o main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o chip8_emulator
