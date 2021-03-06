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

    emulator->keys[0].value = 'x'; //O
    emulator->keys[1].value = '1'; //1
    emulator->keys[2].value = '2'; //2
    emulator->keys[3].value = '3'; //3
    emulator->keys[4].value = 'q'; //4
    emulator->keys[5].value = 'w'; //5
    emulator->keys[6].value = 'e'; //6
    emulator->keys[7].value = 'a'; //7
    emulator->keys[8].value = 's'; //8
    emulator->keys[9].value = 'd'; //9
    emulator->keys[10].value = 'z'; //D
    emulator->keys[11].value = 'c'; //B
    emulator->keys[12].value = '4'; //C
    emulator->keys[13].value = 'r'; //D
    emulator->keys[14].value = 'f'; //E
    emulator->keys[15].value = 'v'; //F

    pthread_create(&emulator->timers_thread, NULL, &emulator_timers_thread, emulator);

    return 0;
}

int emulator_deinitialise(Emulator* emulator)
{
    //join timers thread
    emulator->is_on = 0;

    pthread_join(emulator->timers_thread, NULL);

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

int emulator_handle_key_press(Emulator* emulator, uint8_t key)
{
    for(size_t i = 0; i < (sizeof(emulator->keys) / sizeof(emulator->keys[0])); ++i)
    {
        if(emulator->keys[i].value == key)
        {
            dbgprintf("KEY ACTIVATED: %c-%d!\n", key, key);
            emulator->keys[i].activated = 1;
            break;
        }
    }

    return 0;
}

int emulator_handle_key_release(Emulator* emulator, uint8_t key)
{
    for(size_t i = 0; i < (sizeof(emulator->keys) / sizeof(emulator->keys[0])); ++i)
    {
        if(emulator->keys[i].value == key)
        {
            dbgprintf("KEY RELEASE: %c-%d!\n", key, key);
            emulator->keys[i].activated = 0;
            break;
        }
    }

    return 0;
}

int emulator_tick(Emulator* emulator)
{
    uint16_t* pc = &emulator->pc;
    uint16_t instr = (emulator->memory[*pc] << 8) | emulator->memory[*pc + 1];

    uint16_t instr_pc = *pc;

    emulator_step(emulator);

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
                    emulator->draw_flag = 1;
                    break;
                case 0x0EE: //00EE: Return from subroutine
                    //pop callee pc from stack and set current pc to it
                    emulator->pc = emulator->stack[--emulator->sp];
                    break;
            default:
                printf("DEFAULT: Instr: 0x%x\n", instr);
                assert(0);
            }

            break;
        case 0x1:
            dbgprintf("JUMP! (0x%x)\n", NNN);
            emulator->pc = NNN;

#ifdef STOP_ON_INFINITE_LOOP
            if(NNN == instr_pc) //infinite loop
            {
                printf("INFINITE LOOP DETECTED! EXITING...\n");
                emulator_dump_registers(emulator);
                sleep(2);
                //getchar(); //block
                emulator->is_on = 0;
            }
#endif
            break;
        case 0x2:
            dbgprintf("CALL SUBROUTINE! (0x%x)\n", NNN);
            //push pc to stack and increment sp
            emulator->stack[emulator->sp++] = emulator->pc;

            emulator->pc = NNN;
            break;
        case 0x3:
            dbgprintf("SKIP INSTR IF REGISTER VX == NN!\n");
            if(emulator->regs.V[X] == NN)
            {
                dbgprintf("SKIPPED COZ THEY WERE EQUAL!\n");
                emulator_step(emulator);
            }
            break;
        case 0x4:
            dbgprintf("SKIP INSTR IF REGISTER VX != NN!\n");
            if(emulator->regs.V[X] != NN)
            {
                dbgprintf("SKIPPED COZ THEY WERE NOT EQUAL!\n");
                emulator_step(emulator);
            }
            break;
        case 0x5:
            dbgprintf("SKIP INSTR IF REGISTER VX == VY!\n");
            if(emulator->regs.V[X] == emulator->regs.V[Y])
            {
                dbgprintf("SKIPPED COZ THEY WERE EQUAL!\n");
                emulator_step(emulator);
            }
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
                    dbgprintf("SET VX TO VY!\n");
                    emulator->regs.V[X] = emulator->regs.V[Y];
                    break;
                case 0x1:
                    emulator->regs.V[X] |= emulator->regs.V[Y];
                    break;
                case 0x2:
                    emulator->regs.V[X] &= emulator->regs.V[Y];
                    break;
                case 0x3:
                        emulator->regs.V[X] ^= emulator->regs.V[Y];
                        break;
                case 0x4:
                    dbgprintf("ADD VY TO VX!\n");
                    emulator->regs.V[X] += emulator->regs.V[Y];
                    if(emulator->regs.V[Y] > emulator->regs.V[X])
                        emulator->regs.VF = 1; //carry
                    else
                        emulator->regs.VF = 0;

                    break;
                case 0x5:
                    if(emulator->regs.V[X] > emulator->regs.V[Y])
                        emulator->regs.VF = 1;
                    else
                        emulator->regs.VF = 0;

                    emulator->regs.V[X] -= emulator->regs.V[Y];    
                    break;
                case 0x6:
                    emulator->regs.VF = emulator->regs.V[X] & 0x1;
                    emulator->regs.V[X] >>= 1;
                    break;
                case 0x7:
                    if(emulator->regs.V[Y] > emulator->regs.V[X])
                        emulator->regs.VF = 1;
                    else
                        emulator->regs.VF = 0;
                    emulator->regs.V[X] = emulator->regs.V[Y] - emulator->regs.V[X];
                    break;
                case 0xE:
                    emulator->regs.VF = emulator->regs.V[X] >> 7;
                    emulator->regs.V[X] <<= 1;
                    break;
            default:
                printf("DEFAULT: Instr: 0x%x\n", instr);
                assert(0);
            }

            break;
        case 0x9:
            dbgprintf("SKIP INSTR IF REGISTER VX != VY!\n");
            if(emulator->regs.V[X] != emulator->regs.V[Y])
            {
                dbgprintf("SKIPPED COZ THEY WERE NOT EQUAL!\n");
                emulator_step(emulator);
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
            switch(NN)
            {
                case 0xA1: //EX9E
                    //skip if key not pressed
                    if(emulator->keys[emulator->regs.V[X]].activated == 0)
                        *pc += 2;
                    break;
                case 0x9E: //EX9E
                    //skip if key pressed
                    if(emulator->keys[emulator->regs.V[X]].activated == 1)
                        *pc += 2;
                    break;
                default:
                    printf("DEFAULT: Instr: 0x%x\n", instr);
                    assert(0);
            }

            break;
        case 0xF:
            switch(NN)
            {
                case 0x1E: //FX1E
                    dbgprintf("ADD V[X] to the I register!\n");
                    if(emulator->regs.I + emulator->regs.V[X] > 0xFFF)
                        emulator->regs.VF = 1;
                    else
                        emulator->regs.VF = 0;
                    emulator->regs.I += emulator->regs.V[X];
                    break;
            case 0x55: //FX55
                for (int i = 0; i <= X; ++i)
                    emulator->memory[emulator->regs.I + i] = emulator->regs.V[i];
                COSMAC_VIP(emulator->regs.I += X + 1;)
                break;
            case 0x65: //FX65
                for (int i = 0; i <= X; ++i)
                    emulator->regs.V[i] = emulator->memory[emulator->regs.I + i];
                COSMAC_VIP(emulator->regs.I += X + 1;)
                break;
            case 0x15: //FX15
                dbgprintf("SET THE DELAY TIMER TO V[X]!\n");
                emulator->delay_timer = emulator->regs.V[X];
                break;
            case 0x07: //FX07
                dbgprintf("SET V[X] TO THE DELAY TIMER!\n");
                emulator->regs.V[X] = emulator->delay_timer;
                break;
            case 0x18: //FX18
                dbgprintf("SET THE SOUND TIMER TO V[X]!\n");
                emulator->sound_timer = emulator->regs.V[X];
                break;
            case 0x33: //FX33
            {
                uint8_t number = emulator->regs.V[X];
                for(uint8_t offset = 2; number > 0; --offset)
                {
                    emulator->memory[emulator->regs.I + offset] = number % 10; //last digit
                    number /= 10;
                }

                break;
            }
            case 0x0A:
            {
                uint8_t key_pressed = 0;
                for(uint8_t i = 0; i < (sizeof(emulator->keys) / sizeof(emulator->keys[0])); ++i)
                {
                    if(emulator->keys[i].activated == 1)
                    {
                        emulator->regs.V[X] = i;
                        key_pressed = 1;
                        break;
                    }

                }

                if(!key_pressed)
                   *pc -= 2; //block (loop)
            }
                break;
            case 0x29:
                emulator->regs.I = emulator->regs.V[X] * 0x5 + FONT_LOAD_LOCATION;
                break;
            default:
                printf("DEFAULT: Instr: 0x%x\n", instr);
                assert(0);
            }

            break;
    default:
        printf("DEFAULT!: Instr: 0x%x -- %d\n", instr, N);
        assert(0);
    }

    dbgprintf("\n");

    if(DEBUG)
        emulator_dump_registers(emulator);

    return 0;
}

void emulator_step(Emulator* emulator)
{
    emulator->pc += 2;
}

void* emulator_timers_thread(Emulator* emulator)
{
    printf("STARTED TIMERS THREAD\n");
    while(emulator->is_on)
    {
        if(emulator->delay_timer > 0)
        {
            dbgprintf("lower timer!\n");
            --emulator->delay_timer;
        }

        if(emulator->sound_timer > 0)
            --emulator->sound_timer;

        if(!DEBUG)
            usleep(1000000 / TIMERS_THREAD_FREQUENCY);
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
