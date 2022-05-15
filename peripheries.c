#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "utils.h"
#include "constants.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

unsigned char *parlcd_mem_base;
unsigned char *mem_base;

void setup_peripheries()
{
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    // lcd
    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL || mem_base == NULL)
        exit(1);
    parlcd_hx8357_init(parlcd_mem_base);
}

void draw_lcd()
{
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (int ptr = 0; ptr < 480 * 320; ptr++)
    {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }
}

uint32_t get_knobs()
{
    return *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
}

void led_line_left()
{
    for (int i = 0; i < 32; i++)
    {
        uint32_t val_line = 1 << i;
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
        delay(50);
    }
    *(volatile uint32_t *)(mem_base + SPILED_REG_LED_LINE_o) = 0;
}

void led_line_right()
{
    for (size_t i = 31; i > 0; i--)
    {
        uint32_t val_line = 1 << i;
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
        delay(50);
    }
    *(volatile uint32_t *)(mem_base + SPILED_REG_LED_LINE_o) = 0;
}

void turn_led_color(int color)
{
    *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = color;
    *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = color;
}

void warning_led(int color, int intensity, int n)
{
    for (size_t i = 0; i < n; i++)
    {
        turn_led_color(color);
        delay(intensity);
        turn_led_color(0);
        delay(intensity);
    }
}

void turn_led_off()
{
    turn_led_color(0);
}

void turn_led_red()
{
    turn_led_color(0xFF0000);
}

void turn_led_green()
{
    turn_led_color(0x00FF00);
}

void red_warning_led()
{
    warning_led(0xFF0000, 200, 5);
}

void blue_warning_led()
{
    warning_led(0x0000FF, 200, 5);
}

void green_warning_led()
{
    warning_led(0x00FF00, 200, 5);
}

void react_to_cell(int cell, bool enemy)
{
    switch (cell)
    {
    case SHIP_HIT:
        red_warning_led();
        break;
    case SEA_HIT:
        blue_warning_led();
        break;
    case SUNKEN_SHIP:
        enemy ? green_warning_led() : warning_led(0xFF0000, 500, 2);
        break;
    }
}
