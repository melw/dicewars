#ifdef OSX
#include "fmod.h"
#include "fmod_errors.h"
#else
#include "fmod/fmod.h"
#include "fmod/fmod_errors.h"
#endif

FMOD_SYSTEM *sfx_system;
FMOD_SOUND *sfx_sounds[NUM_SOUNDS];
FMOD_CHANNEL *sfx_channels[NUM_SOUNDS];
FMOD_RESULT sfx_result;
FMOD_BOOL sfx_isPlaying;
FMOD_CHANNELGROUP *sfx_masterChannelGroup;

#define ERRCHECK(res) ERRCHECK_(res,__FILE__,__LINE__)
void ERRCHECK_(FMOD_RESULT result, char *file, int line)
{
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s  (%s:%d)", result, FMOD_ErrorString(result),file,line);
		exit(-1);
	}
}

void initSound()
{
	sfx_result = FMOD_System_Create(&sfx_system);
	ERRCHECK(sfx_result);

	for(int i=0;i<NUM_SOUNDS;i++)
	{
		sfx_sounds[i] = NULL;
		sfx_channels[i] = 0;
	}

	sfx_result = FMOD_System_Init(sfx_system, 32, FMOD_INIT_NORMAL, NULL);
	ERRCHECK(sfx_result);

	sfx_result = FMOD_System_GetMasterChannelGroup(sfx_system, &sfx_masterChannelGroup);
	ERRCHECK(sfx_result);

	for (int i=0;i<NUM_SOUNDS;i++)
		loadSound(i);
	
	logPrintf("FMOD sound system inited.\n");
}

void loadSound(int index)
{
	logPrintf("load sound: %d\n", index);

	FMOD_CREATESOUNDEXINFO exinfo;

	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.length = getSoundLength(index);

	sfx_result = FMOD_System_CreateSound(sfx_system, (const char*)getSound(index), (FMOD_MODE)(FMOD_SOFTWARE | FMOD_OPENMEMORY | FMOD_LOOP_OFF | FMOD_2D), &exinfo, &sfx_sounds[index]);

	ERRCHECK(sfx_result);
}

void unloadSound(int index)
{
	if (sfx_sounds[index] != NULL)
		sfx_result = FMOD_Sound_Release(sfx_sounds[index]);
	sfx_sounds[index] = NULL;
}

void playSoundLooping(int index, int looping)
{
	FMOD_System_Update(sfx_system);

	FMOD_Channel_IsPlaying(sfx_channels[index], &sfx_isPlaying);
	if(sfx_channels[index]!=0&&sfx_isPlaying==true)
		FMOD_Channel_Stop(sfx_channels[index]);

	if(looping==1) // play only once
	{
		FMOD_Sound_SetMode(sfx_sounds[index], FMOD_LOOP_OFF);
	}
	else
	{
		FMOD_Sound_SetMode(sfx_sounds[index], FMOD_LOOP_NORMAL);
		if(!looping)
			FMOD_Sound_SetLoopCount(sfx_sounds[index], -1); // infinity
		else
			FMOD_Sound_SetLoopCount(sfx_sounds[index], looping-1);
	}

	FMOD_CHANNEL *channel = 0;
	sfx_result = FMOD_System_PlaySound(sfx_system, FMOD_CHANNEL_FREE, sfx_sounds[index], false, &channel);

	ERRCHECK(sfx_result);
	sfx_channels[index] = channel;
}

void stopSound(int index)
{
	if(sfx_sounds[index]!=NULL)
		FMOD_Channel_Stop(sfx_channels[index]);
}

