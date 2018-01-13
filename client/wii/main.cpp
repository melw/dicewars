#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/ioctl.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <unistd.h>
//#include <pthread.h>
#include <errno.h>
#include <gccore.h>
#include <ogcsys.h>

void reboot();

#include "../../common/common.h"
#include "../../common/dicewars_version.h"
#include "../../common/gamelogic.h"
#include "../../common/network.h"
#include "../../common/inifile.h"
#include "../../common/database.h"
#include "../../common/gamestatistics.h"
#include "../common/game.h"
#include "../common/image.h"
#include "../common/font.h"
#include "../common/keyboard.h"
#include "../common/menu.h"
#include "../common/network.h"
#include "../common/sound.h"
#include "../common/connectivity.h"

/*** 2D Video Globals ***/
GXRModeObj *vmode;        /*** Graphics Mode Object ***/
u32 *xfb[2] = { NULL, NULL };    /*** Framebuffers ***/
int whichfb = 0;        /*** Frame buffer toggle ***/
int canvasWidth = 640;
int canvasHeight = 480;
int canvasX = (canvasWidth>>2)-(X_RES>>2);
int canvasY = (canvasHeight>>2)-(Y_RES>>1);

#define SCALE 1
//#define SCALE 2
SCREEN *scaleBuffer[2];

int globalTimer;

void init_ogc(void)
{
	VIDEO_Init();		/*** ALWAYS CALL FIRST IN ANY LIBOGC PROJECT!
							Not only does it initialise the video 
							subsystem, but also sets up the ogc os
						***/
 
	PAD_Init();			/*** Initialise pads for input ***/
 
	/*** Try to match the current video display mode
		using the higher resolution interlaced.
    
		So NTSC/MPAL gives a display area of 640x480
		PAL display area is 640x528
	***/

	switch (VIDEO_GetCurrentTvMode ())
	{

	case VI_NTSC:
		vmode = &TVNtsc480IntDf;
		break;
 
	case VI_PAL:
		vmode = &TVPal528IntDf;
		break;
 
	case VI_MPAL:
		vmode = &TVMpal480IntDf;
		break;
 
	default:
		vmode = &TVNtsc480IntDf;
		break;
  }

	canvasWidth = vmode->viWidth;
	canvasHeight = vmode->viHeight;
//	canvasX = (canvasWidth>>2)-(X_RES>>2);
	canvasX = (canvasWidth-(X_RES*SCALE))>>2;
	canvasY = (canvasHeight>>2)-(Y_RES>>1)+(Y_RES>>3);
 
	/*** Let libogc configure the mode ***/
	VIDEO_Configure (vmode);
 
	/*** Now configure the framebuffer. 
		Really a framebuffer is just a chunk of memory
		to hold the display line by line.
	***/
 
	xfb[0] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer (vmode));

	/*** I prefer also to have a second buffer for double-buffering.
		This is not needed for the console demo.
	***/
	xfb[1] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer (vmode));
 

	/*** Define a console ***/
	console_init (xfb[0], 20, 64, vmode->fbWidth, vmode->xfbHeight, vmode->fbWidth * 2);
 
	/*** Clear framebuffer to black ***/
	VIDEO_ClearFrameBuffer (vmode, xfb[0], COLOR_BLACK);
	VIDEO_ClearFrameBuffer (vmode, xfb[1], COLOR_BLACK);
 
	/*** Set the framebuffer to be displayed at next VBlank ***/
	VIDEO_SetNextFramebuffer (xfb[0]);
 
	/*** Get the PAD status updated by libogc ***/
	VIDEO_SetBlack (0);
 
	/*** Update the video for next vblank ***/
	VIDEO_Flush ();

	VIDEO_WaitVSync ();        /*** Wait for VBL ***/

	if (vmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync ();

	activeScreen = SCREEN_LOWER;
/*	screenBuffer[SCREEN_LOWER] = (unsigned long *)malloc(X_RES*Y_RES*4);
	screenBuffer[SCREEN_UPPER] = (unsigned long *)malloc(X_RES*Y_RES*4);
	scaleBuffer[SCREEN_LOWER] = (unsigned long *)malloc(X_RES*Y_RES*4*SCALE*SCALE);
	scaleBuffer[SCREEN_UPPER] = (unsigned long *)malloc(X_RES*Y_RES*4*SCALE*SCALE); */
	screenBuffer[SCREEN_LOWER] = (SCREEN*)malloc(X_RES*Y_RES*4);
	screenBuffer[SCREEN_UPPER] = (SCREEN*)malloc(X_RES*Y_RES*4);
	scaleBuffer[SCREEN_LOWER]  = (SCREEN*)malloc(X_RES*Y_RES*4*SCALE*SCALE);
	scaleBuffer[SCREEN_UPPER]  = (SCREEN*)malloc(X_RES*Y_RES*4*SCALE*SCALE); 
}

