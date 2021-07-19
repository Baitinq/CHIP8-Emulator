#include <emulator.h>

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
    int bytes_read = fread(emulator->memory + 0x200, 1, st.st_size, rom);
    if(bytes_read != st.st_size)
    {
        perror("doesnt cuadrar\n");
        return 2;
    }

    return 0;
}

int emulator_tick(Emulator* emulator)
{
    return 0;
}
