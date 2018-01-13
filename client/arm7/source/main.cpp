#include <nds.h>
#include <stdlib.h>
#include <dswifi7.h>
#include "reboot.h"
#ifdef LOBBY
	#include "smi_startup_viaipc.h"
	#include "MessageQueue.h"
	#include "wifi_hal.h"
#endif

#define DSWIFI_INIT (0x12345678)
#define DSWIFI_SYNC (0x87654321)

#define MESSAGE_PLAY_SOUND 1 
#define MESSAGE_STOP_SOUND 2 
#define MESSAGE_WIFI_INIT 3 // (0x12345678)
#define MESSAGE_WIFI_SYNC 4 // (0x87654321)

#define IPCTAG_PLAY_SOUND 10
#define IPCTAG_STOP_SOUND 11

vu8* soundData;

s8 channelIndex[16];
s8 soundPlayed[256]; // max sounds limited to 256...

TransferSound transferSound;

s8 getFreeSoundChannel() {
	// force single sound
	// return 0;
  for (int i=0; i<16; i++) {
    if ( (SCHANNEL_CR(i) & SCHANNEL_ENABLE) == 0 ) return i;
  }
  return -1;
}

typedef struct FIFO_message
{
	u8 command;
	u8 soundIndex;
	u8 loopCount;
	TransferSoundData sounddata;
} FIFO_message, *pFIFO_message;

//#define sound_fifo ((FIFO_message*)((uint32)(IPC)+sizeof(TransferRegion))) 
#define sound_fifo ((TransferMessage)((uint32)(IPC)+sizeof(TransferRegion))) 

void toggleSignBit(unsigned char *sample, u32 len)
{
  for(; len > 0; --len, ++sample)
  {
    *sample ^= 0x80;
  }
}

void startSound(int soundIndex, int sampleRate, const void* data, uint32 bytes, u8 vol=0x7F, u8 pan=63, u8 format=0, u16 loopCount=1)
{
  REG_IME = IME_DISABLE;

	s8 channel = -1;
	for(int i=0;i<16;i++) // if we were playing the same sound, use the same channel
	{
		if(channelIndex[i] == soundIndex)
		{
			channel = i;
			break; 
		}
	}
	if(channel==-1)
	  channel = getFreeSoundChannel();
	if(channel<0)
		return;

  bytes &= ~7;		// Multiple of 4 bytes!

  uint32 flags = SCHANNEL_ENABLE | SOUND_VOL(vol) | SOUND_PAN(pan);
  
  if(format==0) { // 8-bit wav
  	flags |= SOUND_8BIT | SOUND_FORMAT_8BIT;
  	if(soundPlayed[soundIndex]==-1) {
	  	toggleSignBit((unsigned char*)data, bytes);
	  	soundPlayed[soundIndex]=1;
	  }
  } else if(format==1) { // 16-bit wav
  	flags |= SOUND_FORMAT_16BIT;
  } else if(format==2) { // 16-bit adpcm
  	flags |= SOUND_FORMAT_ADPCM;
  } else if(format==3) { // 8-bit raw
  	flags |= SOUND_8BIT | SOUND_FORMAT_8BIT;
  }
  
  SCHANNEL_CR(channel) = 0;
  SCHANNEL_TIMER(channel)  = SOUND_FREQ(sampleRate);
  SCHANNEL_SOURCE(channel) = ((uint32) (data));
  SCHANNEL_LENGTH(channel) = ((bytes & 0x7FFFFFFF) >> 2);
  SCHANNEL_REPEAT_POINT(channel) = 0;

  SCHANNEL_CR(channel + 2) = 0;
  SCHANNEL_TIMER(channel + 2)  = SOUND_FREQ(sampleRate);
  SCHANNEL_SOURCE(channel + 2) = ((uint32) (data));
  SCHANNEL_LENGTH(channel + 2) = ((bytes & 0x7FFFFFFF) >> 2);
  SCHANNEL_REPEAT_POINT(channel + 2) = 0;
  
  if(loopCount==1)
  	flags |= SOUND_ONE_SHOT;
  else 
  	flags |= SOUND_REPEAT; // TODO: loops forever! implement 2..n loopcounts

  soundData = (vu8* ) data;

  SCHANNEL_CR(channel)     = flags;
  SCHANNEL_CR(channel + 2) = flags;

	channelIndex[channel] = soundIndex;
	
  REG_IME = IME_ENABLE;
}

