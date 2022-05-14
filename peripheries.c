#include "utils.h"
#include "constants.h"
#include <stdio.h>
#include <unistd.h>
#include "mzapo_phys.h"
#include "mzapo_regs.h"

void led_line_left()
{
    for (int i = 0; i < 30; i++)
    {
        uint32_t val_line = (1 << i);
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_LINE_o) = val_line;

        delay(50);
        // val_line = (1 << i) | val_line;
    }
}

void led_line_right()
{
    for (size_t i = 29; i > 0; i--)
    {
        uint32_t val_line = (1 << i);
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_LINE_o) = val_line;

        delay(50);
    }
}

void warning_led_police(int colorLeft, int colorRight, int intensity, int n)
{
    for (size_t i = 0; i < n; i++)
    {
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = colorLeft;
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = 0;
        delay(intensity);
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = colorRight;
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = 0;
        delay(intensity);
    }
}

void warning_led(int color, int intensity, int n)
{
    for (size_t i = 0; i < n; i++)
    {
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = color;
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = color;
        delay(intensity);
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = 0;
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = 0;
        delay(intensity);
    }
}

void turn_led_red()
{
    *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = 0xFF0000;
    *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = 0xFF0000;
}

void turn_led_green()
{
    *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = 0x00FF00;
    *(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = 0x00FF00;
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