/*void logPrintf(char *msg, ...)
{
	// TODO: print to file instead?
	struct tm *ptr;
	time_t tm;
	char str[256];
	
	tm = time(NULL);
	ptr = localtime(&tm);
	strftime(str, 100, "[%H:%M:%S %d.%m.%Y] ", ptr);
	printf(str);
	
	if(msg)
	{
		va_list argp;
		va_start(argp, msg);
		vprintf(msg, argp);
		va_end(argp);
	}

	fflush(stdout);
}*/

#define logPrintf(...)

#include "../../common/gamelogic.cpp"
#include "../../common/network.cpp"
#include "../../common/inifile.cpp"
#include "../../common/database.cpp"
#include "../common/font.cpp"
#include "../common/image.cpp"
#include "../common/keyboard.cpp"
#include "../common/menu.cpp"
#include "../common/game.cpp"
#include "../common/network.cpp"
#include "../common/sound.cpp"
#include "../common/connectivity.cpp"

//#include "../fmod/sound.cpp"

u32 CvtRGB (u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2)
{
  int y1, cb1, cr1, y2, cb2, cr2, cb, cr;
 
  y1 = (299 * r1 + 587 * g1 + 114 * b1) / 1000;
  cb1 = (-16874 * r1 - 33126 * g1 + 50000 * b1 + 12800000) / 100000;
  cr1 = (50000 * r1 - 41869 * g1 - 8131 * b1 + 12800000) / 100000;
 
  y2 = (299 * r2 + 587 * g2 + 114 * b2) / 1000;
  cb2 = (-16874 * r2 - 33126 * g2 + 50000 * b2 + 12800000) / 100000;
  cr2 = (50000 * r2 - 41869 * g2 - 8131 * b2 + 12800000) / 100000;
 
  cb = (cb1 + cb2) >> 1;
  cr = (cr1 + cr2) >> 1;
 
  return (y1 << 24) | (cb << 16) | (y2 << 8) | cr;
}

