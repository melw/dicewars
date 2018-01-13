#include <nds.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <dswifi9.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include <fat.h>
#include <sys/dir.h>
#include <errno.h>
#include "reboot.h"

#ifdef GDB
	#include "gdbstub.h"
	#include "gdbstub_tcp.h"
#endif

#ifdef LOBBY
	#include "MessageQueue.h"
	#include "802.11.h"
	#include "lobby.h"
#endif

#include "../../../common/dicewars_version.h"
#include "../../../common/common.h"
#include "../../../common/gamelogic.h"
#include "../../../common/network.h"
#include "../../../common/inifile.h"
#include "../../../common/database.h"
#include "../../../common/gamestatistics.h"
#include "../../common/game.h"
#include "../../common/image.h"
#include "../../common/font.h"
#include "../../common/keyboard.h"
#include "../../common/menu.h"
#include "../../common/network.h"
#include "../../common/sound.h"
#include "../../common/connectivity.h"
#include "../../common/lobby.h"

#define MESSAGE_PLAY_SOUND 1 
#define MESSAGE_STOP_SOUND 2
#define MESSAGE_WIFI_INIT 3 // (0x12345678)
#define MESSAGE_WIFI_SYNC 4 // (0x87654321)

#define IPCTAG_PLAY_SOUND 10
#define IPCTAG_STOP_SOUND 11

typedef struct FIFO_message { 
	u8 command;
	u8 soundIndex;
	u8 loopCount;
	TransferSoundData sounddata;
} FIFO_message, *pFIFO_message; 

//TransferSound transferSound;
//int soundCount = 0;

//#define sound_fifo ((transferMessage)((uint32)(IPC)+sizeof(TransferRegion))) 

static struct FIFO_message transfer[16];

int fifo_lastmsg=0;

void lidSleep()
{
	__asm("mcr p15,0,r0,c7,c0,4");
	__asm("mov r0, r0");
	__asm("BX lr"); 
}

void SendArm7Command(u32 command)
{
	while (REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL);
	if (REG_IPC_FIFO_CR & IPC_FIFO_ERROR) { 
		REG_IPC_FIFO_CR |= IPC_FIFO_SEND_CLEAR; 
	} 
	
	REG_IPC_FIFO_TX = command; 
} 

//void FifoHandler(void);
void Timer_50ms() { Wifi_Timer(50); }
void arm9_synctoarm7() { 
//	REG_IPC_FIFO_TX = DSWIFI_SYNC;
	pFIFO_message transfer2 = (pFIFO_message)( ((u32)&transfer[0]) | 0x00400000); 
	
	transfer2[fifo_lastmsg].command = MESSAGE_WIFI_SYNC;
	SendArm7Command((u32)&transfer[fifo_lastmsg]);
	fifo_lastmsg=(fifo_lastmsg+1)&15; 	
}

void arm9_fifo() { // check incoming fifo messages
	u32 value = REG_IPC_FIFO_RX;
	if(value == 0x87654321) Wifi_Sync();
}

/*void sgIP_dbgprint(char * txt, ...) {
	char buffer[256];
	va_list args;
	va_start(args,txt);
	vsprintf(buffer,txt,args);
	printf(buffer);		
}*/

void logPrintf(char *msg, ...)
{
	#ifdef DEBUGSCREEN
	if(msg)
	{
		va_list argp;
		va_start(argp, msg);
		vprintf(msg, argp);
		va_end(argp);
	}

	fflush(stdout);
	#endif
}

void sgIP_dbgprint(char *msg, ...)
{
	#ifdef DEBUGSCREEN
	if(msg)
	{
		va_list argp;
		va_start(argp, msg);
		vprintf(msg, argp);
		va_end(argp);
	}

	fflush(stdout);
	#endif
}

//#define sgIP_dbgprint(...) logPrintf(...)

