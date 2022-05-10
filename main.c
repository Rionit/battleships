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

#include "utils.h"
#include "constants.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

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

int main(int argc, char *argv[])
{
  unsigned char *parlcd_mem_base;
  int ptr;

  // unsigned int c;
  fb = (unsigned short *)malloc(320 * 480 * 2); // frame buffer

  printf("Hello world\n");
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);

  if (parlcd_mem_base == NULL)
    exit(1);
  parlcd_hx8357_init(parlcd_mem_base);
  draw_grid();

  char *first_row = "ABCDEFGHIJ\0";
  fdes = &font_rom8x16;
  for (int x = 1; x < 12; x++)
  {
    draw_char((x * 24) + (24 - char_width(first_row[x - 1])) / 2 + startX + x + 1, startY, first_row[x - 1], light_green);
  }

  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (ptr = 0; ptr < 480 * 320; ptr++)
  {
    parlcd_write_data(parlcd_mem_base, fb[ptr]);
  }

  while (1)
  {
  };

  printf("Goodbye world\n");

  return 0;
}