void blitScreen()
{
	whichfb ^= 1;
//	scaleBuffer[SCREEN_UPPER] = (SCREEN*)xfb[whichfb];
//	scaleBuffer[SCREEN_LOWER] = (SCREEN*)xfb[whichfb]+canvasWidth*Y_RES*SCALE*SCALE;
	paint();
	if(SCALE==1) // no scaling
	{
		for(int y=0;y<Y_RES;y++)
		{
			for(int x=0;x<X_RES;x+=2)
			{
				u32 rgb1 = screenBuffer[SCREEN_UPPER][x+y*X_RES];
				u32 rgb2 = screenBuffer[SCREEN_UPPER][x+1+y*X_RES];
				u32 rgb3 = screenBuffer[SCREEN_LOWER][x+y*X_RES];
				u32 rgb4 = screenBuffer[SCREEN_LOWER][x+1+y*X_RES];
				xfb[whichfb][((y+canvasY)*(canvasWidth>>1))+(x>>1)+canvasX] = CvtRGB((rgb1>>16), ((rgb1>>8)%256), (rgb1%256), (rgb2>>16), ((rgb2>>8)%256), (rgb2%256));
				xfb[whichfb][((y+canvasY+Y_RES)*(canvasWidth>>1))+(x>>1)+canvasX] = CvtRGB((rgb3>>16), ((rgb3>>8)%256), (rgb3%256), (rgb4>>16), ((rgb4>>8)%256), (rgb4%256));
//				xfb[whichfb][y*(canvasWidth>>1)+(x>>1)+canvasX] = CvtRGB((rgb1>>16), ((rgb1>>8)%256), (rgb1%256), (rgb2>>16), ((rgb2>>8)%256), (rgb2%256));
				//screenBuffer[
				//scaleBuffer[SCREEN_UPPER][x+y*canvasWidth] = screenBuffer[SCREEN_UPPER][x+y*X_RES];
			}
//			memcpy(scaleBuffer[SCREEN_UPPER]+y*canvasWidth, screenBuffer[SCREEN_UPPER]+y*X_RES, X_RES*4);
//			memcpy(scaleBuffer[SCREEN_LOWER]+y*canvasWidth, screenBuffer[SCREEN_LOWER]+y*X_RES, X_RES*2);
		}
	}
	else
	{
		for(int y=0;y<Y_RES;y++)
		{
			for(int x=0;x<X_RES;x++)
			{
				u32 rgb1 = screenBuffer[activeScreen][x+y*X_RES];
				u32 val = CvtRGB((rgb1>>16), ((rgb1>>8)%256), (rgb1%256), (rgb1>>16), ((rgb1>>8)%256), (rgb1%256));
				xfb[whichfb][((y*2)+canvasY)*(canvasWidth>>1)+x+canvasX] = val;
				xfb[whichfb][((y*2)+1+canvasY)*(canvasWidth>>1)+x+canvasX] = val;
//				xfb[whichfb][((y+canvasY+Y_RES)*(canvasWidth>>1))+(x>>1)+canvasX] = CvtRGB((rgb3>>16), ((rgb3>>8)%256), (rgb3%256), (rgb4>>16), ((rgb4>>8)%256), (rgb4%256));
//						scaleBuffer[SCREEN_LOWER][x*SCALE+x2+(y*SCALE+y2)*X_RES*SCALE] = screenBuffer[SCREEN_LOWER][x+y*X_RES];
//						scaleBuffer[SCREEN_UPPER][x*SCALE+x2+(y*SCALE+y2)*X_RES*SCALE] = screenBuffer[SCREEN_UPPER][x+y*X_RES];
			}
		}
	}
	/*** Set this as next frame to display ***/
	VIDEO_SetNextFramebuffer (xfb[whichfb]);
	VIDEO_Flush ();
	VIDEO_WaitVSync ();
}

int main(int argc, char *argv[])
{
	int i;
	/*SDL_Event event;

	if ((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER)==-1)) {
		printf("Error: Could not initialize SDL!\n");
		return -1;
	}*/
	globalTimer=0;
	init_ogc();
//	srand(time(0));
	initSound();
	initGame();
	initTimer(LOCAL);
	
/*	initIniFile("dicewars.ini");
	char *sAddr = readIniFileString("serverAddress");
	int sPort = readIniFileInt("serverPort");
	char *pName = readIniFileString("playerName");
	int fColor = readIniFileInt("favouriteColor");
	closeIniFile();*/

	char *sAddr = NULL;
	int sPort = -1;
	char *pName = NULL;
	int fColor = -1;

	if(sAddr!=NULL) serverAddress = sAddr;
	if(pName!=NULL) { memset(playerName, 0, MAX_NAME_LENGTH); memcpy(playerName, pName, strlen(pName)); }
	if(sPort!=-1) { serverPort = sPort; }
	if(fColor!=-1) { favouriteColor = fColor-1; }

//	if(argc>1)
//		serverPort = atoi(argv[1]);

	int done=0;	
	while ( !done ) //&& SDL_WaitEvent(&event) )
	{
		if(eventCallback())
		{
			update(LOCAL);
			blitScreen();
		}
		else
			done = 1;
	}
	
	reboot();
	
	return 0;
}

