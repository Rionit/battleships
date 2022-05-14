#include "constants.h"
#include <stdbool.h>
void highlight_box(int idxX, int idxY);
void fill_box(int idxX, int idxY, int color);
void fill_board_box(int idxX, int idxY, int color);
void draw_board(int (*gameBoard)[10]);
int get_coord_x(int idx);
int get_coord_y(int idx);
bool flood_filled(int (*board)[10], short x, short y);
bool check_side(int (*board)[10], int x, int y, int incX, int incY, int *length);

// from APO templates
void delay(int msec);
unsigned int hsv2rgb_lcd(int hue, int saturation, int value);
void draw_pixel(int x, int y, unsigned short color);
void draw_pixel_big(int x, int y, unsigned short color);
int char_width(int ch);
void draw_char(int x, int y, char ch, unsigned short color);
unsigned int hsv2rgb_lcd(int hue, int saturation, int value);