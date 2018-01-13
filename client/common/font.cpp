#include "font.h"
#include "image.h"
#include "../data/font_fixed.h"
#include "../data/font_truetype.h"

void initFont()
{
	for(int i=0;i<FONT_CHARACTERS;i++)
	{
		fontCharacterWidths_FIXED[i] = fontCharacterOffsets_FIXED[i+1]-fontCharacterOffsets_FIXED[i];
		fontCharacterWidths_TRUETYPE[i] = fontCharacterOffsets_TRUETYPE[i+1]-fontCharacterOffsets_TRUETYPE[i];
	}
}

int fontCharacterIndex(char c)
{
	// 0..25 = A..Z, 26..51 = a..z, 52..61 = 0..9, 999=' '
	if (c >= 'A' && c <= 'Z') return (int)(c-'A');
	else if (c >= 'a' && c <= 'z') return (int)(c-'a')+26;
	else if (c >= '0' && c <= '9') return (int)(c-'0')+52;
	else if (c == ' ') return 999;
	else
	{
		for(int i=62;i<FONT_CHARACTERS;i++)
		{
			if(c==fontCharacters[i])
				return i;
		}
		return 0;
	}
}

char *int2str(int i)
{
	memset(intbuf, 0, 10);
	sprintf(intbuf, "%d", i);
	return intbuf;
}

int stringWidth(int font, const char *s)
{
	int w=0;
	for (unsigned int ii=0; ii<strlen(s); ii++) 
	{
		char c = s[ii];
		if(font==FONT_FIXED)
		{
			if(c==' ') w += FONT_FIXED_SPACE;
			else       w += fontCharacterWidths_FIXED[fontCharacterIndex(c)]+FONT_FIXED_SPACING;
		}
		else
		{
			if(c==' ') w += FONT_TRUETYPE_SPACE;
			else       w += fontCharacterWidths_TRUETYPE[fontCharacterIndex(c)]+FONT_TRUETYPE_SPACING;
		}
	}
	return w+1;
}

void drawString(int font, const char *s, int x, int y, int align)
{
	int cx, cy;

	if ((align & ALIGN_L) != 0)       cx = x;
	else if ((align & ALIGN_R) != 0)  cx = x - stringWidth(font, s);
	else if ((align & ALIGN_HC) != 0) cx = x - (stringWidth(font, s) >> 1);
	else                              cx = x;

	if ((align & ALIGN_T) != 0) cy = y;
	else if ((align & ALIGN_B) != 0)
	{
		if(font==FONT_FIXED) cy = y - FONT_FIXED_HEIGHT;
		else                 cy = y - FONT_TRUETYPE_HEIGHT;
	}
	else cy = y;

	for (unsigned int ii=0; ii<strlen(s); ii++)
	{
		char c = s[ii];
		int ci = fontCharacterIndex(c);

		if(c==0) // END STRING
		{
			break;
		}
		else if(ci==999)
		{
			if(font==FONT_FIXED) cx += FONT_FIXED_SPACE;
			else                 cx += FONT_TRUETYPE_SPACE;
		}
		else
		{
			if(font==FONT_FIXED)
			{
				drawSubImage(IMAGE_FONT_FIXED_INDEX, cx, cy, ALIGN_LT, fontCharacterWidths_FIXED[ci], FONT_FIXED_HEIGHT, fontCharacterOffsets_FIXED[ci]-1, 0);
				cx += fontCharacterWidths_FIXED[ci]+FONT_FIXED_SPACING;
			}
			else
			{
				drawSubImage(IMAGE_FONT_TRUETYPE_INDEX, cx, cy, ALIGN_LT, fontCharacterWidths_TRUETYPE[ci], FONT_TRUETYPE_HEIGHT, fontCharacterOffsets_TRUETYPE[ci]-1, 0);
				cx += fontCharacterWidths_TRUETYPE[ci]+FONT_TRUETYPE_SPACING;
			}
		}
	}
}
