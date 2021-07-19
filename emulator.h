#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

typedef struct
{
    // general purpose registers
    union
    {
        uint8_t V[16]; //VF is the flag register

        struct
        {
            uint8_t V0;
            uint8_t V1;
            uint8_t V2;
            uint8_t V3;
            uint8_t V4;
            uint8_t V5;
            uint8_t V6;
            uint8_t V7;
            uint8_t V8;
            uint8_t V9;
            uint8_t VA;
            uint8_t VB;
            uint8_t VC;
            uint8_t VD;
            uint8_t VE;
            uint8_t VF; //flag register
        };
    };

    // index register
    uint16_t I;
} Registers;

typedef struct
{
    volatile uint8_t is_on;
    volatile uint8_t delay_timer;
    volatile uint8_t sound_timer;
    uint8_t display[64][32];
    Registers regs;
    uint8_t sp; //stack pointer
    uint16_t stack[16];
    uint16_t pc; //program counter
    uint8_t memory[4096];
} Emulator;

int emulator_initialise(Emulator* emulator);
int emulator_load_rom(Emulator* emulator, char* rom_name);
int emulator_tick(Emulator* emulator);
void emulator_dump_registers(Emulator* emulator);

void* emulator_timers_thread();

#endif
