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

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"

#define GRID_SIZE 276

unsigned short *fb;
font_descriptor_t *fdes;
int scale=2;

void draw_pixel(int x, int y, unsigned short color) {
    fb[(x%480)+480*(y%320)] = color;
}

void draw_pixel_big(int x, int y, unsigned short color) {
  int i,j;
  for (i=0; i<scale; i++) {
    for (j=0; j<scale; j++) {
      draw_pixel(x+i, y+j, color);
    }
  }
}

int char_width(int ch) {
  int width;
  if (!fdes->width) {
    width = fdes->maxwidth;
  } else {
    width = fdes->width[ch-fdes->firstchar];
  }
  return width;
}

void draw_char(int x, int y, char ch, unsigned short color) {
  int w = char_width(ch);
  const font_bits_t *ptr;
  if ((ch >= fdes->firstchar) && (ch-fdes->firstchar < fdes->size)) {
    if (fdes->offset) {
      ptr = &fdes->bits[fdes->offset[ch-fdes->firstchar]];
    } else {
      int bw = (fdes->maxwidth+15)/16;
      ptr = &fdes->bits[(ch-fdes->firstchar)*bw*fdes->height];
    }
    int i, j;
    for (i=0; i<fdes->height; i++) {
      font_bits_t val = *ptr;
      for (j=0; j<w; j++) {
        if ((val&0x8000)!=0) {
          draw_pixel_big(x+scale*j, y+scale*i, color);
        }
        val<<=1;
      }
      ptr++;
    }
  }
}

unsigned int hsv2rgb_lcd(int hue, int saturation, int value) {
  hue = (hue%360);    
  float f = ((hue%60)/60.0);
  int p = (value*(255-saturation))/255;
  unsigned int q = (value*(255-(saturation*f)))/255;
  unsigned int t = (value*(255-(saturation*(1.0-f))))/255;
  unsigned int r,g,b;
  if (hue < 60){
    r = value; g = t; b = p;
  } else if (hue < 120) {
    r = q; g = value; b = p;
  } else if (hue < 180) {
    r = p; g = value; b = t;
  } else if (hue < 240) {
    r = p; g = q; b = value;
  } else if (hue < 300) {
    r = t; g = p; b = value;
  } else {
    r = value; g = p; b = q;
  }
  r>>=3;
  g>>=2;
  b>>=3;
  return (((r&0x1f)<<11)|((g&0x3f)<<5)|(b&0x1f));
}


int main(int argc, char *argv[]) {
  unsigned char *parlcd_mem_base;
  int i,j;
  int ptr;
  
  int dark_green = hsv2rgb_lcd(115, 255, 50);
  int light_green = hsv2rgb_lcd(83, 255, 255); 
  
  //unsigned int c;
  fb  = (unsigned short *)malloc(320*480*2); // frame buffer

  printf("Hello world\n");

  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);

  if (parlcd_mem_base == NULL)
    exit(1);
  parlcd_hx8357_init(parlcd_mem_base);
  
  
  int startX = (480 - GRID_SIZE) / 2;
  int startY = (320 - GRID_SIZE) / 2;
  for (i = 0; i < GRID_SIZE ; i++) {
    for (j = 0; j < GRID_SIZE ; j++) {
      //unsigned int color = i % 25 && j % 25 ? hsv2rgb_lcd(136, 255, 255) : hsv2rgb_lcd(0, 255, 255); 
      unsigned int color = i % 25 && j % 25 ? dark_green : light_green; 
      draw_pixel( startX + i, startY + j, color);
    }
  }
  
  char *first_row = "ABCDEFGHIJ\0";
  //fdes = &font_winFreeSystem14x16;
  fdes = &font_rom8x16;
  for(int x = 1; x < 12; x++){
  
  draw_char((x*24) + (24 - char_width(first_row[x - 1])) / 2 + startX + x + 1, startY , first_row[x- 1], light_green);
  
  }
  
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (ptr = 0; ptr < 480*320 ; ptr++) {
    parlcd_write_data(parlcd_mem_base, fb[ptr]);
  }

  
  while(1){};
  
  printf("Goodbye world\n");

  return 0;
}
