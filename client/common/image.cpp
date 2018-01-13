
int triangleEdges[Y_RES*2];

#ifndef NDS
	// SDL RGB888
	#define rgbToPixel(r,g,b) ((r<<16)+(g<<8)+b)
	#define rgbaToPixel(r,g,b,a) ((a<<24)+(r<<16)+(g<<8)+b)
	#define colorToPixel(c) (c)
	#define pixelToRed(r) ((r>>16)&0xff)
	#define pixelToGreen(g) ((g>>8)&0xff)
	#define pixelToBlue(b) (b&0xff)
#else
	// NDS RGB555
	#define rgbToPixel(r,g,b) ((1<<15)+((b>>3)<<10)+((g>>3)<<5)+(r>>3))
	#define rgbaToPixel(r,g,b,a) (((a?0:1)<<15)+((b>>3)<<10)+((g>>3)<<5)+(r>>3))
	#define colorToPixel(c) ((1<<15)+(((c>>3)&0x1f)<<10)+(((c>>8+3)&0x1f)<<5)+((c>>16+3)&0x1f))
	#define pixelToRed(r) (((r>>10)&0x1f)<<3)
	#define pixelToGreen(g) (((g>>5)&0x1f)<<3)
	#define pixelToBlue(b) (((b)&0x1f)<<3)
#endif

int getInvertAlign(int align)
{
	// TODO - only returns ALING_R / ALIGN_L - doesn't need vertical align's atm.
	if ((align&ALIGN_R)!=0) return ALIGN_L;
	else if((align&ALIGN_L)!=0) return ALIGN_R;
	else return align;
}

int getAlignX(int align, int width)
{
	if ((align&ALIGN_HC)!=0) return width>>1;
	if ((align&ALIGN_R)!=0) return width;
	return 0;
}

int getAlignY(int align, int height)
{
	if ((align&ALIGN_VC)!=0) return height>>1;
	if ((align&ALIGN_B)!=0) return height;
	return 0;
}

// TODO: should index these... way too ugly code. when i have time - promise.

int getWidth(int index)
{
	if(index==IMAGE_FONT_FIXED_INDEX)        return IMAGE_FONT_FIXED_WIDTH;
	if(index==IMAGE_FONT_TRUETYPE_INDEX)     return IMAGE_FONT_TRUETYPE_WIDTH;
	if(index==IMAGE_LOGO_INDEX)              return IMAGE_LOGO_WIDTH;
	if(index==IMAGE_DATA_BG_INDEX)           return IMAGE_DATA_BG_WIDTH;
	if(index==IMAGE_GAME_BG_INDEX)           return IMAGE_GAME_BG_WIDTH;
	if(index==IMAGE_DICE_INDEX)              return IMAGE_DICE_WIDTH;
	if(index==IMAGE_DICE_SMALL_INDEX)        return IMAGE_DICE_SMALL_WIDTH;
	if(index==IMAGE_DICE_ICON_INDEX)         return IMAGE_DICE_ICON_WIDTH;
	if(index==IMAGE_DICE_SHADOW_INDEX)       return IMAGE_DICE_SHADOW_WIDTH;
	if(index==IMAGE_DICE_BOX_INDEX)          return IMAGE_DICE_BOX_WIDTH;
	if(index==IMAGE_ENDTURN_INDEX)           return IMAGE_ENDTURN_WIDTH;
	if(index==IMAGE_HEXAGON_ICON_INDEX)      return IMAGE_HEXAGON_ICON_WIDTH;
	if(index==IMAGE_SELECTED_INDEX)          return IMAGE_SELECTED_WIDTH;
	if(index==IMAGE_MAIN_BG_INDEX)           return IMAGE_MAIN_BG_WIDTH;
	if(index==IMAGE_ABOUT_BG_INDEX)          return IMAGE_ABOUT_BG_WIDTH;
	if(index==IMAGE_INSTRUCTIONS_BG_INDEX)   return IMAGE_INSTRUCTIONS_BG_WIDTH;
	if(index==IMAGE_ARROWS_INDEX)            return IMAGE_ARROWS_WIDTH;
	if(index==IMAGE_NUMBERS_INDEX)           return IMAGE_NUMBERS_WIDTH;
	if(index==IMAGE_NUMBERS_BOX_INDEX)       return IMAGE_NUMBERS_BOX_WIDTH;
	if(index==IMAGE_KBD_LOWER_INDEX)         return IMAGE_KBD_LOWER_WIDTH;
	if(index==IMAGE_KBD_LOWER2_INDEX)        return IMAGE_KBD_LOWER2_WIDTH;
	if(index==IMAGE_KBD_UPPER_INDEX)         return IMAGE_KBD_UPPER_WIDTH;
	if(index==IMAGE_KBD_UPPER2_INDEX)        return IMAGE_KBD_UPPER2_WIDTH;
	if(index==IMAGE_HEADS_SMALL_INDEX)       return IMAGE_HEADS_SMALL_WIDTH;
	if(index==IMAGE_SURRENDER_INDEX)         return IMAGE_SURRENDER_WIDTH;
	if(index==IMAGE_SKIPTURN_INDEX)          return IMAGE_SKIPTURN_WIDTH;
	if(index==IMAGE_WIFI_ICONS_INDEX)        return IMAGE_WIFI_ICONS_WIDTH;

	#ifdef NEOFLASH
		if(index==IMAGE_NEOFLASH_INDEX)        return IMAGE_NEOFLASH_WIDTH;
	#endif
	
	return -1;
}

