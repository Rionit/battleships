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
#include <arpa/inet.h>
#include <string.h>

#include "peripheries.h"
#include "tcp.h"
#include "utils.h"
#include "constants.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

enum GAME_STATES
{
  RUNNING,
  WON,
  LOST
};

typedef struct
{
  short curx;
  short cury;
  short knobX;
  short knobY;
  bool horizontal;
  int (*board)[10];
  int (*boardEnemy)[10];
  int shipsLeftPlayer;
  int shipsLeftEnemy;
} game_info;

bool onTurn = true;

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
    draw_char(get_coord_x(x) + (BOX_SIZE / 2) - (char_width(first_row[x - 1]) * header_scale / 2) + header_scale, get_coord_y(0) - 1, first_row[x - 1], light_green, header_scale);
    draw_char(get_coord_x(0) + (char_width((char)(x + 47)) * header_scale / 2) - header_scale,
              get_coord_y(x) + (BOX_SIZE / 2) - (16 * header_scale / 2) + header_scale, (char)(x + 47), light_green, header_scale);
  }
}

void redraw_ship(game_info *info, int length)
{
  for (size_t i = 0; i < length; i++)
  {
    if (info->horizontal)
      fill_board_box(info->curx + i, info->cury, SHIP);
    else
      fill_board_box(info->curx, info->cury + i, SHIP);
  }
}

void draw_board_enemy(game_info *info)
{
  draw_board(info->boardEnemy);
  highlight_box(info->curx, info->cury);
}

void place_ship(game_info *info, int length)
{
  printf("horizontal: %d", info->horizontal);
  for (size_t i = 0; i < length; i++)
  {
    if (info->horizontal)
      info->board[info->cury][info->curx + i] = SHIP;
    else
      info->board[info->cury + i][info->curx] = SHIP;
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

bool can_place(game_info *info, int length)
{
  short x = info->curx;
  short y = info->cury;
  printf("\nx: %d, y: %d\n", x, y);
  for (size_t i = 0; i < length; i++)
  {
    if (info->horizontal)
      x = info->curx + i;
    else
      y = info->cury + i;

    printf("x+i: %d, y+i: %d\n", x, y);
    bool l = (x - 1 >= 0) ? (info->board[y][x - 1] == SHIP) : false;
    bool r = (x + 1 <= 9) ? (info->board[y][x + 1] == SHIP) : false;
    bool b = (y + 1 <= 9) ? (info->board[y + 1][x] == SHIP) : false;
    bool t = (y - 1 >= 0) ? (info->board[y - 1][x] == SHIP) : false;

    if (info->board[y][x] == SHIP || l || r || b || t)
      return false;
  }
  printf("can place!\n");
  return true;
}

void setup_board(game_info *info)
{
  for (size_t i = 0; i < TOTAL_SHIPS; i++)
  {
    info->curx = 0;
    info->cury = 0;
    redraw_ship(info, ships[i]);

    uint32_t knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    // break when knob is pressed and ship can be placed
    while (((knobs & 0x7000000) == 0) || !can_place(info, ships[i]))
    {
      knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o); // read knobs
      bool horizontal = (bool)((((knobs >> 16) & 0xff) / 27) % 2);        // ship rotation
      // Moved with cursor or rotated ship?
      if (set_cursor(&(info->knobX), (knobs & 0xff), &(info->curx), false) || set_cursor(&(info->knobY), ((knobs >> 8) & 0xff), &(info->cury), false) || horizontal != info->horizontal)
      {
        info->horizontal = horizontal;
        // Block moving when ship touches the border
        if (!horizontal && info->cury + ships[i] >= BOX_COUNT)
          info->cury--;
        if (horizontal && info->curx + ships[i] >= BOX_COUNT)
          info->curx--;
        draw_board(info->board);
        redraw_ship(info, ships[i]);
        draw_lcd();
      }
    }

    place_ship(info, ships[i]);
    draw_board(info->board);

    // wait for player to unpress the knob
    while ((knobs & 0x7000000) != 0)
    {
      knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    }
    delay(10);
  }
}

void initial_draw(int (*board)[10])
{
  draw_grid();
  draw_grid_chars();
  draw_board(board);
  highlight_box(0, 0);
  char *text = "Ahoj zKoUsKa te!!!! :)";
  draw_string(20, 100, text);
  draw_lcd();
}

void setup_connection(bool starts)
{
  if (starts)
    connect_to();
  else
    connect_from();
}

game_info setup()
{

  // frame buffer
  fb = (unsigned short *)malloc(320 * 480 * 2);

  // setup display
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  if (parlcd_mem_base == NULL || mem_base == NULL)
    exit(1);
  parlcd_hx8357_init(parlcd_mem_base);

  int *gameBoard_p = malloc(BOARD_LEN * BOARD_LEN * sizeof(int));
  int(*board)[BOARD_LEN] = (int(*)[BOARD_LEN])gameBoard_p;
  int *gameBoard_p2 = malloc(BOARD_LEN * BOARD_LEN * sizeof(int));
  int(*boardEnemy)[BOARD_LEN] = (int(*)[BOARD_LEN])gameBoard_p2;
  for (int i = 0; i < 10; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      boardEnemy[i][j] = SEA;
      board[i][j] = SEA;
    }
  }
  uint32_t knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  // init info
  game_info info = (game_info){0, 0, knobs & 0xff, (knobs >> 8) & 0xff, 0, board, boardEnemy, TOTAL_SHIPS, TOTAL_SHIPS};

  // game board setup
  setup_connection(onTurn);
  initial_draw(info.board);
  setup_board(&info);
  send_ready();
  return info;
}

