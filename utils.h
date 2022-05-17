#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>

void highlight_box(int idxX, int idxY);
void fill_box(int idxX, int idxY, int color);
void fill_board_box(int idxX, int idxY, int color);
void draw_board(int (*board)[10]);
void clear_screen();
void clear_place(int startx, int starty, int endx, int endy);
int get_coord_x(int idx);
int get_coord_y(int idx);

int string_width(int spaceLen, char *string);
void draw_string(int x, int y, char *string, int color);
void draw_grid_chars();
void draw_grid();

// floodfill
bool flood_filled(int (*board)[10], short x, short y);
bool check_side(int (*board)[10], int x, int y, int incX, int incY, int *length);

// from APO templates
void delay(int msec);
void draw_pixel(int x, int y, unsigned short color);
void draw_pixel_big(int x, int y, unsigned short color);
int char_width(int ch);
void draw_char(int x, int y, char ch, unsigned short color);
#endif
