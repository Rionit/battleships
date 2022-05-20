/*******************************************************************
  Battleships - semestral project on MZ_APO board.

  constants.h      - header file with global constants

  Kryštof Gärtner, Filip Doležal

 *******************************************************************/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define TOTAL_SHIPS 3

// all cell states
// + color associated with that state
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

extern const int SIZEX;
extern const int SIZEY;
extern const int GRID_SIZE;
extern const int BOX_COUNT;
extern const int BOARD_LEN;
extern const int BOX_SIZE;
extern const int LINE_MOD;
extern const int SCALE;
extern const int STARTX;
extern const int STARTY;
extern const int KNOB_PRECISION;
extern const int SHIPS[TOTAL_SHIPS];

extern const int dark_green;
extern const int light_green;
extern unsigned short *fb;
#endif