int getHeight(int index)
{
	if(index==IMAGE_FONT_FIXED_INDEX)        return IMAGE_FONT_FIXED_HEIGHT;
	if(index==IMAGE_FONT_TRUETYPE_INDEX)     return IMAGE_FONT_TRUETYPE_HEIGHT;
	if(index==IMAGE_LOGO_INDEX)              return IMAGE_LOGO_HEIGHT;
	if(index==IMAGE_DATA_BG_INDEX)           return IMAGE_DATA_BG_HEIGHT;
	if(index==IMAGE_GAME_BG_INDEX)           return IMAGE_GAME_BG_HEIGHT;
	if(index==IMAGE_DICE_INDEX)              return IMAGE_DICE_HEIGHT;
	if(index==IMAGE_DICE_SMALL_INDEX)        return IMAGE_DICE_SMALL_HEIGHT;
	if(index==IMAGE_DICE_ICON_INDEX)         return IMAGE_DICE_ICON_HEIGHT;
	if(index==IMAGE_DICE_SHADOW_INDEX)       return IMAGE_DICE_SHADOW_HEIGHT;
	if(index==IMAGE_DICE_BOX_INDEX)          return IMAGE_DICE_BOX_HEIGHT;
	if(index==IMAGE_ENDTURN_INDEX)           return IMAGE_ENDTURN_HEIGHT;
	if(index==IMAGE_HEXAGON_ICON_INDEX)      return IMAGE_HEXAGON_ICON_HEIGHT;
	if(index==IMAGE_SELECTED_INDEX)          return IMAGE_SELECTED_HEIGHT;
	if(index==IMAGE_MAIN_BG_INDEX)           return IMAGE_MAIN_BG_HEIGHT;
	if(index==IMAGE_ABOUT_BG_INDEX)          return IMAGE_ABOUT_BG_HEIGHT;
	if(index==IMAGE_INSTRUCTIONS_BG_INDEX)   return IMAGE_INSTRUCTIONS_BG_HEIGHT;
	if(index==IMAGE_ARROWS_INDEX)            return IMAGE_ARROWS_HEIGHT;
	if(index==IMAGE_NUMBERS_INDEX)           return IMAGE_NUMBERS_HEIGHT;
	if(index==IMAGE_NUMBERS_BOX_INDEX)       return IMAGE_NUMBERS_BOX_HEIGHT;
	if(index==IMAGE_KBD_LOWER_INDEX)         return IMAGE_KBD_LOWER_HEIGHT;
	if(index==IMAGE_KBD_LOWER2_INDEX)        return IMAGE_KBD_LOWER2_HEIGHT;
	if(index==IMAGE_KBD_UPPER_INDEX)         return IMAGE_KBD_UPPER_HEIGHT;
	if(index==IMAGE_KBD_UPPER2_INDEX)        return IMAGE_KBD_UPPER2_HEIGHT;
	if(index==IMAGE_HEADS_SMALL_INDEX)       return IMAGE_HEADS_SMALL_HEIGHT;
	if(index==IMAGE_SURRENDER_INDEX)         return IMAGE_SURRENDER_HEIGHT;
	if(index==IMAGE_SKIPTURN_INDEX)          return IMAGE_SKIPTURN_HEIGHT;
	if(index==IMAGE_WIFI_ICONS_INDEX)        return IMAGE_WIFI_ICONS_HEIGHT;

	#ifdef NEOFLASH
		if(index==IMAGE_NEOFLASH_INDEX)        return IMAGE_NEOFLASH_HEIGHT;
	#endif

	return -1;
}