#include "../../../common/gamelogic.cpp"
#include "../../../common/network.cpp"
#include "../../../common/inifile.cpp"
#include "../../../common/database.cpp"
#include "../../common/font.cpp"
#include "../../common/keyboard.cpp"
#include "../../common/image.cpp"
#include "../../common/menu.cpp"
#include "../../common/game.cpp"
#include "../../common/network.cpp"
#include "../../common/sound.cpp"
#include "../../common/connectivity.cpp"
#include "../../common/lobby.cpp"

//#include "sound.cpp"

u16* doubleBuffer;
u16* subDoubleBuffer;

int activeBuffer;
int globalTimer;
boolean gameInited = false;

touchPosition touchXY;

#ifdef DEBUG
int fpsCounter1, fpsCounter2, globalTimerLast;
float fps = 60.0f;

#endif

/*void initSound()
{
	#ifdef NOSOUND
		return;
	#endif
	// init fifo control
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ; 
// original:  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;
// 	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR; 
	
	fifo_lastmsg = 0;
}*/

void loadSound(int index)
{
	// no implementation needed on ds
}
/*
void playSoundLooping(int index, int looping)
{
	#ifdef NOSOUND
		return;
	#endif
	
	int len = getSoundLength(index);
	unsigned long length = 65536; // 5+len; // header 3 bytes
	char *IPC_buffer = (char *)malloc(length);
	IPC_buffer[0] = IPCTAG_PLAY_SOUND; // command
	IPC_buffer[1] = index; // sound index
	IPC_buffer[2] = looping; // loop count
	IPC_buffer[3] = (len%256); IPC_buffer[4] = (len>>8)%256;
	//IPC_buffer = (char*)getSound(index)+0x2C; // skip wav header
	memcpy(IPC_buffer+5, (char*)getSound(index)+0x2C, len); // skip wav header
	IPC_SendMessage(IPC_buffer,length);
//	IPC_PassCustomMessage(IPC_buffer,length);
	free(IPC_buffer);
}
*/

void playSoundLooping(int index, int looping)
{
	#ifdef NOSOUND
		return;
	#endif
/*
	unsigned long length = 3+1500; // getSoundLength(index)-(0x2C);
	char *IPC_buffer = (char *)malloc(length);
	IPC_buffer[0] = IPCTAG_PLAY_SOUND; // command
	IPC_buffer[1] = index; // sound index
	IPC_buffer[2] = looping; // loop count
	for(int i=0;i<1500;i++)
		IPC_buffer[3+i] = rand()%255;
//	memcpy(IPC_buffer+3, (char*)(getSound(index)+0x2C), length-3);
	
	free(IPC_buffer);	
*/

	playGenericSound(getSound(index)+0x2C, getSoundLength(index)-(0x2C<<4));

	#ifdef DEBUG
//	sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "PLAY: %d (%d)", index, getSoundLength(index));
	#endif
}

void stopSound(int index)
{
	#ifdef NOSOUND
		return;
	#endif
	if(index<NUM_SOUNDS)
	{
		unsigned long length = 2;
		unsigned char *IPC_buffer = (unsigned char *)malloc(length);
		IPC_buffer[0] = IPCTAG_STOP_SOUND; // command
		IPC_buffer[1] = index; // sound index
		IPC_SendMessage((char*)IPC_buffer,length);
		free(IPC_buffer);
	}
}

