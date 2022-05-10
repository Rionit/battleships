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

void draw_grid_chars(){
  char *first_row = "ABCDEFGHIJ\0";
  fdes = &font_rom8x16;
  for(int x = 1; x < 11; x++){
  
  draw_char(get_coord_x(x) + (BOX_SIZE / 2) - (char_width(first_row[x-1])*scale / 2) + scale, get_coord_y(0) - 1 , first_row[x-1], light_green);
  draw_char(get_coord_x(0) + (char_width((char)(x+47))*scale / 2) - scale, 
    get_coord_y(x) + (BOX_SIZE / 2) - (16*scale / 2) + scale, (char)(x+47), light_green);
  
  } 
}

void redraw_cursor(int *curIdx, int *curIdy, int newIdx, int newIdy){
  fill_box(*curIdx, *curIdy, dark_green);
  fill_box(newIdx, newIdy, hsv2rgb_lcd(145,255, 125));
  *curIdx = newIdx; 
  *curIdy = newIdy;
}

void initial_draw(){
  draw_grid();
  draw_grid_chars();
}

unsigned char *parlcd_mem_base;
void draw_lcd(){
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
      for (int ptr = 0; ptr < 480 * 320; ptr++)
      {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
      }
}

int main(int argc, char *argv[])
{
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

  initial_draw();

  int curx, cury;
  curx=0;
  cury=0;
  while (1)
  {
    // knobama menim barvu rgb ledky
     knobs = *(volatile uint32_t*) (mem_base + SPILED_REG_KNOBS_8BIT_o);      
     // modry knob urcuje pozici ledky
     int idx = (int)((knobs&0xff)/26) + 1;
     int idy = (int)(((knobs>>8)&0xff)/26) + 1;
     if(idx != curx || idy != cury){
      printf("%d %d %d %d\n", idx, idy, curx, cury); 
      redraw_cursor(&curx, &cury, idx, idy);
      draw_lcd();
     }
      if((knobs&0x7000000)!=0){
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = 0xFF0000;
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = 0xFF0000;
        printf("BANG!!!!!!!\n");
        fill_box(curx, cury, hsv2rgb_lcd(360, 255, 255));
        draw_lcd();
      }else{
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = 0;
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = 0;
      }
  }

  printf("Goodbye world\n");

  return 0;
}