int player_state(game_info *info)
{
  printf("entered player state\n");
  draw_board(info->board);
  draw_lcd();

  short shotx, shoty;
  receive_coords(&shotx, &shoty);

  int *cell = &(info->board[shoty][shotx]);
  led_line_left();
  if (*cell == SHIP && flood_filled(info->board, shotx, shoty))
  {
    printf("Naše loď potopena!\n");
    info->shipsLeftPlayer--;
  }
  else if (*cell == SHIP)
  {
    *cell = SHIP_HIT;
    printf("Naše loď zasažena!\n");
  }
  else
  {
    *cell = SEA_HIT;
    printf("HAHAHA!!! Vedle jak ta jedle!\n");
  }

  draw_board(info->board);
  draw_lcd();
  react_to_cell(*cell, false);

  send_response(*cell);
  printf("leaving player state\n");
  if (info->shipsLeftPlayer <= 0)
    return LOST;
  return RUNNING;
}

int enemy_state(game_info *info)
{
  printf("entered enemy state\n");
  draw_board_enemy(info);
  draw_lcd();
  while (1)
  {
    uint32_t knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    // Knobs rotated? -> cursor changed
    if (set_cursor(&(info->knobX), knobs & 0xff, &(info->curx), true) || set_cursor(&(info->knobY), (knobs >> 8) & 0xff, &(info->cury), true))
    {
      // printf("curx:%d cury:%d\n", curx, cury);
      draw_board_enemy(info);
      draw_lcd();
    }

    // Knobs pressed?
    if ((knobs & 0x7000000) != 0 && info->boardEnemy[info->cury][info->curx] == SEA)
    {
      while ((knobs & 0x7000000) != 0)
        knobs = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);

      // Shoot
      printf("BANG!!!!!!!\n");
      int *cell = &(info->boardEnemy[info->cury][info->curx]);
      *cell = send_coord(info->curx, info->cury);

      led_line_right();

      if (*cell == SUNKEN_SHIP)
      {
        flood_filled(info->boardEnemy, info->curx, info->cury);
        info->shipsLeftEnemy--;
      }

      draw_board_enemy(info);
      draw_lcd();

      react_to_cell(*cell, true);

      break;
    }
  }
  if (info->shipsLeftEnemy <= 0)
  {
    return WON;
  }
  printf("leaving enemy state\n");
  return RUNNING;
}

void main_menu()
{
}

int main(int argc, char *argv[])
{

  main_menu();

  game_info info = setup();

  int state = RUNNING;

  while (state == RUNNING)
  {
    printf("new turn\n");
    if (onTurn)
    {
      state = enemy_state(&info);
    }
    else
    {
      state = player_state(&info);
    }
    delay(2000);
    onTurn = !onTurn;
  }

  if (state == WON)
  {
    turn_led_green();
    draw_char(10, 50, SMILEY_FACE, light_green, 2);
    printf("YAAAY CHCIPL BASTARD!!\n");
  }
  else
  {
    turn_led_red();
    draw_char(10, 10, SAD_FACE, light_green, 2);
    printf("kurva\n");
  }
  draw_lcd();

  // printf("WHITE: %d\n", hsv2rgb_lcd(180, 100, 225));
  // printf("RED: %d\n", hsv2rgb_lcd(255, 255, 225));

  printf("Goodbye world\n");

  return 0;
}
