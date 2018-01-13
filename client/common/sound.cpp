
// yep yep, i know - index these as well

byte* getSound(int index)
{
	#ifndef NOSOUND
		if(index==SOUND_SELECT_INDEX)      return SOUND_SELECT_DATA;
		if(index==SOUND_RESELECT_INDEX)    return SOUND_RESELECT_DATA;
		if(index==SOUND_KEYBOARD_INDEX)    return SOUND_KEYBOARD_DATA;
		if(index==SOUND_DICE_INDEX)        return SOUND_DICE_DATA;
		if(index==SOUND_ATTACK_WON_INDEX)  return SOUND_ATTACK_WON_DATA;
		if(index==SOUND_ATTACK_LOST_INDEX) return SOUND_ATTACK_LOST_DATA;
		if(index==SOUND_END_TURN_INDEX)    return SOUND_END_TURN_DATA;
		if(index==SOUND_DEATH_INDEX)       return SOUND_DEATH_DATA;
		if(index==SOUND_START_GAME_INDEX)  return SOUND_START_GAME_DATA;
		if(index==SOUND_GAME_OVER_INDEX)   return SOUND_GAME_OVER_DATA;
		if(index==SOUND_GAME_WON_INDEX)    return SOUND_GAME_WON_DATA;
/*		if(index==SOUND_MENU_LOOP_INDEX)   return SOUND_MENU_LOOP_DATA;
		#ifdef DEBUG
			if(index==SOUND_TEST_INDEX)      return SOUND_TEST_DATA;
		#endif*/
	#endif
	return NULL;
}

int getSoundLength(int index)
{
	#ifndef NOSOUND
		if(index==SOUND_SELECT_INDEX)      return SOUND_SELECT_LENGTH;
		if(index==SOUND_RESELECT_INDEX)    return SOUND_RESELECT_LENGTH;
		if(index==SOUND_KEYBOARD_INDEX)    return SOUND_KEYBOARD_LENGTH;
		if(index==SOUND_DICE_INDEX)        return SOUND_DICE_LENGTH;
		if(index==SOUND_ATTACK_WON_INDEX)  return SOUND_ATTACK_WON_LENGTH;
		if(index==SOUND_ATTACK_LOST_INDEX) return SOUND_ATTACK_LOST_LENGTH;
		if(index==SOUND_END_TURN_INDEX)    return SOUND_END_TURN_LENGTH;
		if(index==SOUND_DEATH_INDEX)       return SOUND_DEATH_LENGTH;
		if(index==SOUND_START_GAME_INDEX)  return SOUND_START_GAME_LENGTH;
		if(index==SOUND_GAME_OVER_INDEX)   return SOUND_GAME_OVER_LENGTH;
		if(index==SOUND_GAME_WON_INDEX)    return SOUND_GAME_WON_LENGTH;
/*		if(index==SOUND_MENU_LOOP_INDEX)   return SOUND_MENU_LOOP_LENGTH;
		#ifdef DEBUG
			if(index==SOUND_TEST_INDEX)      return SOUND_TEST_LENGTH;
		#endif*/
	#endif
	return -1;
}

void playSound(int index)
{
	playSoundLooping(index, 1);
}

void resetSound()
{
	for (int i=0;i<NUM_SOUNDS;i++)
		stopSound(i);
}
