#include <stdio.h>
#include <unistd.h>
#include <config.h>
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

    if(emulator_load_rom(&emulator, argv[1]) != 0)
        return 2;

    printf("Hello brother!\n");

    /*for(int i = 0; i < 4096; ++i)
    {
        printf("%c ", emulator.memory[i]);
    }

    putchar('\n');*/

    while(emulator.is_on)
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
