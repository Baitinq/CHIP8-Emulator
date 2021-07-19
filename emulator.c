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

    printf("instr: 0x%x\n", instr);
    printf("A: 0x%x\nX: 0x%x\nY: 0x%x\nN: 0x%x\nNN: 0x%x\nNNN: 0x%x\n\n", first_nibble, X, Y, N, NN, NNN);

    switch(first_nibble)
    {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
    }

    return 0;
}
