#include <emulator.h>

int emulator_initialise(Emulator* emulator)
{
    const uint8_t font[] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    //zero out the memory
    memset(emulator, 0, sizeof(Emulator));

    memcpy(emulator->memory + FONT_LOAD_LOCATION, font, sizeof(font));
    emulator->is_on = 1;

    return 0;
}

int emulator_load_rom(Emulator* emulator, char* rom_name)
{
    printf("load rom!: %s\n", rom_name);
    FILE* rom = fopen(rom_name, "r");
    if(rom == NULL)
    {
        perror("no rom file!\n");
        return 1;
    }

    struct stat st;
    fstat(fileno(rom), &st);

     //rom loaded after 0x200 into memory
    int bytes_read = fread(emulator->memory + GAME_LOAD_LOCATION, 1, st.st_size, rom);
    fclose(rom);

    if(bytes_read != st.st_size)
    {
        perror("doesnt cuadrar\n");
        return 2;
    }

    emulator->pc = GAME_LOAD_LOCATION;

    return 0;
}

int emulator_tick(Emulator* emulator)
{
    uint16_t* pc = &emulator->pc;
    uint16_t instr = (emulator->memory[*pc] << 8) | emulator->memory[*pc + 1];
    *pc += 2;

    uint8_t first_nibble = (instr >> 12) & 0xf;
    uint8_t X = (instr & 0x0F00) >> 8; //second_nibble
    uint8_t Y = (instr & 0x00F0) >> 4; //third_nibble
    uint8_t N = (instr & 0x000F); //fourth_nibble
    uint8_t NN = (instr & 0x00FF); //second_byte
    uint16_t NNN = (instr & 0x0FFF); //last three nibbles

    dbgprintf("instr: 0x%x\n", instr);
    dbgprintf("A: 0x%x\nX: 0x%x\nY: 0x%x\nN: 0x%x\nNN: 0x%x\nNNN: 0x%x\n", first_nibble, X, Y, N, NN, NNN);

    switch(first_nibble)
    {
        case 0x0:
            switch(NNN)
            {
                case 0x000:
                    emulator_dump_registers(emulator);
                    exit(1); //stop executing when program over
                case 0x0E0: //00E0: Clear screen
                    dbgprintf("CLEAR SCREEN!\n");
                    break;
            }

            break;
        case 0x1:
            dbgprintf("JUMP! (0x%x)\n", NNN);
            emulator->pc = NNN;
            break;
        case 0x2:
            break;
        case 0x3:
            break;
        case 0x4:
            break;
        case 0x5:
            break;
        case 0x6:
            dbgprintf("SET REGISTER VX! (0x%x)\n", NN);
            emulator->regs.V[X] = NN;
            break;
        case 0x7:
            dbgprintf("ADD VALUE TO REGISTER VX! (0x%x)\n", NN);
            emulator->regs.V[X] += NN;
            break;
        case 0x8:
            break;
        case 0xA:
            dbgprintf("SET INDEX REGISTER I! (0x%x)\n", NNN);
            emulator->regs.I = NNN;
            break;
        case 0xB:
            break;
        case 0xC:
            break;
        case 0xD:
            dbgprintf("DRAW!\n");
            break;
        case 0xE:
            break;
        case 0xF:
            break;
    }

    dbgprintf("\n");

    return 0;
}

void emulator_dump_registers(Emulator* emulator)
{
    printf("REGISTERS: \n");
    printf("\tV0: 0x%x", emulator->regs.V0);
    printf("\tV1: 0x%x", emulator->regs.V1);
    printf("\tV2: 0x%x", emulator->regs.V2);
    printf("\tV3: 0x%x", emulator->regs.V3);
    printf("\tV4: 0x%x", emulator->regs.V4);
    printf("\tV5: 0x%x", emulator->regs.V5);
    printf("\tV6: 0x%x", emulator->regs.V6);
    printf("\tV7: 0x%x", emulator->regs.V7);
    printf("\tV8: 0x%x", emulator->regs.V8);
    printf("\tV9: 0x%x", emulator->regs.V9);
    printf("\tVA: 0x%x", emulator->regs.VA);
    printf("\tVB: 0x%x", emulator->regs.VB);
    printf("\tVC: 0x%x", emulator->regs.VC);
    printf("\tVD: 0x%x", emulator->regs.VD);
    printf("\tVE: 0x%x", emulator->regs.VE);
    printf("\tVF: 0x%x", emulator->regs.VF);
    printf("\tI: 0x%x", emulator->regs.I);
}