/*
void playSoundLooping(int index, int looping)
{
	#ifdef NOSOUND
		return;
	#endif
	// assuming we're playing a wav sample
	fifo_lastmsg=(fifo_lastmsg+1)&15; 
	pFIFO_message transfer2 = (pFIFO_message)( ((u32)&transfer[0]) | 0x00400000); 
	
	transfer2[fifo_lastmsg].command = MESSAGE_PLAY_SOUND; 
	transfer2[fifo_lastmsg].soundIndex = index;
	transfer2[fifo_lastmsg].loopCount = looping;

	transfer2[fifo_lastmsg].sounddata.len = getSoundLength(index)-(0x2C<<4); // 0x2C;        // PCM header is 0x2C bytes long
//	if(getSound(index)+34 == 16) transfer2[fifo_lastmsg].sounddata.format = 1; // 16 bits
//	else                         transfer2[fifo_lastmsg].sounddata.format = 0; // 8 bits
	transfer2[fifo_lastmsg].sounddata.format = 1;
	transfer2[fifo_lastmsg].sounddata.data = getSound(index)+0x2C; // skip header
//	int bitrate = sfx_files[sfx_soundDataOffset+(int)sfx_soundStartTable(index)+24]+(sfx_files[sfx_soundDataOffset+(int)sfx_soundStartTable(index)+25]<<8);
//		printf("bitrate: %d\n", bitrate);
	transfer2[fifo_lastmsg].sounddata.rate = 22050; // bitrate; // TODO: hardcoded for testing
	transfer2[fifo_lastmsg].sounddata.vol = 127;
	transfer2[fifo_lastmsg].sounddata.pan = 64;
	SendArm7Command((u32)&transfer[fifo_lastmsg]);
}


void stopSound(int index)
{
	#ifdef NOSOUND
		return;
	#endif
	if(index<NUM_SOUNDS)
	{
		fifo_lastmsg=(fifo_lastmsg+1)&15; 
		pFIFO_message transfer2 = (pFIFO_message)( ((u32)&transfer[0]) | 0x00400000); 
		
		transfer2[fifo_lastmsg].command = MESSAGE_STOP_SOUND;
		transfer2[fifo_lastmsg].soundIndex = index;
		transfer2[fifo_lastmsg].sounddata.data = NULL;
		SendArm7Command((u32)&transfer[fifo_lastmsg]);
	}
}
*/

boolean fileLocator(char *start, char *target, boolean isDir, int depth, char *result)
{
	struct stat st;
	DIR_ITER *dir = diropen(start);
	static char child[256];
	char temp[256];
	
	if (dir)
	{
		while (dirnext(dir, child, &st) == 0)
		{
			if (strlen(child) == 1 && child[0] == '.')
				continue;
						
			if (strlen(child) == 2 && child[0] == '.' && child[1] == '.')
				continue;
			
			if (((st.st_mode & S_IFDIR) && isDir) || (!(st.st_mode & S_IFDIR) && !isDir) )
			{
				if (strcasecmp(target, child) == 0)
				{
					strcpy(result, start);
					if (start[strlen(start)-1] != '/')
					{
						strcat(result, "/");
					}
					strcat(result, child);
					if(isDir)
						strcat(result, "/");
				
					dirclose(dir);
					return true;
				}
			}
				
			if ((st.st_mode & S_IFDIR) && depth > 1)
			{
				strcpy(temp, start);
				if (start[strlen(start)-1] != '/')
				{
					strcat(temp, "/");
				}
				strcat(temp, child);
				strcat(temp, "/");
				
				if (fileLocator(temp, target, isDir, depth-1, result))
				{
					dirclose(dir);
					return true;
				}
			}
		}
	}
	
	dirclose(dir);
	return false;
}

