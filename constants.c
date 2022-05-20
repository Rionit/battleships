/*******************************************************************
  Battleships - semestral project on MZ_APO board.

  constants.c      - c file with global constants

  Kryštof Gärtner, Filip Doležal

 *******************************************************************/

const int SIZEX = 480;
const int SIZEY = 320;
const int GRID_SIZE = 276;
const int BOX_COUNT = 11;
const int BOARD_LEN = BOX_COUNT - 1;
const int BOX_SIZE = (GRID_SIZE - (BOX_COUNT + 1)) / BOX_COUNT;
const int LINE_MOD = BOX_SIZE + 1;
const int SCALE = 2;
const int STARTX = (SIZEX - GRID_SIZE) / 2;
const int STARTY = (SIZEY - GRID_SIZE) / 2;
const int KNOB_PRECISION = 10;
const int SHIPS[] = {2, 3, 4};

const int dark_green = 384;
const int light_green = 40928;

unsigned short *fb;
