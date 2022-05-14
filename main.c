/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  change_me.c      - main file

  include your name there and license for distribution.

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include "utils.h"
#include "constants.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

enum STATES
{
  SEA = 384,
  SHIP = 40928,
  SEA_HIT = 36636,
  SHIP_HIT = 57344
};

typedef struct
{
  short curx;
  short cury;
  short knobX;
  short knobY;
  bool vertical;
  int (*gameBoard)[10];
} game_info;

unsigned char *mem_base;

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

void redraw_ship(game_info info, int length)
{
  for (size_t i = 0; i < length; i++)
  {
    if (info.vertical)
      fill_board_box(info.curx + i, info.cury, SHIP);
    else
      fill_board_box(info.curx, info.cury + i, SHIP);
  }
}

void draw_board_cur(game_info info)
{
  draw_board(info.gameBoard);
  highlight_box(info.curx, info.cury);
}

void delay(int msec)
{
  struct timespec wait_delay = {.tv_sec = msec / 1000,
                                .tv_nsec = (msec % 1000) * 1000 * 1000};
  clock_nanosleep(CLOCK_MONOTONIC, 0, &wait_delay, NULL);
}

void place_ship(game_info info, int length)
{
  printf("vertical: %d", info.vertical);
  for (size_t i = 0; i < length; i++)
  {
    if (info.vertical)
      info.gameBoard[info.cury][info.curx + i] = SHIP;
    else
      info.gameBoard[info.cury + i][info.curx] = SHIP;
  }
}

unsigned char *parlcd_mem_base;
void draw_lcd()
{
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (int ptr = 0; ptr < 480 * 320; ptr++)
  {
    parlcd_write_data(parlcd_mem_base, fb[ptr]);
  }
}

int set_cursor(short *curKnob, short prevKnob, short *cur, bool modulate)
{

  int dif = prevKnob - *curKnob;
  if (dif > 200)
    dif = -(255 - (prevKnob - *curKnob));
  else if (dif < -200)
    dif = 255 - (*curKnob - prevKnob);

  if (dif > knob_precision || dif < -knob_precision)
  {
    *curKnob = prevKnob;
    if (modulate)
    {
      *cur = ((*cur + (dif > 0 ? -1 : +1)) % 10);
      *cur += *cur < 0 ? 10 : 0;
    }
    else
    {
      *cur = (*cur + (dif > 0 ? -1 : +1));
      *cur = *cur < 0 ? 0 : *cur;
      *cur = *cur > 9 ? 9 : *cur;
    }
    return true;
  }
  return false;
}

bool can_place(game_info info, int length)
{
  short x = info.curx;
  short y = info.cury;
  for (size_t i = 0; i < length; i++)
  {
    if (info.vertical)
      x++;
    else
      y++;
    if (info.gameBoard[y][x] == SHIP || info.gameBoard[y][x + 1] == SHIP || info.gameBoard[y][x - 1] == SHIP || info.gameBoard[y + 1][x] == SHIP || info.gameBoard[y - 1][x] == SHIP)
      return false;
  }
  return true;
}

void setup_board(game_info info)
{
  for (size_t i = 0; i < SHIPS_LENGTH; i++)
  {
    info.curx = 0;
    info.cury = 0;
    redraw_ship(info, ships[i]);

    uint32_t knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    // break when knob is pressed and ship can be placed
    while (((knobs & 0x7000000) == 0) || !can_place(info, ships[i]))
    {
      uint32_t knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o); // read knobs
      bool vertical = (bool)(((knobs >> 16) & 0xff) / 26) % 2;                     // ship rotation

      // Moved with cursor or rotated ship?
      if (set_cursor(&(info.knobX), (knobs & 0xff), &(info.curx), false) || set_cursor(&(info.knobY), ((knobs >> 8) & 0xff), &(info.cury), false) || vertical != info.vertical)
      {
        info.vertical = vertical;
        // Block moving when ship touches the border
        if (!vertical && info.cury + ships[i] >= BOX_COUNT)
          info.cury--;
        if (vertical && info.curx + ships[i] >= BOX_COUNT)
          info.curx--;
        draw_board(info.gameBoard);
        redraw_ship(info, ships[i]);
        draw_lcd();
      }
    }

    place_ship(info, ships[i]);
    draw_board(info.gameBoard);

    // wait for player to unpress the knob
    while ((knobs & 0x7000000) != 0)
    {
      knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    }
    delay(10);
  }
}

void initial_draw(int (*gameBoard)[10])
{
  draw_grid();
  draw_grid_chars();
  draw_board(gameBoard);
  highlight_box(0, 0);
  draw_lcd();
}

game_info setup()
{
  int *gameBoard_p = malloc(BOARD_LEN * BOARD_LEN * sizeof(int));
  int(*gameBoard)[BOARD_LEN] = (int(*)[BOARD_LEN])gameBoard_p;
  for (int i = 0; i < 10; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      gameBoard[i][j] = SEA;
    }
  }
  uint32_t knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  // init info
  game_info info = (game_info){0, 0, knobs & 0xff, (knobs >> 8) & 0xff, 0, gameBoard};

  // frame buffer
  fb = (unsigned short *)malloc(320 * 480 * 2);

  // setup display
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  if (parlcd_mem_base == NULL || mem_base == NULL)
    exit(1);
  parlcd_hx8357_init(parlcd_mem_base);

  // game board setup
  setup_board(info);
  initial_draw(info.gameBoard);
  return info;
}

int main(int argc, char *argv[])
{
  game_info info = setup();
  // printf("RED: %d\n", hsv2rgb_lcd(360, 255, 225));
  // printf("WHITE: %d\n", hsv2rgb_lcd(180, 100, 225));

  while (1)
  {
    uint32_t knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    // Knobs rotated? -> cursor changed
    if (set_cursor(&(info.knobX), knobs & 0xff, &(info.curx), true) || set_cursor(&(info.knobY), (knobs >> 8) & 0xff, &(info.cury), true))
    {
      // printf("curx:%d cury:%d\n", curx, cury);
      draw_board_cur(info);
      draw_lcd();
    }

    // Knobs pressed?
    if ((knobs & 0x7000000) != 0)
    {
      // red RGB leds
      *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = 0xFF0000;
      *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = 0xFF0000;

      // Shoot
      printf("BANG!!!!!!!\n");
      int *box = &(info.gameBoard[info.cury][info.curx]);
      switch (*box)
      {
      case SEA:
        *box = SEA_HIT;
        break;
      case SHIP:
        *box = SHIP_HIT;
        break;
      }
      draw_board_cur(info);
      draw_lcd();
    }
    else
    {
      // Turn off LEDs
      *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = 0;
      *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = 0;
    }
  }

  printf("Goodbye world\n");

  return 0;
}