int getType(int index)
{
	if(index==IMAGE_FONT_FIXED_INDEX)        return IMAGE_FONT_FIXED_TYPE;
	if(index==IMAGE_FONT_TRUETYPE_INDEX)     return IMAGE_FONT_TRUETYPE_TYPE;
	if(index==IMAGE_LOGO_INDEX)              return IMAGE_LOGO_TYPE;
	if(index==IMAGE_DATA_BG_INDEX)           return IMAGE_DATA_BG_TYPE;
	if(index==IMAGE_GAME_BG_INDEX)           return IMAGE_GAME_BG_TYPE;
	if(index==IMAGE_DICE_INDEX)              return IMAGE_DICE_TYPE;
	if(index==IMAGE_DICE_SMALL_INDEX)        return IMAGE_DICE_SMALL_TYPE;
	if(index==IMAGE_DICE_ICON_INDEX)         return IMAGE_DICE_ICON_TYPE;
	if(index==IMAGE_DICE_SHADOW_INDEX)       return IMAGE_DICE_SHADOW_TYPE;
	if(index==IMAGE_DICE_BOX_INDEX)          return IMAGE_DICE_BOX_TYPE;
	if(index==IMAGE_ENDTURN_INDEX)           return IMAGE_ENDTURN_TYPE;
	if(index==IMAGE_HEXAGON_ICON_INDEX)      return IMAGE_HEXAGON_ICON_TYPE;
	if(index==IMAGE_SELECTED_INDEX)          return IMAGE_SELECTED_TYPE;
	if(index==IMAGE_MAIN_BG_INDEX)           return IMAGE_MAIN_BG_TYPE;
	if(index==IMAGE_ABOUT_BG_INDEX)          return IMAGE_ABOUT_BG_TYPE;
	if(index==IMAGE_INSTRUCTIONS_BG_INDEX)   return IMAGE_INSTRUCTIONS_BG_TYPE;
	if(index==IMAGE_ARROWS_INDEX)            return IMAGE_ARROWS_TYPE;
	if(index==IMAGE_NUMBERS_INDEX)           return IMAGE_NUMBERS_TYPE;
	if(index==IMAGE_NUMBERS_BOX_INDEX)       return IMAGE_NUMBERS_BOX_TYPE;
	if(index==IMAGE_KBD_LOWER_INDEX)         return IMAGE_KBD_LOWER_TYPE;
	if(index==IMAGE_KBD_LOWER2_INDEX)        return IMAGE_KBD_LOWER2_TYPE;
	if(index==IMAGE_KBD_UPPER_INDEX)         return IMAGE_KBD_UPPER_TYPE;
	if(index==IMAGE_KBD_UPPER2_INDEX)        return IMAGE_KBD_UPPER2_TYPE;
	if(index==IMAGE_HEADS_SMALL_INDEX)       return IMAGE_HEADS_SMALL_TYPE;
	if(index==IMAGE_SURRENDER_INDEX)         return IMAGE_SURRENDER_TYPE;
	if(index==IMAGE_SKIPTURN_INDEX)          return IMAGE_SKIPTURN_TYPE;
	if(index==IMAGE_WIFI_ICONS_INDEX)        return IMAGE_WIFI_ICONS_TYPE;

	#ifdef NEOFLASH
		if(index==IMAGE_NEOFLASH_INDEX)        return IMAGE_NEOFLASH_TYPE;
	#endif

	return -1;
}

