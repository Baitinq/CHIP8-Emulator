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

    srand(time(NULL));

    //zero out the memory
    memset(emulator, 0, sizeof(Emulator));

    memcpy(emulator->memory + FONT_LOAD_LOCATION, font, sizeof(font));
    emulator->is_on = 1;

    pthread_t emulator_timers_thread_id;
    pthread_create(&emulator_timers_thread_id, NULL, &emulator_timers_thread, emulator);

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

    uint16_t instr_pc = *pc;

    *pc += 2;

    uint8_t first_nibble = (instr >> 12) & 0xf;
    uint8_t X = (instr & 0x0F00) >> 8; //second_nibble
    uint8_t Y = (instr & 0x00F0) >> 4; //third_nibble
    uint8_t N = (instr & 0x000F); //fourth_nibble
    uint8_t NN = (instr & 0x00FF); //second_byte
    uint16_t NNN = (instr & 0x0FFF); //last three nibbles

    dbgprintf("instr: 0x%x [0x%x]\n", instr, instr_pc);
    dbgprintf("A: 0x%x\nX: 0x%x\nY: 0x%x\nN: 0x%x\nNN: 0x%x\nNNN: 0x%x\n", first_nibble, X, Y, N, NN, NNN);

    switch(first_nibble)
    {
        case 0x0:
            switch(NNN)
            {
                case 0x0E0: //00E0: Clear screen
                    dbgprintf("CLEAR SCREEN!\n");
                    memset(emulator->display, 0, sizeof(emulator->display));
                    break;
            }

            break;
        case 0x1:
            dbgprintf("JUMP! (0x%x)\n", NNN);
            emulator->pc = NNN;

            if(NNN == instr_pc) //infinite loop
            {
                printf("INFINITE LOOP DETECTED! EXITING...\n");
                emulator_dump_registers(emulator);
                getchar(); //block
                exit(0); //stop executing when program over
            }

            break;
        case 0x2:
            printf("TODO: Instr: 0x%x\n", instr);
            assert(0);
            break;
        case 0x3:
            if(emulator->regs.V[X] == NN)
                *pc += 2;
            break;
        case 0x4:
        printf("TODO: Instr: 0x%x\n", instr);
        assert(0);
            break;
        case 0x5:
            if(emulator->regs.V[X] == emulator->regs.V[Y])
                *pc += 2;
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
            switch(N)
            {
                case 0x0:
                emulator->regs.V[X] = emulator->regs.V[Y];
                break;
            case 0x1:
            case 0x2:
            case 0x3:
                printf("TODO: Instr: 0x%x -- %d\n", instr, N);
                assert(0);
                    break;
            case 0x4:
                if(emulator->regs.V[X] + emulator->regs.V[Y] > 255)
                    emulator->regs.VF = 1;
                else
                    emulator->regs.VF = 0;

                emulator->regs.V[X] += emulator->regs.V[Y];
                break;
            case 0x5:
            case 0x6:
            case 0xE:
                break;
            }

            break;
        case 0xA:
            dbgprintf("SET INDEX REGISTER I! (0x%x)\n", NNN);
            emulator->regs.I = NNN;
            break;
        case 0xB:
        printf("TODO: Instr: 0x%x\n", instr);
        assert(0);
            break;
        case 0xC:
            emulator->regs.V[X] = rand() & NN;
            break;
        case 0xD:
            dbgprintf("DRAW!\n");
            int x = emulator->regs.V[X] % 64;
            int y = emulator->regs.V[Y] % 32;

            emulator->regs.VF = 0;

            for(int row = 0; row < N; ++row)
            {
                uint8_t pixels = emulator->memory[emulator->regs.I + row];
                for(int offset = 0; offset < 8; ++offset)
                {
                    if((pixels & (0x80 >> offset)) != 0)
                    {
                        if(emulator->display[x + offset][y + row] == 1)
                          emulator->regs.V[0xF] = 1;
                        emulator->display[x + offset][y + row] ^= 1;
                    }
                }
            }

            emulator->draw_flag = 1;
            break;
        case 0xE:
            break;
        case 0xF:
            break;
    }

    dbgprintf("\n");

    return 0;
}

void* emulator_timers_thread(Emulator* emulator)
{
    printf("STARTED TIMERS THREAD\n");

    while(emulator->is_on)
    {
        if(emulator->delay_timer > 0)
            --emulator->delay_timer;

        if(emulator->sound_timer > 0)
            --emulator->sound_timer;

        usleep(1000000 / 60);
    }

    return NULL;
}

void emulator_dump_registers(Emulator* emulator)
{
    printf("REGISTERS: \n");
    printf("\tV0: 0x%x - %d\n", emulator->regs.V0, emulator->regs.V0);
    printf("\tV1: 0x%x - %d\n", emulator->regs.V1, emulator->regs.V1);
    printf("\tV2: 0x%x - %d\n", emulator->regs.V2, emulator->regs.V2);
    printf("\tV3: 0x%x - %d\n", emulator->regs.V3, emulator->regs.V3);
    printf("\tV4: 0x%x - %d\n", emulator->regs.V4, emulator->regs.V4);
    printf("\tV5: 0x%x - %d\n", emulator->regs.V5, emulator->regs.V5);
    printf("\tV6: 0x%x - %d\n", emulator->regs.V6, emulator->regs.V6);
    printf("\tV7: 0x%x - %d\n", emulator->regs.V7, emulator->regs.V7);
    printf("\tV8: 0x%x - %d\n", emulator->regs.V8, emulator->regs.V8);
    printf("\tV9: 0x%x - %d\n", emulator->regs.V9, emulator->regs.V9);
    printf("\tVA: 0x%x - %d\n", emulator->regs.VA, emulator->regs.VA);
    printf("\tVB: 0x%x - %d\n", emulator->regs.VB, emulator->regs.VB);
    printf("\tVC: 0x%x - %d\n", emulator->regs.VC, emulator->regs.VC);
    printf("\tVD: 0x%x - %d\n", emulator->regs.VD, emulator->regs.VD);
    printf("\tVE: 0x%x - %d\n", emulator->regs.VE, emulator->regs.VE);
    printf("\tVF: 0x%x - %d\n", emulator->regs.VF, emulator->regs.VF);
    printf("\tI: 0x%x - %d\n", emulator->regs.I, emulator->regs.I);
}