void blitScreen()
{
	if(gameInited)
		paint();
/*	#ifdef DEBUG
	if(fpsCounter1%30==0)
	{
		fps = (fpsCounter1-fpsCounter2)/(((globalTimer-globalTimerLast)/60.0));
		printf("fps:%.1f\n", fps);

		fpsCounter2 = fpsCounter1;
		globalTimerLast = globalTimer;
	}

	fpsCounter1++;

	setColor(0xffffff);
	fillRect(0,0,44,10);
	setColor(0);
	char str[10];
	sprintf(str, "fps: %.1f", fps);
	drawString(FONT_FIXED, str, 1, 0, ALIGN_LT);
	#endif
*/
	swiWaitForVBlank();
//  REG_IME = 0;

	if(activeBuffer==1) {
		doubleBuffer = VRAM_B;
		memcpy(doubleBuffer, screenBuffer[SCREEN_LOWER], 256*256*2);
		videoSetMode(MODE_FB0);
		screenBuffer[SCREEN_LOWER] = VRAM_B;

/*		subDoubleBuffer = VRAM_D;
		memcpy(subDoubleBuffer, screenBuffer[SCREEN_UPPER], 256*256*2);
		vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_LCD); 
	
		subDoubleBuffer = (u16*)BG_BMP_RAM(8);
		memcpy(subDoubleBuffer, screenBuffer[SCREEN_UPPER], 256*256*2);
		screenBuffer[SCREEN_UPPER] = (u16*)BG_BMP_RAM(8);*/


		activeBuffer = 0;

//		vramSetBankC(VRAM_C_SUB_BG_0x06200000);
//		BG3_CR = BG_BMP16_256x256 | BG_BMP_BASE(8) | BG_PRIORITY(3);
	} else {
		doubleBuffer = VRAM_A;
		memcpy(doubleBuffer, screenBuffer[SCREEN_LOWER], 256*256*2);
		videoSetMode(MODE_FB1);
		screenBuffer[SCREEN_LOWER] = VRAM_A;

/*		subDoubleBuffer = (u16*)BG_BMP_RAM(0);
		memcpy(subDoubleBuffer, screenBuffer[SCREEN_UPPER], 256*256*2);
		screenBuffer[SCREEN_UPPER] = (u16*)BG_BMP_RAM(0);

		subDoubleBuffer = VRAM_C;
		memcpy(subDoubleBuffer, screenBuffer[SCREEN_UPPER], 256*256*2);
		vramSetBankC(VRAM_C_LCD);
    vramSetBankD(VRAM_D_SUB_SPRITE);
*/
		activeBuffer = 1;
//
//		vramSetBankC(VRAM_C_SUB_BG_0x06220000);
//		BG3_CR = BG_BMP16_256x256 | BG_BMP_BASE(0) | BG_PRIORITY(3);
	}

//  REG_IME = 1;
}

void setActiveScreen(int display)
{
	// TODO
	activeScreen = display;
}

static int touching;
int last_mouseX=-1;
int last_mouseY=-1;