unsigned char* getImage(int index)
{
	if(index==IMAGE_FONT_FIXED_INDEX)        return IMAGE_FONT_FIXED_DATA;
	if(index==IMAGE_FONT_TRUETYPE_INDEX)     return IMAGE_FONT_TRUETYPE_DATA;
	if(index==IMAGE_LOGO_INDEX)              return IMAGE_LOGO_DATA;
	if(index==IMAGE_DATA_BG_INDEX)           return IMAGE_DATA_BG_DATA;
	if(index==IMAGE_GAME_BG_INDEX)           return IMAGE_GAME_BG_DATA;
	if(index==IMAGE_DICE_INDEX)              return IMAGE_DICE_DATA;	
	if(index==IMAGE_DICE_SMALL_INDEX)        return IMAGE_DICE_SMALL_DATA;
	if(index==IMAGE_DICE_ICON_INDEX)         return IMAGE_DICE_ICON_DATA;
	if(index==IMAGE_DICE_SHADOW_INDEX)       return IMAGE_DICE_SHADOW_DATA;
	if(index==IMAGE_DICE_BOX_INDEX)          return IMAGE_DICE_BOX_DATA;
	if(index==IMAGE_ENDTURN_INDEX)           return IMAGE_ENDTURN_DATA;
	if(index==IMAGE_HEXAGON_ICON_INDEX)      return IMAGE_HEXAGON_ICON_DATA;
	if(index==IMAGE_SELECTED_INDEX)          return IMAGE_SELECTED_DATA;
	if(index==IMAGE_MAIN_BG_INDEX)           return IMAGE_MAIN_BG_DATA;
	if(index==IMAGE_ABOUT_BG_INDEX)          return IMAGE_ABOUT_BG_DATA;
	if(index==IMAGE_INSTRUCTIONS_BG_INDEX)   return IMAGE_INSTRUCTIONS_BG_DATA;
	if(index==IMAGE_ARROWS_INDEX)            return IMAGE_ARROWS_DATA;
	if(index==IMAGE_NUMBERS_INDEX)           return IMAGE_NUMBERS_DATA;
	if(index==IMAGE_NUMBERS_BOX_INDEX)       return IMAGE_NUMBERS_BOX_DATA;
	if(index==IMAGE_KBD_LOWER_INDEX)         return IMAGE_KBD_LOWER_DATA;
	if(index==IMAGE_KBD_LOWER2_INDEX)        return IMAGE_KBD_LOWER2_DATA;
	if(index==IMAGE_KBD_UPPER_INDEX)         return IMAGE_KBD_UPPER_DATA;
	if(index==IMAGE_KBD_UPPER2_INDEX)        return IMAGE_KBD_UPPER2_DATA;
	if(index==IMAGE_HEADS_SMALL_INDEX)       return IMAGE_HEADS_SMALL_DATA;
	if(index==IMAGE_SURRENDER_INDEX)         return IMAGE_SURRENDER_DATA;
	if(index==IMAGE_SKIPTURN_INDEX)          return IMAGE_SKIPTURN_DATA;
	if(index==IMAGE_WIFI_ICONS_INDEX)        return IMAGE_WIFI_ICONS_DATA;

	#ifdef NEOFLASH
		if(index==IMAGE_NEOFLASH_INDEX)        return IMAGE_NEOFLASH_DATA;
	#endif

	return NULL;
}