void stopSound(int chan)
{
	SCHANNEL_CR(chan) = 0;
  SCHANNEL_CR(chan + 2) = 0;
	channelIndex[chan] = -1;
}

int vcount;
touchPosition first,tempPos;

//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	static int lastbut = -1;
	
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0;

	but = REG_KEYXY;

	// Check if the lid has been closed.
	if(but & IPC_LID_CLOSED)
	{
		// Save the current interrupt sate.
		u32 ie_save = REG_IE;
		// Turn the speaker down.
		swiChangeSoundBias(0,0x400);
		// Save current power state.
		int power = readPowerManagement(PM_CONTROL_REG);
		// Set sleep LED.
		writePowerManagement(PM_CONTROL_REG, PM_LED_CONTROL(1));
		// Register for the lid interrupt.
		REG_IE = IRQ_LID;
		
		// Power down till we get our interrupt.
		swiSleep(); //waits for PM (lid open) interrupt
		
		REG_IF = ~0;
		// Restore the interrupt state.
		REG_IE = ie_save;
		// Restore power state.
		writePowerManagement(PM_CONTROL_REG, power);
		
		// Turn the speaker up.
		swiChangeSoundBias(1,0x400);
	}

	if (!( (but ^ lastbut) & (1<<6))) {
 
		tempPos = touchReadXY();

		if ( tempPos.x == 0 || tempPos.y == 0 ) {
			but |= (1 <<6);
			lastbut = but;
		} else {
			x = tempPos.x;
			y = tempPos.y;
			xpx = tempPos.px;
			ypx = tempPos.py;
			z1 = tempPos.z1;
			z2 = tempPos.z2;
		}
		
	} else {
		lastbut = but;
		but |= (1 <<6);
	}

	if ( vcount == 80 ) {
		first = tempPos;
	} else {
		if (	abs( xpx - first.px) > 10 || abs( ypx - first.py) > 10 ||
				(but & ( 1<<6)) ) {

			but |= (1 <<6);
			lastbut = but;

		} else { 	
			IPC->mailBusy = 1;
			IPC->touchX			= x;
			IPC->touchY			= y;
			IPC->touchXpx		= xpx;
			IPC->touchYpx		= ypx;
			IPC->touchZ1		= z1;
			IPC->touchZ2		= z2;
			IPC->mailBusy = 0;
		}
	}
	IPC->buttons		= but;
	vcount ^= (80 ^ 130);
	SetYtrigger(vcount);
}

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------

//	#ifndef LOBBY
//		Wifi_Update();
//	#endif

	//sound code  :)
	TransferSound *snd = IPC->soundData;
	IPC->soundData = 0;

	if (0 != snd) {

		for (int i=0; i<snd->count; i++) {
			s32 chan = getFreeSoundChannel();

			if (chan >= 0) {
				startSound(chan, snd->data[i].rate, snd->data[i].data, snd->data[i].len, snd->data[i].vol, snd->data[i].pan, snd->data[i].format, 1);
			}
		}
	}
}

//=======================================================
void arm7_synctoarm9() {
//=======================================================
   REG_IPC_FIFO_TX = DSWIFI_SYNC;
}

enum {
//---------------------------------------------------------------------------------
	CMD_WAIT,
	WIFI_INIT,
};
	
u32 fifo_status = CMD_WAIT;

