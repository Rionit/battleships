void highlight_box(int idxX, int idxY);
void fill_box(int idxX, int idxY, int color);
void fill_board_box(int idxX, int idxY, int color);
int get_coord_x(int idx);
int get_coord_y(int idx);

// from APO templates
unsigned int hsv2rgb_lcd(int hue, int saturation, int value);
void draw_pixel(int x, int y, unsigned short color);
void draw_pixel_big(int x, int y, unsigned short color);
int char_width(int ch);
void draw_char(int x, int y, char ch, unsigned short color);
unsigned int hsv2rgb_lcd(int hue, int saturation, int value);