boolean eventCallback()
{
	if(gameState==GAME_STATE_EXIT)
		return false;

	scanKeys();

	touchXY=touchReadXY();
	
	int pressed = keysDown();
	int keysup = keysUp();    
	int held = keysHeld();

	/* if lid 'key' */
	if (pressed & KEY_LID) {
		/* hinge is closed */
		/* set only key irq for waking up */
		unsigned long oldIE = REG_IE;
		REG_IE = IRQ_KEYS;
		*(volatile unsigned short *)0x04000132 = BIT(14) | 255; /* any of the inner keys might irq */
		/* power off everything not needed */
		powerOFF(POWER_LCD);
		/* set system into sleep */
		lidSleep();
		/* wait a bit until returning power */
		while (REG_VCOUNT!=0);
		while (REG_VCOUNT==0);
		while (REG_VCOUNT!=0);
		/* power on again */
		powerON(POWER_LCD);
		/* set up old irqs again */
		REG_IE = oldIE;
	}; 
   
	if ( !(held & KEY_TOUCH) || touchXY.x == 0 || touchXY.y == 0)
	{
		if(touching==1) // artificial click
		{
			touching = 0;

			keyReleased(KEY_POINTER);
			pointerX=-1; // last_mouseX;
			pointerY=-1; // last_mouseY;
			pointerMoved=true;
		}
	}
	else
	{
		touching = 1;
		
		if ( pressed & KEY_TOUCH ) // first touch
		{
			pointerMoved=true;
			pointerPressX=touchXY.px;
			pointerPressY=touchXY.py;
			
			keyPressed(KEY_POINTER);
		}
				
		// pointer moved
		if(last_mouseX!=touchXY.px||last_mouseY!=touchXY.py)
			pointerMoved=true;
		else
			pointerMoved=false;
	
		// set new pointer position
		pointerX = touchXY.px;
		pointerY = touchXY.py;

		last_mouseX = touchXY.px;
		last_mouseY = touchXY.py;
	}

	if(pressed>0)
	{
		if (pressed & KEY_A) keyPressed(KEY_SELECT);
		if (pressed & KEY_B) keyPressed(KEY_BACK);
		if (pressed & KEY_UP) keyPressed(KEY_UP);
		if (pressed & KEY_DOWN) keyPressed(KEY_DOWN);
		if (pressed & KEY_RIGHT) keyPressed(KEY_RIGHT);
		if (pressed & KEY_LEFT) keyPressed(KEY_LEFT);
		if (pressed & KEY_START) keyPressed(KEY_ESCAPE);
	}
	if(keysup>0)
	{
		if (keysup & KEY_A) keyReleased(KEY_SELECT);
		if (keysup & KEY_B)	keyReleased(KEY_BACK);
		if (keysup & KEY_UP) keyReleased(KEY_UP);
		if (keysup & KEY_DOWN) keyReleased(KEY_DOWN);
		if (keysup & KEY_RIGHT) keyReleased(KEY_RIGHT);
		if (keysup & KEY_LEFT) keyReleased(KEY_LEFT);
		if (keysup & KEY_START) keyReleased(KEY_ESCAPE);
	}
		
	return true;
}

unsigned long getTime()
{
	// TODO: real milliseconds
	return globalTimer*16;
}

static void on_irq()
{
	globalTimer++; // 60fps timer
	REG_IF = IRQ_VBLANK;
	
	#ifdef LOBBY
		IPC_RcvCompleteCheck();
		LOBBY_Update();
	#endif
}

int main(void)
{
//	#if defined DEBUG | defined DEBUGSCREEN
//	defaultExceptionHandler();
//	#endif
	REG_EXMEMCNT=0xe800;

	powerON(POWER_ALL);
	REG_IME = 0 ;
	REG_IE = 0 ;
	REG_IF = 0xFFFF ;
	#ifndef LOBBY
	  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;
	#endif
	lcdMainOnBottom();
	globalTimer=0;
	activeBuffer=0;

	defaultExceptionHandler() ;

	setGenericSound(	22050,	/* sample rate */
						127,	/* volume */
						64,		/* panning */
						1 );	/* sound format*/

  for(int i=0;i<NETWORK_MAX_THREADS;i++)
  {
    threadGameIndex[i]=threadPingCounter[i]=threadPongCounter[i]=-1;
    threadPingTimer[i]=0;
    threadIds[i]=0;
    threadConnected[i]=false;
  }
	initGame();
	
	#ifndef LOBBY
		*((volatile u16 *)0x0400010E) = 0;
	#endif
	irqInit();

	lobbyPreInit();

	irqSet(IRQ_VBLANK, &on_irq);
	irqEnable(IRQ_VBLANK) ;

	lobbyInit();

/*	irqSet(IRQ_FIFO_NOT_EMPTY, arm9_fifo);
	irqEnable(IRQ_FIFO_NOT_EMPTY);
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;
	initSound();*/

	videoSetMode(MODE_FB0);
	vramSetMainBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_SUB_BG, VRAM_D_SUB_SPRITE);

	screenBuffer[SCREEN_LOWER] = VRAM_B;
	doubleBuffer = VRAM_A;

	#ifdef DEBUGSCREEN
		videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
		vramSetBankC(VRAM_C_SUB_BG);
		SUB_BG0_CR = BG_MAP_BASE(31);	
		BG_PALETTE_SUB[255] = RGB15(31,31,31);
		consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(31), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);
	#else
		videoSetModeSub(MODE_0_2D |
						DISPLAY_SPR_1D_LAYOUT |
						DISPLAY_SPR_ACTIVE |
						DISPLAY_BG0_ACTIVE |
						DISPLAY_BG1_ACTIVE );
	
