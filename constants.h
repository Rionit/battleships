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

const int sizeX;
const int sizeY;
const int GRID_SIZE;
const int BOX_COUNT;
const int BOARD_LEN;
const int BOX_SIZE;
const int LINE_MOD;
const int scale;
const int startX;
const int startY;
const int knob_precision;
const int dark_green;
const int light_green;
const int ships[TOTAL_SHIPS];

unsigned short *fb;
font_descriptor_t *fdes;
#endif