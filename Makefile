CC=cc
CFLAGS=-g -Wall -I.

all: chip8_emulator

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $^

chip8_emulator: emulator.o main.c
	$(CC) $(CFLAGS) -o $@ $^

clean: chip8_emulator
	rm *.o $@