//		videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
		videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D_BMP_256); 
		vramSetBankC(VRAM_C_SUB_BG_0x06200000);
//		subDoubleBuffer = (u16*)BG_BMP_RAM(8);
		
		SUB_BG2_CR = BG_BMP16_256x256; //  | BG_BMP_BASE(0) | BG_PRIORITY(3);
	
		SUB_BG2_XDX = 1<<8;
		SUB_BG2_XDY = 0;
		SUB_BG2_YDX = 0;
		SUB_BG2_YDY = 1<<8;
		SUB_BG2_CY = 0;
		SUB_BG2_CX = 0;

		// M3 leaves garbage to VRAM banks, clear everything first
		for (int i = 0; i < (131072/4 * 4); i++) { 
			BG_GFX[i] = 0;
			BG_GFX_SUB[i] = 0;
		}
	#endif
	screenBuffer[SCREEN_UPPER] = (uint16*)0x06200000;
	
	fatInitDefault();
	srand(time(NULL));
	initTimer(LOCAL);

	#ifdef NEOFLASH
		logPrintf("NEOFLASH splash screen...\n");
		setActiveScreen(SCREEN_UPPER);
		drawImage(IMAGE_NEOFLASH_INDEX, 0, 0, ALIGN_LT);
		setActiveScreen(SCREEN_LOWER);
		for(int i=0;i<192;i+=2)
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
		int curTime = getTime();
		while(true)
		{
			blitScreen();
			if(getTime()-curTime>5000) // exit splash after 3 seconds
				break;
		}
	#endif

	// init default player name from the ds firmware	
	if(PersonalData->name[0]) // prevent loading empty name / emulator check
	{
		for(int i=0;i<MAX_NAME_LENGTH;i++)
		{
			if(i<PersonalData->nameLen)
			{
				playerName[LOCAL][i] = PersonalData->name[i];
			}
			else
				playerName[LOCAL][i] = 0;
		}
		#ifdef LOBBY
			LOBBY_SetOwnName(playerName[LOCAL]);
		#endif
	}

	char *target = "dicewars.ini";
	char iniFileName[256] = {0};	
	if(fileLocator("/", target, false, 3, iniFileName)) // found dicewars.ini, read the values from there
	{
//		initIniFile("/games/dicewars.ini");
		initIniFile(iniFileName);
		char *sAddr = readIniFileString("serverAddress");
		int sPort = readIniFileInt("serverPort");
		char *pName = readIniFileString("playerName");
		int fColor = readIniFileInt("favouriteColor");
		closeIniFile();
	
		if(sAddr!=NULL) serverAddress = sAddr;
		if(pName!=NULL) { memset(playerName, 0, MAX_NAME_LENGTH); memcpy(playerName, pName, strlen(pName)); }
		if(sPort!=-1) { serverPort = sPort; }
		if(fColor!=-1) { favouriteColor = fColor-1; }
	}

	#ifdef GDB
    struct init_data_tcp_comms_gdbstub init_data;
    init_data.port = 30000;
    logPrintf("Initialising GDB stub\n");
    if ( !init_gdbstub( &tcpCommsIf_gdbstub, &init_data)) {
      logPrintf("Failed to initialise the debugger stub - cannot continue\n");
      while(1);
    }
		logPrintf("GDB init done.\n");
		networkInitCounter = 5;
		networkInitialized = true;
		
	  haltTarget_gdbstub();
	#endif

	gameInited = true;
	int done=0;	
	while ( !done )
	{
		if(eventCallback())
		{
			update(LOCAL);
			blitScreen();
		}
		else
			done = 1;
	}

	return 0;
}

