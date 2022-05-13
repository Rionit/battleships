/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  change_me.c      - main file

  include your name there and license for distribution.

  Remove next text: This line should not appear in submitted
  work and project name should be change to match real application.
  If this text is there I want 10 points subtracted from final
  evaluation.

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

void draw_grid()
{
  for (int y = 0; y < GRID_SIZE; y++)
  {
    for (int x = 0; x < GRID_SIZE; x++)
    {
      unsigned int color = x % 25 && y % 25 ? dark_green : light_green;
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

void redraw_ship(int *curIdx, int *curIdy, int rotation, int length)
{

  if (!rotation && *curIdy + length > 10)
  {
    (*curIdy)--;
  }
  if (rotation && *curIdx + length > 10)
  {
    (*curIdx)--;
  }

  for (size_t i = 0; i < length; i++)
  {

    if (rotation)
    {
      fill_board_box(*curIdx + i, *curIdy, SHIP);
    }
    else
    {
      fill_board_box(*curIdx, *curIdy + i, SHIP);
    }
  }
}

void draw_board(int gameBoard[10][10], int curx, int cury)
{
  for (size_t i = 0; i < 10; i++)
  {
    for (size_t j = 0; j < 10; j++)
    {
      // printf("%d\t", gameBoard[i][j]);
      fill_board_box(j, i, gameBoard[i][j]);
    }
    // printf("\n");
  }
  // printf("\n");
  // fill_board_box(curx, cury, gameBoard[cury][curx] + 500);
  highlight_box(curx, cury);
}

void delay(int msec)
{
  struct timespec wait_delay = {.tv_sec = msec / 1000,
                                .tv_nsec = (msec % 1000) * 1000 * 1000};
  clock_nanosleep(CLOCK_MONOTONIC, 0, &wait_delay, NULL);
}

void place_ship(int *curIdx, int *curIdy, int rotation, int length, int gameBoard[10][10])
{
  printf("rotation: %d", rotation);
  for (size_t i = 0; i < length; i++)
  {

    if (rotation)
    {
      printf(" JSEM V 1\n");
      gameBoard[*curIdy][*curIdx + i] = SHIP;
    }
    else
    {
      printf(" JSEM V 0\n");
      gameBoard[*curIdy + i][*curIdx] = SHIP;
    }
  }

  draw_board(gameBoard, *curIdx, *curIdy);
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

int get_cursor(int *curKnob, int prevKnob, int *cur, bool modulate)
{

  int dif = prevKnob - *curKnob;
  if (dif > 200)
  {
    dif = -(255 - (prevKnob - *curKnob));
  }
  else if (dif < -200)
  {
    dif = 255 - (*curKnob - prevKnob);
  }

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

bool is_occupied(int x, int y, int gameBoard[10][10])
{
  return gameBoard[y][x] == SHIP;
}

bool can_place(int curIdx, int curIdy, int rotation, int length, int gameBoard[10][10])
{
  for (size_t i = 0; i < length; i++)
  {
    int incX, incY;
    if (rotation)
    {
      incX = i;
      incY = 0;
    }
    else
    {
      incX = 0;
      incY = i;
    }
    if (is_occupied(curIdx + incX, curIdy + incY, gameBoard) || is_occupied(curIdx + incX + 1, curIdy + incY, gameBoard) || is_occupied(curIdx + incX - 1, curIdy + incY, gameBoard) || is_occupied(curIdx + incX, curIdy + incY + 1, gameBoard) || is_occupied(curIdx + incX, curIdy + incY - 1, gameBoard))
    {
      return false;
    }
  }

  return true;
}

void setup_board(int gameBoard[10][10])
{

  unsigned char *mem_base;
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

  for (size_t i = 0; i < SHIPS_LENGTH; i++)
  {

    uint32_t knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    int curx, cury;
    int curKnobx, curKnoby;
    curKnobx = (knobs & 0xff);
    curKnoby = ((knobs >> 8) & 0xff);
    curx = 0;
    cury = 0;
    int curRot = 0;

    while (((knobs & 0x7000000) == 0) || !can_place(curx, cury, curRot, ships[i], gameBoard))
    {
      // knobama menim barvu rgb ledky
      knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
      // modry knob urcuje pozici ledky

      int rotation = (int)(((knobs >> 16) & 0xff) / 26) % 2;

      if (get_cursor(&curKnobx, (knobs & 0xff), &curx, false) || get_cursor(&curKnoby, ((knobs >> 8) & 0xff), &cury, false) || rotation != curRot)
      {
        curRot = rotation;
        draw_board(gameBoard, curx, cury);
        redraw_ship(&curx, &cury, curRot, ships[i]);
        draw_lcd();
      }
    }

    place_ship(&curx, &cury, curRot, ships[i], gameBoard);

    while ((knobs & 0x7000000) != 0)
    {
      knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    }
    delay(10);
  }
}

void initial_draw(int gameBoard[10][10])
{
  draw_grid();
  draw_grid_chars();
  draw_board(gameBoard, 0, 0);
  draw_lcd();
}

int main(int argc, char *argv[])
{

  int gameBoard[10][10];

  printf("RED: %d\n", hsv2rgb_lcd(360, 255, 225));
  printf("WHITE: %d\n", hsv2rgb_lcd(180, 100, 225));

  for (int i = 0; i < 10; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      gameBoard[i][j] = SEA;
      //"[%d, %d]: %d\n", i, j, gameBoard[i][j]);
    }
  }

  unsigned char *mem_base;
  uint32_t knobs;

  // unsigned int c;
  fb = (unsigned short *)malloc(320 * 480 * 2); // frame buffer

  printf("Hello world\n");
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

  if (parlcd_mem_base == NULL || mem_base == NULL)
    exit(1);
  parlcd_hx8357_init(parlcd_mem_base);

  initial_draw(gameBoard);
  setup_board(gameBoard);

  knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  int curx, cury;
  int curKnobx, curKnoby;
  curKnobx = (knobs & 0xff);
  curKnoby = ((knobs >> 8) & 0xff);
  curx = 0;
  cury = 0;
  while (1)
  {
    // knobama menim barvu rgb ledky
    knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    // modry knob urcuje pozici ledky
    if (get_cursor(&curKnobx, (knobs & 0xff), &curx, true) || get_cursor(&curKnoby, ((knobs >> 8) & 0xff), &cury, true))
    {
      // printf("curx:%d cury:%d\n", curx, cury);
      draw_board(gameBoard, curx, cury);
      draw_lcd();
    }

    if ((knobs & 0x7000000) != 0)
    {
      draw_board(gameBoard, curx, cury);
      *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = 0xFF0000;
      *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = 0xFF0000;

      printf("BANG!!!!!!!\n");
      switch (gameBoard[cury][curx])
      {
      case SEA:
        fill_board_box(curx, cury, SEA_HIT);
        gameBoard[cury][curx] = SEA_HIT;
        break;
      case SHIP:
        fill_board_box(curx, cury, SHIP_HIT);
        gameBoard[cury][curx] = SHIP_HIT;
        break;
      }

      draw_lcd();
    }
    else
    {
      *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = 0;
      *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = 0;
    }
  }

  printf("Goodbye world\n");

  return 0;
}