//=======================================================
void fifoCallback(unsigned char *data, int length) { // check incoming fifo messages
//=======================================================
	//pFIFO_message message = (pFIFO_message)data; //  REG_IPC_FIFO_RX; 
		
//	switch(message->command)
	switch(data[0])
	{
		case IPCTAG_PLAY_SOUND:
		{
//			startSound(message->soundIndex, message->sounddata.rate, message->sounddata.data, message->sounddata.len, message->sounddata.vol, message->sounddata.pan, message->sounddata.format, message->loopCount); 
/*
			int count = data[3];
			startSound(data[1],
			           transferSound.data[count].rate,
			           transferSound.data[count].data,
			           transferSound.data[count].len,
			           transferSound.data[count].vol,
			           transferSound.data[count].pan,
			           transferSound.data[count].format,
			           22050,
			           data+4,
			           10000,
			           127,
			           63,
			           1,
			           data[2]);*/
			//startSound(data[1], 22050, data+3, length-3, 127, 64, 1, 1); // data[2]);
// 			startSound(data[1], 22050, data+3, 100000, 127, 64, 1, 1); // data[2]);
			break;
		}
		
		case IPCTAG_STOP_SOUND:
		{
			for(int i=0;i<16;i++)
			{
//				if(channelIndex[i]==message->soundIndex)
				if(channelIndex[i]==data[1])
				{
					stopSound(i);
				}
			}
			break;		
		}
	}
}

/*void arm7_fifo() { // check incoming fifo messages
//=======================================================
  while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
  {
		if(fifo_status==WIFI_INIT)
		{
			Wifi_Init(REG_IPC_FIFO_RX);
			Wifi_SetSyncHandler(arm7_synctoarm9); // allow wifi lib to notify arm9
			fifo_status = CMD_WAIT;
		}
		else
		{
			pFIFO_message message = (pFIFO_message) REG_IPC_FIFO_RX; 
	
			switch(message->command)
			{
				case MESSAGE_PLAY_SOUND:
				{
					startSound(message->soundIndex, message->sounddata.rate, message->sounddata.data, message->sounddata.len, message->sounddata.vol, message->sounddata.pan, message->sounddata.format, message->loopCount); 
					break;
				}
				
				case MESSAGE_STOP_SOUND:
				{
					for(int i=0;i<16;i++)
					{
						if(channelIndex[i]==message->soundIndex)
						{
							stopSound(i);
						}
					}
					break;		
				}
				
				case MESSAGE_WIFI_INIT:
				{
					fifo_status=WIFI_INIT;
					break;
				}
				
				case MESSAGE_WIFI_SYNC:
				{
					Wifi_Sync();
					break;
				}
			}
		}
  }
}
*/

//---------------------------------------------------------------------------------
int main(int argc, char ** argv) {
//---------------------------------------------------------------------------------

	readUserSettings();

	//enable sound
	powerON(POWER_SOUND);
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);
	for(int i=0;i<16;i++) // reset channel indexes
		channelIndex[i] = -1;
	for(int i=0;i<256;i++) // reset sound indexes
		soundPlayed[i] = -1;

	irqInit();
	initClockIRQ();
	irqSet(IRQ_VBLANK, VblankHandler);
//	irqSet(IRQ_WIFI, Wifi_Interrupt);
	SetYtrigger(80);
	vcount=80;
	irqSet(IRQ_VCOUNT, VcountHandler);
	irqEnable(IRQ_VBLANK);
	irqEnable(IRQ_VCOUNT);

	#ifdef GDB
    u32 fifo_temp;   

    while(1) { // wait for magic number
      while(REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY) swiWaitForVBlank();
      fifo_temp=REG_IPC_FIFO_RX;
      if(fifo_temp==0x12345678) break;
    }

    while(REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY) swiWaitForVBlank();
    fifo_temp=REG_IPC_FIFO_RX; // give next value to wifi_init
    Wifi_Init(fifo_temp);
	#endif    
	
	IPC->mailBusy = 0;

	IPC_Init();
	IPC_SetCustomCallback(&fifoCallback);

	#ifdef LOBBY
		LWIFI_Init();
	#endif

  /*irqSet(IRQ_FIFO_NOT_EMPTY,arm7_fifo); // set up fifo irq
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;
	irqEnable(IRQ_FIFO_NOT_EMPTY | IRQ_VBLANK | IRQ_WIFI | IRQ_VCOUNT);

	Wifi_SetSyncHandler(arm7_synctoarm9);*/
	
	// Keep the ARM7 out of main RAM
	while (1) 
	{
		#ifdef LOBBY
			LWIFI_IPC_UpdateNOIRQ();
		#endif
    if(need_reboot()) reboot();
//		swiWaitForVBlank();
	}
}