unsigned char* getPalette(int index)
{
	if(index==PALETTE_LOGO_INDEX)            return PALETTE_LOGO_DATA;
	if(index==PALETTE_DATA_BG_INDEX)         return PALETTE_DATA_BG_DATA;	
	if(index==PALETTE_GAME_BG_INDEX)         return PALETTE_GAME_BG_DATA;	
	if(index==PALETTE_DICE_INDEX)            return PALETTE_DICE_DATA;	
	if(index==PALETTE_DICE_SMALL_INDEX)      return PALETTE_DICE_SMALL_DATA;
	if(index==PALETTE_DICE_ICON_INDEX)       return PALETTE_DICE_ICON_DATA;
	if(index==PALETTE_DICE_SHADOW_INDEX)     return PALETTE_DICE_SHADOW_DATA;
	if(index==PALETTE_DICE_BOX_INDEX)        return PALETTE_DICE_BOX_DATA;
	if(index==PALETTE_ENDTURN_INDEX)         return PALETTE_ENDTURN_DATA;
	if(index==PALETTE_HEXAGON_ICON_INDEX)    return PALETTE_HEXAGON_ICON_DATA;
	if(index==PALETTE_SELECTED_INDEX)        return PALETTE_SELECTED_DATA;
	if(index==PALETTE_MAIN_BG_INDEX)         return PALETTE_MAIN_BG_DATA;
	if(index==PALETTE_ABOUT_BG_INDEX)        return PALETTE_ABOUT_BG_DATA;
	if(index==PALETTE_INSTRUCTIONS_BG_INDEX) return PALETTE_INSTRUCTIONS_BG_DATA;
	if(index==PALETTE_ARROWS_INDEX)          return PALETTE_ARROWS_DATA;
	if(index==PALETTE_NUMBERS_INDEX)         return PALETTE_NUMBERS_DATA;
	if(index==PALETTE_NUMBERS_BOX_INDEX)     return PALETTE_NUMBERS_BOX_DATA;
	if(index==PALETTE_KBD_LOWER_INDEX)       return PALETTE_KBD_LOWER_DATA;
	if(index==PALETTE_KBD_LOWER2_INDEX)      return PALETTE_KBD_LOWER2_DATA;
	if(index==PALETTE_KBD_UPPER_INDEX)       return PALETTE_KBD_UPPER_DATA;
	if(index==PALETTE_KBD_UPPER2_INDEX)      return PALETTE_KBD_UPPER2_DATA;
	if(index==PALETTE_HEADS_SMALL_INDEX)     return PALETTE_HEADS_SMALL_DATA;
	if(index==PALETTE_SURRENDER_INDEX)       return PALETTE_SURRENDER_DATA;
	if(index==PALETTE_SKIPTURN_INDEX)        return PALETTE_SKIPTURN_DATA;
	if(index==PALETTE_WIFI_ICONS_INDEX)      return PALETTE_WIFI_ICONS_DATA;

	#ifdef NEOFLASH
		if(index==PALETTE_NEOFLASH_INDEX)      return PALETTE_NEOFLASH_DATA;
	#endif

	return NULL;
}

int getPaletteTransparent(int index)
{
	if(index==PALETTE_LOGO_INDEX)            return PALETTE_LOGO_TRANSPARENT;
	if(index==PALETTE_DATA_BG_INDEX)         return PALETTE_DATA_BG_TRANSPARENT;	
	if(index==PALETTE_GAME_BG_INDEX)         return PALETTE_GAME_BG_TRANSPARENT;	
	if(index==PALETTE_DICE_INDEX)            return PALETTE_DICE_TRANSPARENT;	
	if(index==PALETTE_DICE_SMALL_INDEX)      return PALETTE_DICE_SMALL_TRANSPARENT;
	if(index==PALETTE_DICE_ICON_INDEX)       return PALETTE_DICE_ICON_TRANSPARENT;
	if(index==PALETTE_DICE_SHADOW_INDEX)     return PALETTE_DICE_SHADOW_TRANSPARENT;
	if(index==PALETTE_DICE_BOX_INDEX)        return PALETTE_DICE_BOX_TRANSPARENT;
	if(index==PALETTE_ENDTURN_INDEX)         return PALETTE_ENDTURN_TRANSPARENT;
	if(index==PALETTE_HEXAGON_ICON_INDEX)    return PALETTE_HEXAGON_ICON_TRANSPARENT;
	if(index==PALETTE_SELECTED_INDEX)        return PALETTE_SELECTED_TRANSPARENT;
	if(index==PALETTE_MAIN_BG_INDEX)         return PALETTE_MAIN_BG_TRANSPARENT;
	if(index==PALETTE_ABOUT_BG_INDEX)        return PALETTE_ABOUT_BG_TRANSPARENT;
	if(index==PALETTE_INSTRUCTIONS_BG_INDEX) return PALETTE_INSTRUCTIONS_BG_TRANSPARENT;
	if(index==PALETTE_ARROWS_INDEX)          return PALETTE_ARROWS_TRANSPARENT;
	if(index==PALETTE_NUMBERS_INDEX)         return PALETTE_NUMBERS_TRANSPARENT;
	if(index==PALETTE_NUMBERS_BOX_INDEX)     return PALETTE_NUMBERS_BOX_TRANSPARENT;
	if(index==PALETTE_KBD_LOWER_INDEX)       return PALETTE_KBD_LOWER_TRANSPARENT;
	if(index==PALETTE_KBD_LOWER2_INDEX)      return PALETTE_KBD_LOWER2_TRANSPARENT;
	if(index==PALETTE_KBD_UPPER_INDEX)       return PALETTE_KBD_UPPER_TRANSPARENT;
	if(index==PALETTE_KBD_UPPER2_INDEX)      return PALETTE_KBD_UPPER2_TRANSPARENT;
	if(index==PALETTE_HEADS_SMALL_INDEX)     return PALETTE_HEADS_SMALL_TRANSPARENT;
	if(index==PALETTE_SURRENDER_INDEX)       return PALETTE_SURRENDER_TRANSPARENT;
	if(index==PALETTE_SKIPTURN_INDEX)        return PALETTE_SKIPTURN_TRANSPARENT;
	if(index==PALETTE_WIFI_ICONS_INDEX)      return PALETTE_WIFI_ICONS_TRANSPARENT;

	#ifdef NEOFLASH
		if(index==PALETTE_NEOFLASH_INDEX)      return PALETTE_NEOFLASH_TRANSPARENT;
	#endif

	return -1;
}

