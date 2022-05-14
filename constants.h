#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "font_types.h"
#define SHIPS_LENGTH 2

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
const int ships[SHIPS_LENGTH];

unsigned short *fb;
font_descriptor_t *fdes;
#endif