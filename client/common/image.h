#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

#define IMAGE_1BIT 0
#define IMAGE_ALPHA 1
#define IMAGE_PALETTE 2

#define MAX_IMAGES 255

#include "../data/image_logo.h"
#include "../data/image_data_bg.h"
#include "../data/image_game_bg.h"
#include "../data/image_dice.h"
#include "../data/image_dice_small.h"
#include "../data/image_dice_icon.h"
#include "../data/image_dice_shadow.h"
#include "../data/image_dice_box.h"
#include "../data/image_endturn.h"
#include "../data/image_hexagon_icon.h"
#include "../data/image_selected.h"
#include "../data/image_main_bg.h"
#include "../data/image_about_bg.h"
#include "../data/image_instructions_bg.h"
#include "../data/image_arrows.h"
#include "../data/image_numbers.h"
#include "../data/image_numbers_box.h"
#include "../data/image_kbd_lower.h"
#include "../data/image_kbd_lower2.h"
#include "../data/image_kbd_upper.h"
#include "../data/image_kbd_upper2.h"
#include "../data/image_heads_small.h"
#include "../data/image_surrender.h"
#include "../data/image_skipturn.h"
#include "../data/image_wifi_icons.h"
#include "../data/palette_logo.h"
#include "../data/palette_data_bg.h"
#include "../data/palette_game_bg.h"
#include "../data/palette_dice.h"
#include "../data/palette_dice_small.h"
#include "../data/palette_dice_icon.h"
#include "../data/palette_dice_shadow.h"
#include "../data/palette_dice_box.h"
#include "../data/palette_endturn.h"
#include "../data/palette_hexagon_icon.h"
#include "../data/palette_selected.h"
#include "../data/palette_main_bg.h"
#include "../data/palette_about_bg.h"
#include "../data/palette_instructions_bg.h"
#include "../data/palette_arrows.h"
#include "../data/palette_numbers.h"
#include "../data/palette_numbers_box.h"
#include "../data/palette_kbd_lower.h"
#include "../data/palette_kbd_lower2.h"
#include "../data/palette_kbd_upper.h"
#include "../data/palette_kbd_upper2.h"
#include "../data/palette_heads_small.h"
#include "../data/palette_surrender.h"
#include "../data/palette_skipturn.h"
#include "../data/palette_wifi_icons.h"

#ifdef NEOFLASH
	#include "../data/image_neoflash.h"
	#include "../data/palette_neoflash.h"
#endif

int activeColor;

void setColor(int color);
int getColor();

int getInvertAlign(int align);
int getAlignX(int align, int width);
int getAlignY(int align, int height);

void drawSubImage(int index, int x, int y, int align, int width, int height, int xpos, int ypos);
void drawImage(int index, int x, int y, int align);

void drawLine(int x0, int y0, int x1, int y1);
void drawRect(int x, int y, int width, int height);
void drawHexagon(int x, int y, int width, int height, boolean horizontal);

void fillRect(int x, int y, int width, int height);
void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2);
void fillHexagon(int x, int y, int width, int height, boolean horizontal);

boolean imageLoaded[MAX_IMAGES];
SCREEN *images[MAX_IMAGES];

#endif // IMAGE_H_INCLUDED
