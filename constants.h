
#include "font_types.h"

int const GRID_SIZE = 276;
int const BOX_COUNT = 11;
int const BOX_SIZE = GRID_SIZE - (BOX_COUNT + 1) / BOX_COUNT;
int const LINE_MOD = BOX_SIZE + 1;
int const scale = 2;
int const startX = (480 - GRID_SIZE) / 2;
int const startY = (320 - GRID_SIZE) / 2;

int dark_green = 384;
int light_green = 40928;

unsigned short *fb;
font_descriptor_t *fdes;