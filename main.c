#include <emulator.h>

int main(int argc, char** argv);
void show_help();

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        show_help();
        return 1;
    }

    Emulator emulator;

    emulator_initialise(&emulator);

    if(emulator_load_rom(&emulator, argv[1]) != 0)
        return 2;

    printf("Hello brother!\n");

    while(emulator.is_on == 1)
    {
        emulator_tick(&emulator);
        usleep(1000000 / INSTRUCTIONS_PER_SECOND);
    }

    return 0;
}

void show_help()
{
    printf("BAD USAGE! -> ./chip8_emulator [ROM]\n");
}