int last_mouseX=X_CEN;
int last_mouseY=Y_CEN;

void setActiveScreen(int display)
{
	activeScreen = display;
}

boolean eventCallback()
{
	int key;

	globalTimer++;
	
	if(gameState==GAME_STATE_EXIT)
		return false;
	
	PAD_ScanPads();
	int pressed = PAD_ButtonsDown(0);
	int keysup = PAD_ButtonsUp(0);

	int mousex = last_mouseX;
	int mousey = last_mouseY;
	mousex += (PAD_StickX(0)>>3);
	mousey -= (PAD_StickY(0)>>3);
	if(mousex<0) mousex=0;
	if(mousex>X_RES) mousex=X_RES;
	if(mousey<0) mousey=0;
	if(mousey>Y_RES) mousey=Y_RES;

//	if (mousex*SCALE!=last_mouseX||(mousey*SCALE-Y_RES*SCALE)!=last_mouseY)
	if (mousex*SCALE!=last_mouseX||mousey*SCALE!=last_mouseY)
	{
		last_mouseX=mousex*SCALE;
		last_mouseY=mousey*SCALE; //-Y_RES*SCALE*SCREEN_UPPER;

		pointerX=mousex/SCALE;
		pointerY=mousey/SCALE; //-Y_RES*SCREEN_UPPER;
		pointerMoved=true;
	}
	else
	{
		pointerMoved=false;
	}

	if(pressed>0)
	{
		if (pressed & PAD_BUTTON_A) keyPressed(KEY_SELECT);
		if (pressed & PAD_BUTTON_B) keyPressed(KEY_BACK);
		if (pressed & PAD_BUTTON_UP) keyPressed(KEY_UP);
		if (pressed & PAD_BUTTON_DOWN) keyPressed(KEY_DOWN);
		if (pressed & PAD_BUTTON_RIGHT) keyPressed(KEY_RIGHT);
		if (pressed & PAD_BUTTON_LEFT) keyPressed(KEY_LEFT);
		if (pressed & PAD_BUTTON_MENU) keyPressed(KEY_ESCAPE);
		if (pressed & PAD_BUTTON_START) reboot();
		if (pressed & PAD_TRIGGER_R)
		{
			keyPressed(KEY_POINTER);
			pointerPressX=mousex/SCALE;
			pointerPressY=mousey/SCALE; //-Y_RES*SCREEN_UPPER;
			pointerX=mousex/SCALE;
			pointerY=mousey/SCALE; //-Y_RES*SCREEN_UPPER;
			pointerMoved=true;
		}
	}
	if(keysup>0)
	{
		if (keysup & PAD_BUTTON_A) keyReleased(KEY_SELECT);
		if (keysup & PAD_BUTTON_B)	keyReleased(KEY_BACK);
		if (keysup & PAD_BUTTON_UP) keyReleased(KEY_UP);
		if (keysup & PAD_BUTTON_DOWN) keyReleased(KEY_DOWN);
		if (keysup & PAD_BUTTON_RIGHT) keyReleased(KEY_RIGHT);
		if (keysup & PAD_BUTTON_LEFT) keyReleased(KEY_LEFT);
		if (keysup & PAD_BUTTON_MENU) keyReleased(KEY_ESCAPE);
		if (keysup & PAD_TRIGGER_R)
		{
			keyReleased(KEY_POINTER);
			pointerX=mousex/SCALE;
			pointerY=mousey/SCALE; //-Y_RES*SCREEN_UPPER;
			pointerMoved=true;
		}			
	}

	return true;
}

unsigned long getTime()
{
	// TODO: real milliseconds
	return globalTimer*20;
}

void reboot()
{
	void (*reload)() = (void(*)())0x80001800;
	reload();
}
