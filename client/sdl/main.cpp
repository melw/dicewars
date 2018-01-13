#ifdef TARGET_WIN32
	#include <windows.h>
	#include <conio.h>
	#include <winsock.h>
	#include <Iphlpapi.h>
	#include <Assert.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <pthread.h>
	#include <errno.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "sdl/SDL.h"

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

void logPrintf(char *msg, ...)
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
}

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

#include "../fmod/sound.cpp"

#define SCALE 1
unsigned long *scaleBuffer[NUM_SCREENS];

SDL_Surface *sdlSurface;
boolean gameInited = false;

void sdlInit()
{
	activeScreen = SCREEN_LOWER;
	for(int i=0;i<NUM_SCREENS;i++)
	{
		screenBuffer[i] = (unsigned long *)malloc(X_RES*Y_RES*4);
		scaleBuffer[i] = (unsigned long *)malloc(X_RES*Y_RES*4*SCALE*SCALE);
	}
	
	sdlSurface = SDL_SetVideoMode(X_RES*SCALE, Y_RES*NUM_SCREENS*SCALE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF );

	if ( sdlSurface == NULL ) {
		printf("Couldn't set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
}

void blitScreen()
{
	if (SDL_MUSTLOCK(sdlSurface)) 
		if (SDL_LockSurface(sdlSurface) < 0) 
			return;

	for(int i=0;i<NUM_SCREENS;i++)
		scaleBuffer[i] = (unsigned long *)sdlSurface->pixels+(X_RES*Y_RES*SCALE*SCALE)*i;

	if(gameInited)
		paint();
	if(SCALE==1) // no scaling
	{
		for(int i=0;i<NUM_SCREENS;i++)
			memcpy(scaleBuffer[i], screenBuffer[i], X_RES*Y_RES*4);
	}
	else
	{
		for(int y=0;y<Y_RES;y++)
		{
			for(int x=0;x<X_RES;x++)
			{
				for(int y2=0;y2<SCALE;y2++)
				{
					for(int x2=0;x2<SCALE;x2++)
					{
						for(int i=0;i<NUM_SCREENS;i++)
						{
							scaleBuffer[i][x*SCALE+x2+(y*SCALE+y2)*X_RES*SCALE] = screenBuffer[i][x+y*X_RES];
						}
					}
				}
			}
		}
	}
	if (SDL_MUSTLOCK(sdlSurface)) 
		SDL_UnlockSurface(sdlSurface);

//	SDL_UpdateRect(sdlSurface, 0, 0, X_RES*SCALE, Y_RES*SCALE*2);
	SDL_Flip(sdlSurface);
}

int main(int argc, char *argv[])
{
	int i;
	SDL_Event event;

	if ((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER)==-1)) {
		logPrintf("Error: Could not initialize SDL!\n");
		return -1;
	}
	sdlInit();
	srand(time(0));
	initSound();
	initGame();
	initTimer(LOCAL);

	#ifdef NEOFLASH
		logPrintf("NEOFLASH splash screen...\n");
		setActiveScreen(SCREEN_UPPER);
		drawImage(IMAGE_NEOFLASH_INDEX, 0, 0, ALIGN_LT);
		setActiveScreen(SCREEN_LOWER);
		for(int i=0;i<Y_RES;i+=2)
		{
			int col = 0xffffff-((i+32)+((i+64)<<8)+(i<<16));
			setColor(col);
			fillRect(0,i,X_RES,2);
		}
		setColor(0);
		drawString(FONT_TRUETYPE, "DICEWARS DS", X_CEN, Y_CEN-17-1, ALIGN_HCT);
		drawString(FONT_TRUETYPE, "NEO Summer 2007 version", X_CEN, Y_CEN+3-1, ALIGN_HCT);
		drawString(FONT_TRUETYPE, "DICEWARS DS", X_CEN+1, Y_CEN-17, ALIGN_HCT);
		drawString(FONT_TRUETYPE, "NEO Summer 2007 version", X_CEN+1, Y_CEN+3, ALIGN_HCT);
		drawString(FONT_TRUETYPE, "DICEWARS DS", X_CEN, Y_CEN-17+1, ALIGN_HCT);
		drawString(FONT_TRUETYPE, "NEO Summer 2007 version", X_CEN, Y_CEN+3+1, ALIGN_HCT);
		drawString(FONT_TRUETYPE, "DICEWARS DS", X_CEN-1, Y_CEN-17, ALIGN_HCT);
		drawString(FONT_TRUETYPE, "NEO Summer 2007 version", X_CEN-1, Y_CEN+3, ALIGN_HCT);
		setColor(0xffffff);
		drawString(FONT_TRUETYPE, "DICEWARS DS", X_CEN, Y_CEN-17, ALIGN_HCT);
		drawString(FONT_TRUETYPE, "NEO Summer 2007 version", X_CEN, Y_CEN+3, ALIGN_HCT);
		blitScreen();
		int curTime = getTime();
		while(true)
		{
			if(getTime()-curTime>5000) // exit splash after 3 seconds
				break;
		}
	#endif

	initIniFile("dicewars.ini");
	char *sAddr = readIniFileString("serverAddress");
	int sPort = readIniFileInt("serverPort");
	char *pName = readIniFileString("playerName");
	int fColor = readIniFileInt("favouriteColor");
	closeIniFile();

	if(sAddr!=NULL) serverAddress = sAddr;
	if(pName!=NULL) { memset(playerName, 0, MAX_NAME_LENGTH); memcpy(playerName, pName, strlen(pName)); }
	if(sPort!=-1) { serverPort = sPort; }
	if(fColor!=-1) { favouriteColor = fColor-1; }

	if(argc>1)
		serverPort = atoi(argv[1]);

	gameInited = true;
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
	
	logPrintf("QUIT GAME\n");
	fflush(stdout);
	SDL_Quit();
	
	return 0;
}

int last_mouseX=-1;
int last_mouseY=-1;

void setActiveScreen(int display)
{
	if(display<NUM_SCREENS)
		activeScreen = display;
}

boolean eventCallback()
{
	int key;
	SDL_Event event;

	int mousex, mousey;
	SDL_GetMouseState(&mousex, &mousey);

	if(gameState==GAME_STATE_EXIT)
		return false;

	if (mousex*SCALE!=last_mouseX||(mousey*SCALE-((Y_RES*SCALE)*(NUM_SCREENS-1)))!=last_mouseY)
	{
		last_mouseX=mousex*SCALE;
		last_mouseY=mousey*SCALE-(Y_RES*SCALE)*(NUM_SCREENS-1);

		pointerX=mousex/SCALE;
		pointerY=mousey/SCALE-Y_RES*(NUM_SCREENS-1);
		pointerMoved=true;
	}
	else
	{
		pointerMoved=false;
	}
	while (SDL_PollEvent( &event ) ) {
		switch (event.type) {
			case SDL_QUIT:
				return false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym==SDLK_F4&&event.key.keysym.mod&KMOD_ALT)
					return false;
				else
					keyPressed(event.key.keysym.sym);
				break;

			case SDL_KEYUP:
				keyReleased(event.key.keysym.sym);
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button==SDL_BUTTON_LEFT)
				{
					keyPressed(KEY_POINTER);
					pointerPressX=mousex/SCALE;
					pointerPressY=mousey/SCALE-Y_RES*(NUM_SCREENS-1);
					pointerX=mousex/SCALE;
					pointerY=mousey/SCALE-Y_RES*(NUM_SCREENS-1);
					pointerMoved=true;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button==SDL_BUTTON_LEFT)
				{
					keyReleased(KEY_POINTER);
					pointerX=mousex/SCALE;
					pointerY=mousey/SCALE-Y_RES*(NUM_SCREENS-1);
					pointerMoved=true;
				}
				break;

			default:
				break;
		}
	}

	fflush(stdout);
	return true;
}

unsigned long getTime()
{
	return SDL_GetTicks();
}

void reboot()
{
	exit(1);
}