void loadImage(int index)
{
	int i,r,g,b,x,y;

	if(getType(index)==IMAGE_PALETTE) // precalc image values according to the palette
	{
		images[index] = (SCREEN*)malloc(getWidth(index)*getHeight(index)*sizeof(SCREEN));
		byte *img = getImage(index);
		for(y=0;y<getHeight(index);y++)
		{
			for(x=0;x<getWidth(index);x++)
			{
				i = img[x+y*getWidth(index)];
				if(i==getPaletteTransparent(index))
				{
					images[index][x+y*getWidth(index)] = 0;
				}
				else
				{
					byte *pal = getPalette(index);
					r = pal[i*3];
					g = pal[i*3+1];
					b = pal[i*3+2];
					if(rgbToPixel(r,g,b)==0)
						images[index][x+y*getWidth(index)] = 1; // 0 is being mapped as transparent
					else
						images[index][x+y*getWidth(index)] = rgbToPixel(r,g,b);
				}
			}
		}
	}

	imageLoaded[index] = true;
}

void drawSubImage(int index, int x, int y, int align, int width, int height, int xpos, int ypos)
{
	if(!imageLoaded[index]) loadImage(index);
	
	int xx,yy;
	byte *img = getImage(index);
	x -= getAlignX(align, width);
	y -= getAlignY(align, height);
	for(yy=0;yy<height;yy++)
	{
		for(xx=0;xx<width;xx++)
		{
			if((x+xx)>=0&&(x+xx)<X_RES&&(y+yy)>=0&&(y+yy)<Y_RES)
			{	
				if(getType(index)==IMAGE_PALETTE)
				{
					// TODO: slow
					SCREEN val = images[index][xpos+xx+(ypos+yy)*getWidth(index)];
					if(val) screenBuffer[activeScreen][x+xx+(y+yy)*X_RES] = val;
				}
				else if(getType(index)==IMAGE_1BIT) // FONT_FIXED
				{
					int val = img[xpos+xx+(ypos+yy)*getWidth(index)];
					if(val==0)
						screenBuffer[activeScreen][x+xx+(y+yy)*X_RES] = colorToPixel(activeColor);
				}
				else if(getType(index)==IMAGE_ALPHA) // FONT_TRUETYPE
				{
					int bri = img[xpos+xx+(ypos+yy)*getWidth(index)];
					int bri2 = 256-bri;
					int orig = screenBuffer[activeScreen][x+xx+(y+yy)*X_RES];
					
					// additive - redo this if it looks too ugly :)
					int r1 = (pixelToRed(orig)*bri2)>>8;
					int g1 = (pixelToGreen(orig)*bri2)>>8;
					int b1 = (pixelToBlue(orig)*bri2)>>8;
					int r2 = (((activeColor>>16)&0xff)*bri)>>8;
					int g2 = (((activeColor>>8)&0xff)*bri)>>8;
					int b2 = ((activeColor&0xff)*bri)>>8;
					int r = (r1+r2)&0xff;
					int g = (g1+g2)&0xff;
					int b = (b1+b2)&0xff;
					screenBuffer[activeScreen][x+xx+(y+yy)*X_RES] = rgbToPixel(r,g,b);
				}
				//else // normal rgba32 image - do we have these?
				//	screenBuffer[activeScreen][x+xx+(y+yy)*X_RES] = colorToPixel(img[xpos+xx+(ypos+yy)*getWidth(index)]);
			}
		}
	}
}

