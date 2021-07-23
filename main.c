#include <emulator.h>
#include <SDL.h>

int main(int argc, char** argv);
void show_help();

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        show_help();
        return 1;
    }


    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("CHIP8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    Emulator emulator;

    emulator_initialise(&emulator);

    load:
    if(emulator_load_rom(&emulator, argv[1]) != 0)
        return 2;

    printf("Hello brother!\n");

    uint32_t pixels[32 * 64];
    SDL_Event event;
    while(emulator.is_on == 1)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                goto exit;
            if(event.type == SDL_WINDOWEVENT)
                SDL_RenderPresent(renderer);
            if(event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        goto exit;
                    case SDLK_F5:
                        goto load;
                    default:
                        emulator_handle_key_press(&emulator, event.key.keysym.sym);
                }

            }
            if(event.type == SDL_KEYUP)
                emulator_handle_key_release(&emulator, event.key.keysym.sym);
        }

        emulator_tick(&emulator);

        if(emulator.draw_flag == 1)
        {
            //update pixel buffer with the emulator's display pixels
            for(int w = 0; w < 64; ++w)
            {
                for(int h = 0; h < 32; ++h)
                {
                    uint8_t pixel = emulator.display[w][h];
                    pixels[64 * h + w] = (FOREGROUND_COLOR * pixel) | BLACKGROUND_COLOR;
                }
            }

            SDL_UpdateTexture(texture, NULL, pixels, sizeof(uint32_t) * 64);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            emulator.draw_flag = 0;
        }

        //usleep(1000000 / INSTRUCTIONS_PER_SECOND);
    }

    exit:
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void show_help()
{
    printf("BAD USAGE! -> ./chip8_emulator [ROM]\n");
}
