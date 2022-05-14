#include "constants.h"
#include "font_types.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define BIG_PIXEL_SCALE 2

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
    fb[(x % 480) + 480 * (y % 320)] = color;
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

void draw_board(int (*gameBoard)[10])
{
    for (size_t y = 0; y < BOARD_LEN; y++)
    {
        for (size_t x = 0; x < BOARD_LEN; x++)
        {
            fill_board_box(x, y, gameBoard[y][x]);
        }
    }
}

void draw_pixel_big(int x, int y, unsigned short color)
{
    int i, j;
    for (i = 0; i < BIG_PIXEL_SCALE; i++)
    {
        for (j = 0; j < BIG_PIXEL_SCALE; j++)
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

void draw_char(int x, int y, char ch, unsigned short color, int scale)
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
                {
                    draw_pixel_big(x + scale * j, y + scale * i, color);
                }
                val <<= 1;
            }
            ptr++;
        }
    }
}

void draw_string(int x, int y, char *string)
{
    int offset = 0;
    for (size_t i = 0; i < strlen(string); i++)
    {
        draw_char(x + offset, y, string[i], light_green, 1);
        offset += char_width(string[i]) + 10;
    }
}

unsigned int hsv2rgb_lcd(int hue, int saturation, int value)
{
    hue = (hue % 360);
    float f = ((hue % 60) / 60.0);
    int p = (value * (255 - saturation)) / 255;
    unsigned int q = (value * (255 - (saturation * f))) / 255;
    unsigned int t = (value * (255 - (saturation * (1.0 - f)))) / 255;
    unsigned int r, g, b;
    if (hue < 60)
    {
        r = value;
        g = t;
        b = p;
    }
    else if (hue < 120)
    {
        r = q;
        g = value;
        b = p;
    }
    else if (hue < 180)
    {
        r = p;
        g = value;
        b = t;
    }
    else if (hue < 240)
    {
        r = p;
        g = q;
        b = value;
    }
    else if (hue < 300)
    {
        r = t;
        g = p;
        b = value;
    }
    else
    {
        r = value;
        g = p;
        b = q;
    }
    r >>= 3;
    g >>= 2;
    b >>= 3;
    return (((r & 0x1f) << 11) | ((g & 0x3f) << 5) | (b & 0x1f));
}

void delay(int msec)
{
    struct timespec wait_delay = {.tv_sec = msec / 1000,
                                  .tv_nsec = (msec % 1000) * 1000 * 1000};
    clock_nanosleep(1, 0, &wait_delay, NULL);
}

bool check_side(int (*board)[10], short x, short y, int incX, int incY, int *length)
{

    y += incY == 0 ? 0 : incY;
    x += incX == 0 ? 0 : incX;
    while ((y <= 9) && (y >= 0) && (x <= 9) && x >= 0 && board[y][x] != SEA && board[y][x] != SEA_HIT)
    {
        if (board[y][x] == SHIP)
        {
            return false;
        }
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
    {
        return false;
    }

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

    printf("3 \n");
    return true;
}