void drawImage(int index, int x, int y, int align)
{
	if(!imageLoaded[index]) loadImage(index);
	int xx,yy;
	
	if(getType(index)==IMAGE_PALETTE)
	{
		x -= getAlignX(align, getWidth(index));
		y -= getAlignY(align, getHeight(index));
		for(yy=0;yy<getHeight(index);yy++)
		{
			for(xx=0;xx<getWidth(index);xx++)
			{
				if((x+xx)>=0&&(x+xx)<X_RES&&(y+yy)>=0&&(y+yy)<Y_RES)
				{
					SCREEN val = images[index][xx+yy*getWidth(index)];
					if(val) screenBuffer[activeScreen][x+xx+(y+yy)*X_RES] = val;
				}
			}
		}
	}
}

void setColor(int color)
{
	activeColor = color;
}

int getColor()
{
	return activeColor;
}

void fillRect(int x, int y, int width, int height)
{
	for(int yy=y;yy<y+height;yy++)
	{
		for(int xx=x;xx<x+width;xx++)
		{
			if(xx>=0&&xx<X_RES&&yy>=0&&yy<Y_RES)
				screenBuffer[activeScreen][xx+yy*X_RES] = colorToPixel(activeColor);
		}
	}
}

void triangleEdge(int x0, int y0, int x1, int y1)
{
	int mi = y0<y1 ? y0 : y1;
	int ma = y0>y1 ? y0 : y1;
	if (mi<0) mi = 0;
	if (ma>Y_RES) ma = Y_RES;
	if (mi>=ma) return;
	int xi = (x1-x0 << 16)/(y1-y0);
	ma -= mi; x0 = ((y0<y1 ? x0 : x1) << 16)+(mi-(y0<y1?y0:y1))*xi+(xi>>1);
	int i=0;
	do { 
		int tmp = triangleEdges[mi*2+1+i];
		triangleEdges[i+mi*2] = tmp;
		i++; 
		triangleEdges[i+mi*2] = x0;
		i++;
		x0 += xi; 
	} while (--ma>0);
}

void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2)
{
	triangleEdge(x0, y0, x1, y1);
	triangleEdge(x1, y1, x2, y2);
	triangleEdge(x2, y2, x0, y0);
	int ymi = y0<y1 ? y0 : y1;
	int yma = y0>y1 ? y0 : y1;
	if (y2<ymi) ymi = y2;
	if (ymi<0) ymi = 0;
	if (y2>yma) yma = y2;
	if (yma>Y_RES) yma = Y_RES;

	int col = activeColor;
	for (int y=ymi; y<yma; y++) {
		int mi = triangleEdges[y*2]+32768>>16, ma = triangleEdges[y*2+1]+32768>>16;
		if (mi>ma) { int t = mi; mi = ma; ma = t; }
		if (mi<0) mi = 0;
		if (ma>X_RES) ma = X_RES;
		for (int x = mi; x<ma; x++) 
		{
//			if(x>=0&&x<X_RES&&y>=0&&y<Y_RES)
			screenBuffer[activeScreen][x+y*X_RES] = colorToPixel(col);
		}
	}
}

void drawHLine(int x1, int y, int x2) {
	fillRect(x1<x2 ? x1:x2, y, x1<x2 ? x2-x1+1 : x1-x2+1, 1);
}

