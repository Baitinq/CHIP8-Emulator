#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <stdio.h>
#include <config.h>
#include <sys/stat.h>
#include <string.h>

typedef struct
{
    // general purpose registers
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

    uint16_t I; // index register
} Registers;

typedef struct
{
    uint8_t is_on;
    uint16_t pc; //program counter
    Registers regs;
    uint8_t sp; //stack pointer
    uint16_t stack[16];
    volatile uint8_t delay_timer;
    volatile uint8_t sound_timer;
    uint8_t memory[4096];
} Emulator;

int emulator_initialise(Emulator* emulator);
int emulator_load_rom(Emulator* emulator, char* rom_name);
int emulator_tick(Emulator* emulator);
void emulator_dump_registers(Emulator* emulator);

#endif
