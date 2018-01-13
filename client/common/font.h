#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

#define FONT_TRUETYPE 1
#define FONT_TRUETYPE_HEIGHT 17
#define FONT_TRUETYPE_SPACE 7
#define FONT_TRUETYPE_SPACING (-1)

#define FONT_FIXED 2
#define FONT_FIXED_HEIGHT 11
#define FONT_FIXED_SPACE 3
#define FONT_FIXED_SPACING 1

#define FONT_CHARACTERS 97

static int fontCharacterWidths_TRUETYPE[FONT_CHARACTERS];
static int fontCharacterWidths_FIXED[FONT_CHARACTERS];

static int fontCharacterOffsets_TRUETYPE[FONT_CHARACTERS+1] = { 
	1,  11, 21, 31, 42, 51, 59, 71, 82, 87, 95, 105,113,128,
	139,152,162,173,184,193,203,214,224,240,250,260,269,279,
	289,297,307,316,323,333,343,348,354,363,369,383,393,403,
	413,423,430,438,445,455,464,477,487,496,504,515,523,532,
	542,552,562,572,581,592,603,608,615,624,630,639,645,655,
	660,671,682,696,705,717,726,737,754,764,777,786,797,804,
	811,819,826,839,851,859,867,874,885,893,904,914,924,929
};

static int fontCharacterOffsets_FIXED[FONT_CHARACTERS+1] = { 
	1,  5,  10, 15, 20, 24, 28,  33, 38, 40, 45, 50, 54, 60,
	65, 70,	75, 80, 85, 90, 96, 101,107,113,118,124,128,133,
	138,142,147,152,155,160,165,167,170,175,177,185,190,195,
	200,205,209,213,217,222,227,233,238,243,247,252,256,261,
	266,272,277,282,287,292,297,299,302,307,309,317,321,325,
	327,331,335,341,347,351,355,359,367,372,377,382,387,390,
	393,395,398,406,412,415,418,412,426,430,434,438,442,445
};

static char fontCharacters[FONT_CHARACTERS] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 
	'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 
	'W', 'X', 'Y', 'Z',	'a', 'b', 'c', 'd', 'e', 'f', 'g', 
	'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 
	's', 't', 'u', 'v', 'w', 'x', 'y', 'z',	'0', '1', '2', 
	'3', '4', '5', '6', '7', '8', '9', '.', ',', '?', '!', 
	'\\','-', '/', ':', '_', '+', '&', '*', '<', '>', '"', 
	'@', 'Ä', 'Ö', 'ä', 'ö', '(', ')', '\'','´', '%', '#', 
	'[', ']', '`', '~', '¨', '^', 'º', '=', ';'
};

void fontInit();
int stringWidth(int font, const char *s);
void drawString(int font, const char *s, int x, int y, int anchor);

char intbuf[10];
char *int2str(int i);

#endif // FONT_H_INCLUDED
