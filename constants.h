#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "font_types.h"
#define TOTAL_SHIPS 2

enum STATES
{
    SEA = 384,
    SHIP = 40928,
    SEA_HIT = 36636,
    SHIP_HIT = 57344,
    SUNKEN_SHIP = 14364
};

enum GAME_STATES
{
  RUNNING,
  LOST,
  WON
};

extern const int sizeX;
extern const int sizeY;
extern const int GRID_SIZE;
extern const int BOX_COUNT;
extern const int BOARD_LEN;
extern const int BOX_SIZE;
extern const int LINE_MOD;
extern const int scale;
extern const int startX;
extern const int startY;
extern const int knob_precision;
extern const int dark_green;
extern const int light_green;
extern const int ships[TOTAL_SHIPS];

extern unsigned short *fb;
extern font_descriptor_t *fdes;
#endif
