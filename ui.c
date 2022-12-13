/*******************************************************************
  Battleships - semestral project on MZ_APO board.

  ui.c      - c file for implementing game UI (main menu and game over screens)

  Kryštof Gärtner, Filip Doležal

 *******************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "constants.h"
#include "peripheries.h"
#include "utils.h"

#define CENTERED_SCREEN_X(size) ((480 / 2) - (size / 2))

bool main_menu()
{
  char *textsel = "SELECT PLAYER";
  char *textp1 = "PLAYER 1";
  char *textp2 = "PLAYER 2";

  int selectedColor = light_green;
  int defaultColor = dark_green;

  draw_string(CENTERED_SCREEN_X(string_width(10, textsel)), 122, textsel, light_green);
  bool player = false;
  uint32_t knobs = get_knobs();

  // while button is not pressed
  while ((knobs & 0x7000000) == 0)
  {
    knobs = get_knobs();
    // choose player by rotating first knob in any direction
    player = (bool)((((knobs >> 16) & 0xff) / 27) % 2);
    draw_string(CENTERED_SCREEN_X(string_width(10, textp1)), 158, textp1, player ? selectedColor : defaultColor);
    draw_string(CENTERED_SCREEN_X(string_width(10, textp2)), 184, textp2, player ? defaultColor : selectedColor);
    draw_lcd();
  } // WAIT FOR RELEASE
  while ((knobs & 0x7000000) != 0)
    knobs = get_knobs();

  // draws text in top right corner
  draw_lcd();
  return player;
}

void game_over(int state)
{
  clear_screen();

  char *textgame = "GAME OVER";

  char *text;
  // game ended
  if (state == WON)
  {
    green_warning_led();
    turn_led_green();
    text = "YOU WON!";
    printf("YAAAY CHCIPL BAST*RD!!\n");
  }
  else
  {
    red_warning_led();
    turn_led_red();
    text = "YOU LOST!";
    printf("kur*a\n");
  }

  draw_char(CENTERED_SCREEN_X(char_width(state)), 122, state, light_green);
  draw_string(CENTERED_SCREEN_X(string_width(10, textgame)), 96, textgame, light_green);
  draw_string(CENTERED_SCREEN_X(string_width(10, text)), 150, text, light_green);
  draw_lcd();

  // Wait for user to click to end the program
  while ((get_knobs() & 0x7000000) == 0)
  {
  }
}
