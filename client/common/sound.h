#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#define NUM_SOUNDS 11

#ifndef NOSOUND
	#include "../data/sfx_select.h"
	#include "../data/sfx_reselect.h"
	#include "../data/sfx_keyboard.h"
	#include "../data/sfx_dice.h"
	#include "../data/sfx_attack_won.h"
	#include "../data/sfx_attack_lost.h"
	#include "../data/sfx_end_turn.h"
	#include "../data/sfx_death.h"
	#include "../data/sfx_start_game.h"
	#include "../data/sfx_game_over.h"
	#include "../data/sfx_game_won.h"
/*	#include "../data/sfx_menu_loop.h"
	#ifdef DEBUG
		#include "../data/sfx_test.h"
	#endif*/
#else
	#define SOUND_SELECT_INDEX      (-1)
	#define SOUND_RESELECT_INDEX    (-1)
	#define SOUND_KEYBOARD_INDEX    (-1)
	#define SOUND_DICE_INDEX        (-1)
	#define SOUND_ATTACK_WON_INDEX  (-1)
	#define SOUND_ATTACK_LOST_INDEX (-1)
	#define SOUND_END_TURN_INDEX    (-1)
	#define SOUND_DEATH_INDEX       (-1)
	#define SOUND_START_GAME_INDEX  (-1)
	#define SOUND_GAME_OVER_INDEX   (-1)
	#define SOUND_GAME_WON_INDEX    (-1)
/*	#define SOUND_MENU_LOOP_INDEX   (-1)
	#ifdef DEBUG
		#define SOUND_TEST_INDEX (-1)
	#endif*/
#endif

byte* getSound(int index);
int getSoundLength(int index);

void initSound(void);
void loadSound(int index);
void unloadSound(int index);
void playSound(int index);
void playSoundLooping(int index, int looping);
void stopSound(int index);
void resetSound(void);

#endif // SOUND_H_INCLUDED
