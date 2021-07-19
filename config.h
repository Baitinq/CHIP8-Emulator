#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DEBUG 1

#define SCREEN_WIDTH 852
#define SCREEN_HEIGHT 480

#define INSTRUCTIONS_PER_SECOND 700
#define GAME_LOAD_LOCATION 0x200
#define FONT_LOAD_LOCATION 0x50

#define dbgprintf(fmt, ...) \
     do { if(DEBUG) fprintf (stdout, fmt, ##__VA_ARGS__); } while(0)

#endif