void drawVLine(int x, int y1, int y2) {
	fillRect(x, y1<y2 ? y1 : y2, 1, y1<y2 ? y2-y1+1 : y1-y2+1);
}

void drawLine(int x0, int y0, int x1, int y1)
{
	if(x0==x1) drawVLine(x0, y0, y1);
	else if(y0==y1) drawHLine(x0, y0, x1);
	else
	{
		int xd = x1-x0, yd = y1-y0;
		xd = xd<0 ? -xd : xd;
		yd = yd<0 ? -yd : yd;
		if (xd<yd) {
			if (y1<y0) { int tmp = x1; x1 = x0; x0 = tmp; tmp = y1; y1 = y0; y0 = tmp; }
			int oldy = y0, y;
			int dir = x1<x0 ? -1 : 1;
			int muller = (y1-y0)*2*dir, divver = (x1-x0)*2, adder = x1-x0+y0*divver;
			for (int x = x0; x!=x1; x += dir) {
				adder += muller; y = adder/divver;
				fillRect(x, oldy, 1, y-oldy);
				oldy = y;
			}
			fillRect(x1, oldy, 1, 1);
		} else {
			if (x1<x0) { int tmp = x1; x1 = x0; x0 = tmp; tmp = y1; y1 = y0; y0 = tmp; }
			int oldx = x0, x;
			int dir = y1<y0 ? -1 : 1;
			int muller = (x1-x0)*2*dir, divver = (y1-y0)*2, adder = y1-y0+x0*divver;
			for (int y = y0; y!=y1; y += dir) {
				adder += muller; x = adder/divver;
				fillRect(oldx, y, x-oldx, 1);
				oldx = x;
			}
			fillRect(oldx, y1, 1, 1);
		}
	}
}

void drawRect(int x, int y, int width, int height)
{
	fillRect(x, y, width, 1);
	fillRect(x+width-1, y, 1, height);
	fillRect(x, y, 1, height);
	fillRect(x, y+height-1, width, 1);
}


// not mathematically correct "hexagon", but works for this game

void drawHexagon(int x, int y, int width, int height, boolean horizontal)
{
	x--;
	int xinc = width;
	int yinc = height>>1;
	
	// horizontal
	drawLine(x, y-height, x+xinc, y-yinc); // 12 - 2
	drawLine(x+xinc, y-yinc, x+xinc, y+yinc); // 2 - 4
	drawLine(x+xinc, y+yinc, x, y+height); // 4 - 6
	drawLine(x, y+height, x-xinc, y+yinc); // 6 - 8 
	drawLine(x-xinc, y+yinc, x-xinc, y-yinc); // 8 - 10
	drawLine(x-xinc, y-yinc, x, y-height); // 10 - 12
}

void fillHexagon(int x, int y, int width, int height, boolean horizontal)
{
	int xinc = width;
	int yinc = height>>1; // *3/7;
	
	// horizontal
	// DEBUG 4-6 / 10-12 +1 for resolution fixing... wrong wrong wrong
	fillTriangle(x, y, x, y-height, x+xinc, y-yinc); // 12 - 2
	fillTriangle(x, y, x+xinc, y-yinc, x+xinc, y+yinc); // 2 - 4
	fillTriangle(x, y, x+xinc, y+yinc, x, y+height); // 4 - 6
	fillTriangle(x, y, x, y+height, x-xinc, y+yinc); // 6 - 8 
	fillTriangle(x, y, x-xinc, y+yinc, x-xinc, y-yinc); // 8 - 10
	fillTriangle(x, y, x-xinc, y-yinc, x, y-height); // 10 - 12

/* orig
	fillTriangle(x, y, x, y-height, x+xinc, y-yinc); // 12 - 2
	fillTriangle(x, y, x+xinc, y-yinc, x+xinc, y+yinc); // 2 - 4
	fillTriangle(x, y, x+xinc, y+yinc, x, y+height+1); // 4 - 6
	fillTriangle(x, y, x, y+height, x-xinc, y+yinc); // 6 - 8 
	fillTriangle(x, y, x-xinc, y+yinc, x-xinc, y-yinc); // 8 - 10
	fillTriangle(x, y, x-xinc+1, y-yinc, x, y-height); // 10 - 12
	
	*/
}
