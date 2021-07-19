#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <stdio.h>
#include  <sys/stat.h>

typedef struct
{
    uint8_t is_on;
    uint16_t pc;
    uint8_t memory[4096];
} Emulator;

int emulator_load_rom(Emulator* emulator, char* rom_name);
int emulator_tick(Emulator* emulator);

#endif
