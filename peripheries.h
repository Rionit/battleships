
void setup_peripheries();
void draw_lcd();
uint32_t get_knobs();

void react_to_cell(int cell, bool enemy);

// ledline effects
void led_line_left();
void led_line_right();

// rgb leds effects
void blue_warning_led();
void red_warning_led();
void green_warning_led();
void turn_led_red();
void turn_led_green();
void turn_led_off();
void warning_led(int color, int intensity, int n);
