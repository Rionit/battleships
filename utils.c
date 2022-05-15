#include "constants.h"
#include "font_types.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int get_coord(int start, int idx)
{
    return start + (idx + 1) + idx * BOX_SIZE;
}

int get_coord_x(int idx)
{
    return get_coord(startX, idx);
}

int get_coord_y(int idx)
{
    return get_coord(startY, idx);
}

void draw_pixel(int x, int y, unsigned short color)
{
    fb[(x % sizeX) + sizeX * (y % sizeY)] = color;
}

void highlight_box(int idxX, int idxY)
{
    int boxY = get_coord_y(idxY + 1);
    int boxX = get_coord_x(idxX + 1);
    for (int y = 0; y < BOX_SIZE; y++)
    {
        for (int x = 0; x < BOX_SIZE; x++)
        {
            if (y == 0 || x == 0 || y == BOX_SIZE - 1 || x == BOX_SIZE - 1)
                draw_pixel(boxX + x, boxY + y, 65535);
        }
    }
}

void fill_box(int idxX, int idxY, int color)
{
    int boxY = get_coord_y(idxY);
    int boxX = get_coord_x(idxX);
    for (int y = 0; y < BOX_SIZE; y++)
    {
        for (int x = 0; x < BOX_SIZE; x++)
        {
            draw_pixel(boxX + x, boxY + y, color);
        }
    }
}

void fill_board_box(int idxX, int idxY, int color)
{
    fill_box(idxX + 1, idxY + 1, color);
}

void draw_board(int (*board)[10])
{
    for (size_t y = 0; y < BOARD_LEN; y++)
    {
        for (size_t x = 0; x < BOARD_LEN; x++)
        {
            fill_board_box(x, y, board[y][x]);
        }
    }
}

void draw_pixel_big(int x, int y, unsigned short color)
{
    int i, j;
    for (i = 0; i < scale; i++)
    {
        for (j = 0; j < scale; j++)
        {
            draw_pixel(x + i, y + j, color);
        }
    }
}

int char_width(int ch)
{
    int width;
    if (!fdes->width)
        width = fdes->maxwidth;
    else
        width = fdes->width[ch - fdes->firstchar];
    return width;
}

void draw_char(int x, int y, char ch, unsigned short color)
{
    int w = char_width(ch);
    const font_bits_t *ptr;
    if ((ch >= fdes->firstchar) && (ch - fdes->firstchar < fdes->size))
    {
        if (fdes->offset)
        {
            ptr = &fdes->bits[fdes->offset[ch - fdes->firstchar]];
        }
        else
        {
            int bw = (fdes->maxwidth + 15) / 16;
            ptr = &fdes->bits[(ch - fdes->firstchar) * bw * fdes->height];
        }
        int i, j;
        for (i = 0; i < fdes->height; i++)
        {
            font_bits_t val = *ptr;
            for (j = 0; j < w; j++)
            {
                if ((val & 0x8000) != 0)
                    draw_pixel_big(x + scale * j, y + scale * i, color);
                val <<= 1;
            }
            ptr++;
        }
    }
}

void draw_string(int x, int y, char *string, int color)
{
    int offset = 0;
    for (size_t i = 0; i < strlen(string); i++)
    {
        draw_char(x + offset, y, string[i], color);
        offset += char_width(string[i]) + 10;
    }
}

int string_width(int spaceLen, char *string)
{
    int width = 0;
    for (size_t i = 0; i < strlen(string); i++)
    {
        width += char_width(string[i]) + 10;
    }
    return width;
}

void delay(int msec)
{
    struct timespec wait_delay = {.tv_sec = msec / 1000,
                                  .tv_nsec = (msec % 1000) * 1000 * 1000};
    clock_nanosleep(1, 0, &wait_delay, NULL);
}

void draw_grid()
{
    for (int y = 0; y < GRID_SIZE; y++)
    {
        for (int x = 0; x < GRID_SIZE; x++)
        {
            unsigned int color = x % LINE_MOD && y % LINE_MOD ? SEA : SHIP;
            draw_pixel(startX + x, startY + y, color);
        }
    }
}

void draw_grid_chars()
{
    char *first_row = "ABCDEFGHIJ\0";
    fdes = &font_rom8x16;
    for (int x = 1; x < 11; x++)
    {
        draw_char(get_coord_x(x) + (BOX_SIZE / 2) - (char_width(first_row[x - 1]) * scale / 2) + scale, get_coord_y(0) - 1, first_row[x - 1], light_green);
        draw_char(get_coord_x(0) + (char_width((char)(x + 47)) * scale / 2) - scale,
                  get_coord_y(x) + (BOX_SIZE / 2) - (16 * scale / 2) + scale, (char)(x + 47), light_green);
    }
}

void clear_place(int startx, int starty, int endx, int endy)
{
    for (size_t i = startx; i < endx; i++)
    {
        for (size_t j = starty; j < endy; j++)
        {
            draw_pixel(i, j, 0);
        }
    }
}

void clear_screen()
{
    clear_place(0, 0, 480, 320);
}

//------ FLOODFILL -------

bool check_side(int (*board)[10], short x, short y, int incX, int incY, int *length)
{
    y += incY == 0 ? 0 : incY;
    x += incX == 0 ? 0 : incX;
    while ((y <= 9) && (y >= 0) && (x <= 9) && x >= 0 && board[y][x] != SEA && board[y][x] != SEA_HIT)
    {
        if (board[y][x] == SHIP)
            return false;
        *length = *length + 1;
        y += incY == 0 ? 0 : incY;
        x += incX == 0 ? 0 : incX;
    }
    return true;
}

bool flood_filled(int (*board)[10], short x, short y)
{
    printf("flood filling\n");

    bool horizontalLeft = (((x - 1) >= 0) && (board[y][x - 1] == SHIP || board[y][x - 1] == SHIP_HIT));
    bool horizontalRight = (((x + 1) <= 9) && (board[y][x + 1] == SHIP || board[y][x + 1] == SHIP_HIT));
    bool horizontal = horizontalLeft || horizontalRight;
    int incX = horizontal ? 1 : 0;
    int incY = horizontal ? 0 : 1;

    int len1, len2;
    len1 = len2 = 0;

    if (!check_side(board, x, y, incX, incY, &len1) || !check_side(board, x, y, -incX, -incY, &len2))
        return false;

    if (incX == 1)
    {
        for (size_t i = x - len2; i <= x + len1; i++)
        {
            board[y][i] = SUNKEN_SHIP;
        }
    }
    else
    {
        for (size_t i = y - len2; i <= y + len1; i++)
        {
            board[i][x] = SUNKEN_SHIP;
        }
    }
    return true;
}