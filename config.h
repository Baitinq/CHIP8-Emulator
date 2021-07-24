#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DEBUG 1
//#define STOP_ON_INFINITE_LOOP

#define SCREEN_WIDTH 852
#define SCREEN_HEIGHT 480

#define INSTRUCTIONS_PER_SECOND 700 //Default: 700
#define TIMERS_THREAD_FREQUENCY 60 //Default: 60
#define GAME_LOAD_LOCATION 0x200
#define FONT_LOAD_LOCATION 0x50

#define FOREGROUND_COLOR 0x00F00FFF //Default: 0x00FFFFFF
#define BLACKGROUND_COLOR 0xFF000000 //Default: 0xFF000000

#define dbgprintf(fmt, ...) \
     do { if(DEBUG) fprintf (stdout, fmt, ##__VA_ARGS__); } while(0)

#endif
