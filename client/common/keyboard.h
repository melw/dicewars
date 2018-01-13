#define KBD_NUM_BUTTONS 51
#define KBD_CAPS 0x01
#define KBD_BACKSPACE 0x08
#define KBD_ENTER 0x13
#define KBD_SHIFT 0x02

#include "../data/kbd_array.h"

char KBD_LOWER_BUTTONS[KBD_NUM_BUTTONS] = { 
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', KBD_BACKSPACE,
	KBD_CAPS, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', KBD_ENTER,
	KBD_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
	';', '\'', ' ', '[', ']'
};

// TODO: Add $, { and } to fonts
char KBD_UPPER_BUTTONS[KBD_NUM_BUTTONS] = { 
//	'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	'!', '@', '#', 'S', '%', '^', '&', '*', '(', ')', '_', '+',
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', KBD_BACKSPACE,
	KBD_CAPS, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', KBD_ENTER,
	KBD_SHIFT, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
//	':', '~', ' ', '{', '}'
	':', '~', ' ', '(', ')'
};

int KBD_BUTTON_X[KBD_NUM_BUTTONS];
int KBD_BUTTON_Y[KBD_NUM_BUTTONS];
int KBD_BUTTON_WIDTH[KBD_NUM_BUTTONS];
int KBD_BUTTON_HEIGHT[KBD_NUM_BUTTONS];

int kbdDown = 255;
int kbdAnchorX = (X_RES-IMAGE_KBD_LOWER_WIDTH)>>1;
int kbdAnchorY = Y_RES-IMAGE_KBD_LOWER_HEIGHT-4;
boolean kbdShift = false;
boolean kbdCaps = false;
boolean kbdInited = false;
char* kbdString;
int kbdIndex = 0;
boolean kbdUpdated = true;
int kbdPlayerIndex = 0;

void kbdPressed(int x, int y);
void kbdPaint();
void kbdLogic();

