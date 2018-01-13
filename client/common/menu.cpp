
//#include "menu.h"

// TODO: these should be dynamic, yep!

int getMenuType(int menuIndex)
{
	if(menuIndex==MENU_MAIN_INDEX)              return MENU_MAIN_TYPE;
	if(menuIndex==MENU_LOCAL_INDEX)             return MENU_LOCAL_TYPE;
	if(menuIndex==MENU_NETWORK_INDEX)           return MENU_NETWORK_TYPE;
	if(menuIndex==MENU_OPTIONS_INDEX)           return MENU_OPTIONS_TYPE;
	if(menuIndex==MENU_INSTRUCTIONS_INDEX)      return MENU_INSTRUCTIONS_TYPE;
	if(menuIndex==MENU_ABOUT_INDEX)             return MENU_ABOUT_TYPE;
	if(menuIndex==MENU_NETWORK_NEW_GAME_INDEX)  return MENU_NETWORK_NEW_GAME_TYPE;
	if(menuIndex==MENU_NETWORK_JOIN_GAME_INDEX) return MENU_NETWORK_JOIN_GAME_TYPE;
	if(menuIndex==MENU_NETWORK_LOBBY_INDEX)     return MENU_NETWORK_LOBBY_TYPE;
	if(menuIndex==MENU_QUIT_INDEX)              return MENU_QUIT_TYPE;
	if(menuIndex==MENU_PLAYERS_INDEX)           return MENU_PLAYERS_TYPE;
	if(menuIndex==MENU_NETWORK_LOCAL_INDEX)     return MENU_NETWORK_LOCAL_TYPE;
	if(menuIndex==MENU_NETWORK_INTERNET_INDEX)  return MENU_NETWORK_INTERNET_TYPE;
	
	return -1;
}

int getMenuLength(int menuIndex)
{
	if(menuIndex==MENU_MAIN_INDEX)              return MENU_MAIN_LENGTH;
	if(menuIndex==MENU_LOCAL_INDEX)             return MENU_LOCAL_LENGTH;
	if(menuIndex==MENU_NETWORK_INDEX)           return MENU_NETWORK_LENGTH;
	if(menuIndex==MENU_OPTIONS_INDEX)           return MENU_OPTIONS_LENGTH;
	if(menuIndex==MENU_INSTRUCTIONS_INDEX)      return MENU_INSTRUCTIONS_LENGTH;
	if(menuIndex==MENU_ABOUT_INDEX)             return MENU_ABOUT_LENGTH;
	if(menuIndex==MENU_NETWORK_NEW_GAME_INDEX)  return MENU_NETWORK_NEW_GAME_LENGTH;
	if(menuIndex==MENU_NETWORK_JOIN_GAME_INDEX)
	{
		int num = 1+numOpenGames;
		if(num<1) num=1; // minimum of one game shown
		if(num>6) num=6; // maximum of 8 games shown
		return num;
	}
	if(menuIndex==MENU_NETWORK_LOBBY_INDEX)
	{
		if(game[LOCAL].maxPlayers==8) // no title with 8 players
			return game[LOCAL].maxPlayers;
		else // 1 for title
			return 1+game[LOCAL].maxPlayers;
	}
	if(menuIndex==MENU_QUIT_INDEX)              return MENU_QUIT_LENGTH;
	if(menuIndex==MENU_PLAYERS_INDEX)           return 1+hotseatPlayers;
	if(menuIndex==MENU_NETWORK_LOCAL_INDEX)     return MENU_NETWORK_LOCAL_LENGTH;
	if(menuIndex==MENU_NETWORK_INTERNET_INDEX)  return MENU_NETWORK_INTERNET_LENGTH;
	
	return -1;
}

char* getMenuRow(int menuIndex, int rowIndex)
{
	int i,j;
	if(menuIndex==MENU_MAIN_INDEX)              return MENU_MAIN_TEXT[rowIndex];
	if(menuIndex==MENU_LOCAL_INDEX)             return MENU_LOCAL_TEXT[rowIndex];
	if(menuIndex==MENU_NETWORK_INDEX)           return MENU_NETWORK_TEXT[rowIndex];
	if(menuIndex==MENU_OPTIONS_INDEX)
	{
		if(getMenuAction(menuIndex, rowIndex)==MENU_ACTION_EDIT_KEYBOARD) // add edit text to the end of the row
		{
			char *str = (char*)malloc(128); // oe-noes, mem leak!
			
			if(getMenuData(menuIndex, rowIndex)==MENU_EDIT_PLAYER_NAME)
				sprintf(str, "%s%s", MENU_OPTIONS_TEXT[rowIndex], playerName[LOCAL]);
			else if(getMenuData(menuIndex, rowIndex)==MENU_EDIT_SERVER_ADDRESS)
				sprintf(str, "%s%s", MENU_OPTIONS_TEXT[rowIndex], serverAddress);
			else // if(getMenuData(menuIndex, rowIndex)==MENU_EDIT_SERVER_PORT)
				sprintf(str, "%s%d", MENU_OPTIONS_TEXT[rowIndex], serverPort);

			return str;
		}
		else
			return MENU_OPTIONS_TEXT[rowIndex];
	}
	if(menuIndex==MENU_INSTRUCTIONS_INDEX)      return MENU_INSTRUCTIONS_TEXT[rowIndex];
	if(menuIndex==MENU_ABOUT_INDEX)             return MENU_ABOUT_TEXT[rowIndex];
	if(menuIndex==MENU_NETWORK_NEW_GAME_INDEX)  return MENU_NETWORK_NEW_GAME_TEXT[rowIndex];
	if(menuIndex==MENU_NETWORK_JOIN_GAME_INDEX)
	{
		if(rowIndex>0)
			return openGameNames[rowIndex-1];
		else
			return MENU_NETWORK_JOIN_GAME_TEXT[rowIndex];
	}
	if(menuIndex==MENU_NETWORK_LOBBY_INDEX)
	{
		int index = 1;
		if(game[LOCAL].maxPlayers==8) // don't show title if we're waiting for 8 players
			index = 0;
		if(rowIndex>=index)
		{
			j=0;
			for(i=0;i<MAX_PLAYERS;i++)
			{
				if(game[LOCAL].playerStatus[i]==PLAYER_STATUS_READY||game[LOCAL].playerStatus[i]==PLAYER_STATUS_WAITING)
				{
					if(j==rowIndex-index) // &&j<game[LOCAL].numPlayers)
						return game[LOCAL].playerName[i];
					j++;
				}
			}
			if(rowIndex-index>=game[LOCAL].numPlayers)
				return "waiting player";
			else	
				return "fetching names";
		}
		else
			return MENU_NETWORK_LOBBY_TEXT[rowIndex];
	}
	if(menuIndex==MENU_QUIT_INDEX)              return MENU_QUIT_TEXT[rowIndex];
	if(menuIndex==MENU_PLAYERS_INDEX)
	{
		if(getMenuAction(menuIndex, rowIndex)==MENU_ACTION_EDIT_KEYBOARD) // add edit text to the end of the row
		{
			char *str = (char*)malloc(128); // oe-noes, mem leak!
			
			if(getMenuData(menuIndex, rowIndex)==MENU_EDIT_PLAYER_NAME)
				sprintf(str, "%s%s", MENU_PLAYERS_TEXT[rowIndex], playerName[rowIndex-1]);
			
			return str;
		}
		else
			return MENU_PLAYERS_TEXT[rowIndex];
	}
	if(menuIndex==MENU_NETWORK_LOCAL_INDEX)     return MENU_NETWORK_LOCAL_TEXT[rowIndex];
	if(menuIndex==MENU_NETWORK_INTERNET_INDEX)  return MENU_NETWORK_INTERNET_TEXT[rowIndex];

	logPrintf("!!! getMenuRow() leaks! -> return NULL\n");
	return NULL;
}

int getMenuAction(int menuIndex, int rowIndex)
{
	if(menuIndex==MENU_MAIN_INDEX)              return MENU_MAIN_ACTION[rowIndex];
	if(menuIndex==MENU_LOCAL_INDEX)             return MENU_LOCAL_ACTION[rowIndex];
	if(menuIndex==MENU_NETWORK_INDEX)           return MENU_NETWORK_ACTION[rowIndex];
	if(menuIndex==MENU_OPTIONS_INDEX)           return MENU_OPTIONS_ACTION[rowIndex];
	if(menuIndex==MENU_INSTRUCTIONS_INDEX)      return MENU_INSTRUCTIONS_ACTION[rowIndex];
	if(menuIndex==MENU_ABOUT_INDEX)             return MENU_ABOUT_ACTION[rowIndex];
	if(menuIndex==MENU_NETWORK_NEW_GAME_INDEX)  return MENU_NETWORK_NEW_GAME_ACTION[rowIndex];
	if(menuIndex==MENU_NETWORK_JOIN_GAME_INDEX) return MENU_NETWORK_JOIN_GAME_ACTION[rowIndex];
	if(menuIndex==MENU_NETWORK_LOBBY_INDEX)     return MENU_NETWORK_LOBBY_ACTION[rowIndex];
	if(menuIndex==MENU_QUIT_INDEX)              return MENU_QUIT_ACTION[rowIndex];
	if(menuIndex==MENU_PLAYERS_INDEX)           return MENU_PLAYERS_ACTION[rowIndex];
	if(menuIndex==MENU_NETWORK_LOCAL_INDEX)     return MENU_NETWORK_LOCAL_ACTION[rowIndex];
	if(menuIndex==MENU_NETWORK_INTERNET_INDEX)  return MENU_NETWORK_INTERNET_ACTION[rowIndex];

	return -1;
}

int getMenuData(int menuIndex, int rowIndex)
{
	if(menuIndex==MENU_MAIN_INDEX)              return MENU_MAIN_DATA[rowIndex];
	if(menuIndex==MENU_LOCAL_INDEX)             return MENU_LOCAL_DATA[rowIndex];
	if(menuIndex==MENU_NETWORK_INDEX)           return MENU_NETWORK_DATA[rowIndex];
	if(menuIndex==MENU_OPTIONS_INDEX)           return MENU_OPTIONS_DATA[rowIndex];
	if(menuIndex==MENU_INSTRUCTIONS_INDEX)      return MENU_INSTRUCTIONS_DATA[rowIndex];
	if(menuIndex==MENU_ABOUT_INDEX)             return MENU_ABOUT_DATA[rowIndex];
	if(menuIndex==MENU_NETWORK_NEW_GAME_INDEX)  return MENU_NETWORK_NEW_GAME_DATA[rowIndex];
	if(menuIndex==MENU_NETWORK_JOIN_GAME_INDEX) return MENU_NETWORK_JOIN_GAME_DATA[rowIndex];
	if(menuIndex==MENU_NETWORK_LOBBY_INDEX)     return MENU_NETWORK_LOBBY_DATA[rowIndex];
	if(menuIndex==MENU_QUIT_INDEX)              return MENU_QUIT_DATA[rowIndex];
	if(menuIndex==MENU_PLAYERS_INDEX)           return MENU_PLAYERS_DATA[rowIndex];
	if(menuIndex==MENU_NETWORK_LOCAL_INDEX)     return MENU_NETWORK_LOCAL_DATA[rowIndex];
	if(menuIndex==MENU_NETWORK_INTERNET_INDEX)  return MENU_NETWORK_INTERNET_DATA[rowIndex];
	
	return -1;
}

int getMenuAttribute(int menuIndex, int rowIndex)
{
	if(menuIndex==MENU_MAIN_INDEX)              return MENU_MAIN_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_LOCAL_INDEX)             return MENU_LOCAL_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_NETWORK_INDEX)           return MENU_NETWORK_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_OPTIONS_INDEX)           return MENU_OPTIONS_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_INSTRUCTIONS_INDEX)      return MENU_INSTRUCTIONS_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_ABOUT_INDEX)             return MENU_ABOUT_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_NETWORK_NEW_GAME_INDEX)  return MENU_NETWORK_NEW_GAME_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_NETWORK_JOIN_GAME_INDEX) return MENU_NETWORK_JOIN_GAME_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_NETWORK_LOBBY_INDEX)     return MENU_NETWORK_LOBBY_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_QUIT_INDEX)              return MENU_QUIT_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_PLAYERS_INDEX)           return MENU_PLAYERS_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_NETWORK_LOCAL_INDEX)     return MENU_NETWORK_LOCAL_ATTRIBUTES[rowIndex];
	if(menuIndex==MENU_NETWORK_INTERNET_INDEX)  return MENU_NETWORK_INTERNET_ATTRIBUTES[rowIndex];
	
	return -1;
}

// TODO: very very hardcoded atm. - only using wifi icons in this game, so doesn't really matter
int getMenuImageIndex(int menuIndex, int rowIndex)
{
	for(int i=0;i<(int)(sizeof(MENU_IMAGES)/sizeof(*MENU_IMAGES));i+=6)
	{
		if(MENU_IMAGES[i]==getMenuData(menuIndex, rowIndex)) // found an image
		{
			return i;
		}
	}
	return -1;
}

void menuLogic()
{
	int i;

	if(pointerMoved)
	{
		for(i=0;i<16;i++) menuSliderDown[i] = false;
		lastHoverRight = menuHoverRight;
		lastHoverLeft = menuHoverLeft;
		menuHoverRight = false;
		menuHoverLeft = false;
		if(currentMenu==MENU_QUIT_INDEX)
		{
			if(pointerX>=X_CEN-22-(stringWidth(FONT_TRUETYPE, "Yes")>>1)&&
			   pointerX<X_CEN-22+(stringWidth(FONT_TRUETYPE, "Yes")>>1)&&
			   pointerY>=Y_CEN+4&&pointerY<Y_CEN+4+FONT_TRUETYPE_HEIGHT)
			{
				menuHoverLeft = true;
				menuHoverRight = false;
			}
			else if(pointerX>=X_CEN+22-(stringWidth(FONT_TRUETYPE, "No")>>1)&&
			        pointerX<X_CEN+22+(stringWidth(FONT_TRUETYPE, "No")>>1)&&
			        pointerY>=Y_CEN+4&&pointerY<Y_CEN+4+FONT_TRUETYPE_HEIGHT)
			{
				menuHoverLeft = false;
				menuHoverRight = true;
			}
			else
			{
				menuHoverLeft = false;
				menuHoverRight = false;
			}
		}
		else
		{
			if(pointerY>=Y_RES-1-IMAGE_ARROWS_HEIGHT&&pointerY<Y_RES-1)
			{
				if(pointerX>=X_RES-1-(IMAGE_ARROWS_WIDTH>>2)&&pointerX<X_RES-1) // hover right
				{
					menuHoverRight = true;
				  menuUpdated = true;
				}
				if(pointerX>=1&&pointerX<1+(IMAGE_ARROWS_WIDTH>>2)) // hover left
				{
					menuHoverLeft = true;
				  menuUpdated = true;
				}
			}
		}
		if(lastHoverRight!=menuHoverRight||lastHoverLeft!=menuHoverLeft)
			menuUpdated = true;			
		else if(getMenuType(currentMenu)==MENU_TYPE_NORMAL||currentMenu==MENU_NETWORK_JOIN_GAME_INDEX)
		{
			int add=0;
			if(getMenuType(currentMenu)!=MENU_TYPE_NORMAL||(currentMenu==MENU_PLAYERS_INDEX&&hotseatPlayers==8)) add=1;

			for(int i=1-add; i<getMenuLength(currentMenu); i++)
			{
				if(pointerY>=getMenuAttribute(currentMenu, MENU_ATTRIBUTE_Y)+(i+1)*getMenuAttribute(currentMenu, MENU_ATTRIBUTE_HEIGHT)&&
				   pointerY<getMenuAttribute(currentMenu, MENU_ATTRIBUTE_Y)+(i+1)*getMenuAttribute(currentMenu, MENU_ATTRIBUTE_HEIGHT)+
				   getMenuAttribute(currentMenu, MENU_ATTRIBUTE_FONT_HEIGHT)&&
				   pointerX>=getMenuAttribute(currentMenu, MENU_ATTRIBUTE_X)-
				   getAlignX(getMenuAttribute(currentMenu, MENU_ATTRIBUTE_ALIGN), stringWidth(FONT_TRUETYPE, getMenuRow(currentMenu, i)))&&
				   pointerX<getMenuAttribute(currentMenu, MENU_ATTRIBUTE_X)+
				   getAlignX(getInvertAlign(getMenuAttribute(currentMenu, MENU_ATTRIBUTE_ALIGN)), stringWidth(FONT_TRUETYPE, getMenuRow(currentMenu, i))+
				   (getMenuAction(currentMenu, i)==MENU_ACTION_EDIT_SLIDER?MENU_SLIDER_WIDTH*3:0) )) {
					  #ifdef DISABLE_NETWORK_GAME
					  if(currentMenu!=MENU_MAIN_INDEX||i!=2)
					 	{
				  	#endif
					  	currentMenuIndex = i+add;
						  menuUpdated = true;
					  #ifdef DISABLE_NETWORK_GAME
				  	}
				  	#endif
				}
				if(getMenuAction(currentMenu, i)==MENU_ACTION_EDIT_SLIDER) // check if we're hovering the slider
				{
					int x = getMenuAttribute(currentMenu, MENU_ATTRIBUTE_X)+stringWidth(FONT_TRUETYPE, getMenuRow(currentMenu, i));
					int y = getMenuAttribute(currentMenu, MENU_ATTRIBUTE_Y)+(i+1)*getMenuAttribute(currentMenu, MENU_ATTRIBUTE_HEIGHT);
					if(pointerX>=x&&pointerX<x+MENU_SLIDER_WIDTH&&pointerY>=y&&pointerY<y+MENU_SLIDER_HEIGHT)
						menuSliderDown[i*2] = true;
					else if (pointerX>=x+MENU_SLIDER_WIDTH*2&&pointerX<x+MENU_SLIDER_WIDTH*3&&pointerY>=y&&pointerY<y+MENU_SLIDER_HEIGHT)
						menuSliderDown[i*2+1] = true;
				}
			}
		}
	}
	if(currentMenu==MENU_MAIN_INDEX&&networkConnected)
	{
		uninitNetworkConnection();
	}
	if(currentMenu==MENU_NETWORK_LOBBY_INDEX||currentMenu==MENU_NETWORK_JOIN_GAME_INDEX)
	{
	  menuUpdated = true;
	  
		if((!networkInitialized||!networkConnected)&&networkStatus!=NETWORK_STATUS_WRONG_VERSION)
		{
			if(!initNetworkConnection())
			{
				logPrintf("menu network status failed\n");
				networkStatus = NETWORK_STATUS_CONNECTION_FAILED;
			}
			else
			{
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "!!! N:INIT");
				#endif				
			}
		}

	  if(networkInitialized&&networkConnected)
		{
			if(currentMenu==MENU_NETWORK_LOBBY_INDEX&&!networkRequestSent) // create a new game
			{
				networkRequestSent = true;
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> CREATE");
				#endif
		    memset(sendBuffer, 0, NETWORK_MAX_DATA);
		    sendBuffer[0] = MESSAGE_CREATE_GAME;
				sendBuffer[1] = minPlayers; // min players
				game[LOCAL].minPlayers = minPlayers;
				sendBuffer[2] = maxPlayers; // max players
				game[LOCAL].maxPlayers = maxPlayers;
				game[LOCAL].numPlayers = 0;
				game[LOCAL].gameStatus = GAME_STATUS_WAITING_PLAYERS;
	      do // we have certain number of evil gamefields, please skip those
	      {
					sendBuffer[3] = rand()%NUM_FIELDS; // field id
				} while(sendBuffer[3]==21||sendBuffer[3]==26||sendBuffer[3]==27||sendBuffer[3]==38||sendBuffer[3]==39);

				// DEBUG test certain gamefields for the network game:
//				sendBuffer[3] = 17;

				for(i=0;i<MAX_PLAYERS;i++)
					game[LOCAL].playerStatus[i]=PLAYER_STATUS_UNCONFIRMED;
				sendBuffer[4] = favouriteColor;
				for(i=0;i<MAX_NAME_LENGTH;i++)
				{
					sendBuffer[5+i] = playerName[LOCAL][i];
					game[LOCAL].playerName[0][i] = playerName[LOCAL][i];
				}

				#ifdef LOBBY
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> L:ROOM");	
					#endif
					LOBBY_CreateRoom("DICEWARS", maxPlayers, LOBBY_GAMECODE, requiredVersionShort);
					roomOwner = LOBBY_GetUserByID(USERID_MYSELF);				
				#endif
				sendClientData(sendBuffer);
				menuUpperDrawn = false;
				playerJoined = true;
			}
			else if(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX) // list current games
			{
				if(!networkRequestSent)
					currentMenuIndex = 1;
				if(networkMode==NETWORK_MODE_LOBBY||!networkRequestSent)
					clientListOpenGames();
	    }
		}
	}
	else if(currentMenu==MENU_NETWORK_NEW_GAME_INDEX||currentMenu==MENU_NETWORK_JOIN_GAME_INDEX)
	{
		readyToStart = false;
	}
}

void menuKeyPressed(int key)
{
	logPrintf("MENU KEY PRESSED at %d: %d\n", currentMenu, key);
	menuUpdated = true;
	if(key==KEY_SELECT)
	{
		if(currentMenu==MENU_QUIT_INDEX) // reboot
		{
			playSound(SOUND_SELECT_INDEX);
			reboot();
		}
		else if(getMenuType(currentMenu)==MENU_TYPE_GAME)
		{
			playSound(SOUND_SELECT_INDEX);
			if(currentMenu==MENU_NETWORK_LOBBY_INDEX)
			{
				logPrintf("Client pressed ready to start 1\n");
				clientReadyToStart();
			}
			else if(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX&&numOpenGames>0)
			{
				logPrintf("Client joined game %d\n", openGameIds[currentMenuIndex-1]);
				clientJoinGame(openGameIds[currentMenuIndex-1]);
				logPrintf("Client joined game %d ready\n", openGameIds[currentMenuIndex-1]);
			}
		}
		else
		{
			if(getMenuAction(currentMenu, currentMenuIndex)==MENU_ACTION_CHANGE_MENU)
			{
				playSound(SOUND_SELECT_INDEX);
				parentMenu[currentMenuLevel] = currentMenu;
				parentMenuIndex[currentMenuLevel] = currentMenuIndex;
				currentMenu = getMenuData(currentMenu, currentMenuIndex);
				currentMenuLevel++;
				if(currentMenu==MENU_NETWORK_INDEX)
				{
					#ifdef LOBBY
						networkMode=NETWORK_MODE_LOBBY;
					#else
						networkMode=NETWORK_MODE_TCP;
					#endif
					if(networkStatus==NETWORK_STATUS_CONNECTION_FAILED||networkStatus==NETWORK_STATUS_CONNECTION_LOST)
					{
						networkStatus = NETWORK_STATUS_CONNECTING;
						pingCounter=pongCounter=0;

						if(networkInitialized)
						{
							networkInitCounter = 5;
							networkConnected = false;
						}
						else
							networkInitCounter = 0;
					}
				}
				else if(currentMenu==MENU_LOCAL_INDEX)
				{
					gameStartProcessed = false;
					networkMode=NETWORK_MODE_LOCAL;
					if(game[LOCAL].numPlayers<2)
						game[LOCAL].numPlayers = 7;
				}
				else if(currentMenu==MENU_NETWORK_NEW_GAME_INDEX||currentMenu==MENU_NETWORK_JOIN_GAME_INDEX)
					menuUpperDrawn = false;
				currentMenuIndex = 1;
				logPrintf("Current menu now: %d\n", currentMenu);
				return;
			}
			else if(getMenuAction(currentMenu, currentMenuIndex)==MENU_ACTION_CHANGE_STATE)
			{
				playSound(SOUND_SELECT_INDEX);
				gameState = getMenuData(currentMenu, currentMenuIndex);
				logPrintf("HERE changed game state to: %d\n", gameState);
				return;
			}
			else if(getMenuAction(currentMenu, currentMenuIndex)==MENU_ACTION_EDIT_KEYBOARD)
			{
				playSound(SOUND_SELECT_INDEX);
				logPrintf("go to keyboard edit mode.\n");
				memset(kbdString, 0, 255);
				kbdUpdated = true;
				if(getMenuData(currentMenu, currentMenuIndex)==MENU_EDIT_PLAYER_NAME)
					kbdPlayerIndex = currentMenuIndex-1;

				for(int i=0;i<255;i++)
				{
					if(getMenuData(currentMenu, currentMenuIndex)==MENU_EDIT_PLAYER_NAME&&i<(int)strlen(playerName[kbdPlayerIndex]))
					{
						kbdString[i] = playerName[kbdPlayerIndex][i];
					}
					else if(getMenuData(currentMenu, currentMenuIndex)==MENU_EDIT_SERVER_ADDRESS&&i<(int)strlen(serverAddress))
					{
						kbdString[i] = serverAddress[i];
					}
					else if(getMenuData(currentMenu, currentMenuIndex)==MENU_EDIT_SERVER_PORT&&i<5)
					{
						char *sPort = int2str(serverPort);
						if(sPort[i]!=0)
						{
							kbdString[i] = sPort[i];
						}
						else
						{
							kbdString[i] = 0;
							kbdIndex = i;
							break;
						}
					}
					else
					{
						kbdString[i] = 0;
						kbdIndex = i;
						break;
					}
				}
				prevState = GAME_STATE_MENU;
				gameState = GAME_STATE_KEYBOARD;
			}
		}		
	}
//	if((key==KEY_BACK||key==KEY_LEFT)&&currentMenu!=MENU_MAIN_INDEX)
	if(key==KEY_BACK&&currentMenu!=MENU_MAIN_INDEX)
	{
		playSound(SOUND_RESELECT_INDEX);
		if(currentMenu==MENU_NETWORK_LOBBY_INDEX||currentMenu==MENU_NETWORK_JOIN_GAME_INDEX||currentMenu==MENU_NETWORK_NEW_GAME_INDEX)
			menuUpperDrawn = false;
		if(currentMenu==MENU_NETWORK_LOBBY_INDEX||(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX&&playerJoined))
			clientQuitGame();
		else if(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX) networkRequestSent = false;
		currentMenuLevel--;
		currentMenu = parentMenu[currentMenuLevel];
		currentMenuIndex = parentMenuIndex[currentMenuLevel];
	}
	else if((key==KEY_BACK||key==KEY_ESCAPE)&&currentMenu==MENU_MAIN_INDEX) // player wants to quit?
	{
		playSound(SOUND_RESELECT_INDEX);
		currentMenu = MENU_QUIT_INDEX;
		parentMenu[currentMenuLevel] = MENU_MAIN_INDEX;
		parentMenuIndex[currentMenuLevel] = currentMenuIndex;
		currentMenuLevel++;
		menuUpdated = true;
	}
	if(key==KEY_UP)
	{
		playSound(SOUND_RESELECT_INDEX);
		if(--currentMenuIndex<1)
			currentMenuIndex = getMenuLength(currentMenu)-1;
		#ifdef DISABLE_NETWORK_GAME
			if(currentMenu==MENU_MAIN_INDEX&&currentMenuIndex==2)
				if(--currentMenuIndex<1)
					currentMenuIndex = getMenuLength(currentMenu)-1;
		#endif
	}
	if(key==KEY_DOWN)
	{
		playSound(SOUND_RESELECT_INDEX);
//		playSound(SOUND_TEST_INDEX);
		if(++currentMenuIndex>=getMenuLength(currentMenu))
			currentMenuIndex = 1;
		#ifdef DISABLE_NETWORK_GAME
			if(currentMenu==MENU_MAIN_INDEX&&currentMenuIndex==2)
				if(++currentMenuIndex>=getMenuLength(currentMenu))
					currentMenuIndex = 1;
		#endif
	}
	if(key==KEY_RIGHT)
	{
		if(getMenuAction(currentMenu, currentMenuIndex)==MENU_ACTION_EDIT_SLIDER)
		{
			playSound(SOUND_RESELECT_INDEX);
			switch(getMenuData(currentMenu, currentMenuIndex))
			{
				case MENU_EDIT_NUM_PLAYERS:
				{
					if(++game[LOCAL].numPlayers>MAX_PLAYERS) game[LOCAL].numPlayers=2;
					logPrintf("numPlayers now: %d\n", game[LOCAL].numPlayers);
					if(game[LOCAL].numPlayers<hotseatPlayers) hotseatPlayers = game[LOCAL].numPlayers;
				}
				break;

				case MENU_EDIT_HOTSEAT_PLAYERS:
				{
					if(++hotseatPlayers>MAX_PLAYERS) hotseatPlayers=1;
					logPrintf("hotseatPlayers now: %d\n", hotseatPlayers);
					if(game[LOCAL].numPlayers<hotseatPlayers) game[LOCAL].numPlayers = hotseatPlayers;
				}
				break;

				case MENU_EDIT_MIN_PLAYERS:
				{
					if(++minPlayers>MAX_PLAYERS) minPlayers=2;
					logPrintf("minPlayers now: %d\n", minPlayers);
					if(maxPlayers<minPlayers) maxPlayers = minPlayers;
				}
				break;

				case MENU_EDIT_MAX_PLAYERS:
				{
					if(++maxPlayers>MAX_PLAYERS) maxPlayers=2;
					logPrintf("maxPlayers now: %d\n", maxPlayers);
					if(maxPlayers<minPlayers) minPlayers = maxPlayers;
				}
				break;

				case MENU_EDIT_FAVOURITE_COLOR:
				{
					if(++favouriteColor>=MAX_PLAYERS) favouriteColor=0;
					logPrintf("favourite color now: %d\n", favouriteColor);
				}
				break;
			}
		}
	}
	if(key==KEY_LEFT)
	{
		if(getMenuAction(currentMenu, currentMenuIndex)==MENU_ACTION_EDIT_SLIDER)
		{
			playSound(SOUND_RESELECT_INDEX);
			switch(getMenuData(currentMenu, currentMenuIndex))
			{
				case MENU_EDIT_NUM_PLAYERS:
				{
					if(--game[LOCAL].numPlayers<2) game[LOCAL].numPlayers=MAX_PLAYERS;
					logPrintf("numPlayers now: %d\n", game[LOCAL].numPlayers);
					if(game[LOCAL].numPlayers<hotseatPlayers) hotseatPlayers = game[LOCAL].numPlayers;
				}
				break;

				case MENU_EDIT_HOTSEAT_PLAYERS:
				{
					if(--hotseatPlayers<1) hotseatPlayers=MAX_PLAYERS;
					logPrintf("hotseatPlayers now: %d\n", hotseatPlayers);
					if(game[LOCAL].numPlayers<hotseatPlayers) game[LOCAL].numPlayers = hotseatPlayers;
				}
				break;

				case MENU_EDIT_MIN_PLAYERS:
				{
					if(--minPlayers<2) minPlayers=MAX_PLAYERS;
					logPrintf("minPlayers now: %d\n", minPlayers);
					if(maxPlayers<minPlayers) maxPlayers = minPlayers;
				}
				break;

				case MENU_EDIT_MAX_PLAYERS:
				{
					if(--maxPlayers<2) maxPlayers=MAX_PLAYERS;
					logPrintf("maxPlayers now: %d\n", maxPlayers);
					if(maxPlayers<minPlayers) minPlayers = maxPlayers;
				}
				break;

				case MENU_EDIT_FAVOURITE_COLOR:
				{
					if(--favouriteColor<0) favouriteColor=MAX_PLAYERS-1;
					logPrintf("favourite color now: %d\n", favouriteColor);
				}
				break;
			}
		}
	}
}

void menuKeyReleased(int key)
{
	menuUpdated = true;
	if(key==KEY_POINTER)
	{
		if(currentMenu==MENU_QUIT_INDEX)
		{
			if(menuHoverLeft) // quit game
			{
				playSound(SOUND_SELECT_INDEX);
				reboot();
			}
			else // back to the main menu
			{
				playSound(SOUND_RESELECT_INDEX);
				currentMenuLevel--;
				currentMenu = parentMenu[currentMenuLevel];
				currentMenuIndex = parentMenuIndex[currentMenuLevel];
			}
		}
		else if(pointerY>=Y_RES-IMAGE_ARROWS_HEIGHT-1&&pointerY<Y_RES-1&&(menuHoverLeft||menuHoverRight))
		{
			if(menuHoverLeft) // back
			{
				playSound(SOUND_RESELECT_INDEX);
				if(currentMenu==MENU_NETWORK_LOBBY_INDEX||currentMenu==MENU_NETWORK_JOIN_GAME_INDEX||currentMenu==MENU_NETWORK_NEW_GAME_INDEX)
					menuUpperDrawn = false;
				if(currentMenu==MENU_NETWORK_LOBBY_INDEX||(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX&&playerJoined))
					clientQuitGame();
				else if(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX) networkRequestSent = false;
				currentMenuLevel--;
				currentMenu = parentMenu[currentMenuLevel];
				currentMenuIndex = parentMenuIndex[currentMenuLevel];
				menuHoverLeft = false;
				lastHoverLeft = menuHoverLeft;
			}
			else if(getMenuType(currentMenu)==MENU_TYPE_GAME)
			{
				if(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX&&numOpenGames>0)
				{
					if(menuHoverRight)
					{
						playSound(SOUND_SELECT_INDEX);
						clientJoinGame(openGameIds[currentMenuIndex-1]);
						menuHoverRight = false;
						lastHoverRight = menuHoverRight;
					}
				}
				else
				{
					if(menuHoverRight)
					{
						playSound(SOUND_SELECT_INDEX);
						if(currentMenu==MENU_NETWORK_LOBBY_INDEX) 
						{
							logPrintf("Client pressed ready to start 2\n");
							clientReadyToStart();
						}
						menuHoverRight = false;
						lastHoverRight = menuHoverRight;

//						gameState = getMenuData(currentMenu, currentMenuIndex);
//						printf("changed game state to: %d\n", gameState);
					}
				}
			}
		}
		else
		{
			for(int i=0;i<16;i++)
			{
				if(menuSliderDown[i])
				{
					logPrintf("slider pressed!\n");
					menuKeyPressed(i%2?KEY_RIGHT:KEY_LEFT);
					return;
				}
			}
			logPrintf("pointer released at %d.%d\n", pointerX, pointerY);
			int add=1;
			if(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX||(currentMenu==MENU_PLAYERS_INDEX&&hotseatPlayers==8)) add=0;
			for(int i=add; i<getMenuLength(currentMenu); i++)
			{
				if(pointerY>=getMenuAttribute(currentMenu, MENU_ATTRIBUTE_Y)+(i+1)*getMenuAttribute(currentMenu, MENU_ATTRIBUTE_HEIGHT)&&
				   pointerY<getMenuAttribute(currentMenu, MENU_ATTRIBUTE_Y)+(i+1)*getMenuAttribute(currentMenu, MENU_ATTRIBUTE_HEIGHT)+
				   getMenuAttribute(currentMenu, MENU_ATTRIBUTE_FONT_HEIGHT)&&
				   pointerX>=getMenuAttribute(currentMenu, MENU_ATTRIBUTE_X)-
				   getAlignX(getMenuAttribute(currentMenu, MENU_ATTRIBUTE_ALIGN), stringWidth(FONT_TRUETYPE, getMenuRow(currentMenu, i)))&&
				   pointerX<getMenuAttribute(currentMenu, MENU_ATTRIBUTE_X)+
				   getAlignX(getInvertAlign(getMenuAttribute(currentMenu, MENU_ATTRIBUTE_ALIGN)), stringWidth(FONT_TRUETYPE, getMenuRow(currentMenu, i))))
				{
					if(getMenuAction(currentMenu, i)==MENU_ACTION_CHANGE_MENU)
					{
					  #ifdef DISABLE_NETWORK_GAME
					  if(currentMenu!=MENU_MAIN_INDEX||i!=2)
					  {
					  #endif
							playSound(SOUND_SELECT_INDEX);
							parentMenu[currentMenuLevel] = currentMenu;
							parentMenuIndex[currentMenuLevel] = currentMenuIndex;
							currentMenuLevel++;
							currentMenu = getMenuData(currentMenu, i);
							logPrintf("current menu now: %d\n", currentMenu);
					  #ifdef DISABLE_NETWORK_GAME
						}
					  #endif
						if(currentMenu==MENU_NETWORK_INDEX)
						{
							#ifdef LOBBY
								networkMode=NETWORK_MODE_LOBBY;
							#else
								networkMode=NETWORK_MODE_TCP;
							#endif
							if(networkStatus==NETWORK_STATUS_CONNECTION_FAILED||networkStatus==NETWORK_STATUS_CONNECTION_LOST)
							{
								networkStatus = NETWORK_STATUS_CONNECTING;
								pingCounter=pongCounter=0;
		
								if(networkInitialized)
								{
									networkInitCounter = 5;
									networkConnected = false;
								}
								else
								{
									if(networkInitCounter>2) networkInitCounter = 2;
								}
							}
						}
						else if(currentMenu==MENU_LOCAL_INDEX)
						{
							gameStartProcessed = false;
							networkMode=NETWORK_MODE_LOCAL;
							if(game[LOCAL].numPlayers==0)
								game[LOCAL].numPlayers = 7;
						}
						currentMenuIndex = 1;
						return;
					}
					else if(getMenuAction(currentMenu, i)==MENU_ACTION_CHANGE_STATE)
					{
						playSound(SOUND_SELECT_INDEX);
						gameState = getMenuData(currentMenu, i);
						logPrintf("changed game state to: %d\n", gameState);
						return;
					}
					else if(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX&&numOpenGames>0)
					{
						playSound(SOUND_SELECT_INDEX);
						clientJoinGame(openGameIds[currentMenuIndex-1]);
					}
					else if(getMenuAction(currentMenu, currentMenuIndex)==MENU_ACTION_EDIT_KEYBOARD)
					{
						playSound(SOUND_SELECT_INDEX);
						logPrintf("go to keyboard edit mode.\n");
						memset(kbdString, 0, 255);
						kbdUpdated = true;
						if(getMenuData(currentMenu, currentMenuIndex)==MENU_EDIT_PLAYER_NAME)
							kbdPlayerIndex = currentMenuIndex-1;

						for(int i=0;i<255;i++)
						{
							if(getMenuData(currentMenu, currentMenuIndex)==MENU_EDIT_PLAYER_NAME&&i<(int)strlen(playerName[kbdPlayerIndex]))
							{
								kbdString[i] = playerName[kbdPlayerIndex][i];
							}
							else if(getMenuData(currentMenu, currentMenuIndex)==MENU_EDIT_SERVER_ADDRESS&&i<(int)strlen(serverAddress))
							{
								kbdString[i] = serverAddress[i];
							}
							else if(getMenuData(currentMenu, currentMenuIndex)==MENU_EDIT_SERVER_PORT&&i<5)
							{
								char *sPort = int2str(serverPort);
								if(sPort[i]!=0)
								{
									kbdString[i] = sPort[i];
								}
								else
								{
									kbdString[i] = 0;
									kbdIndex = i;
									break;
								}
							}
							else
							{
								kbdString[i] = 0;
								kbdIndex = i;
								break;
							}
						}
						prevState = GAME_STATE_MENU;
						gameState = GAME_STATE_KEYBOARD;
						return;
					}
					else
					{
						logPrintf("nothing to do?\n");
					}
				}
			}
		}
	}
}

boolean menuPlayerReady(int index)
{
	int j=0;
	for(int i=0;i<MAX_PLAYERS;i++)
	{
		if(game[LOCAL].playerStatus[i]!=PLAYER_STATUS_UNCONFIRMED)
		{
			if(j==index)
			{
				if(game[LOCAL].playerStatus[i]==PLAYER_STATUS_READY)
				{
					return true; 
				}
				else
				{
					return false;
				}
			}			
			j++;
		}
	}
	
	return false;
}

void drawSlider(int font, int value, int x, int y, int align, int index)
{
	int tempColor = getColor();
	if(menuSliderDown[index*2]) setColor((tempColor==MENU_COLOR_TEXT?MENU_COLOR_ACTIVE:MENU_COLOR_TEXT));
	drawString(FONT_TRUETYPE, "<", x, y, align);
	setColor(tempColor);
	if(value==-1) // color selection
	{
		setColor(playerColor[favouriteColor]);
		fillHexagon(7+x+MENU_SLIDER_WIDTH, 7+y, 6, 5, true);
		setColor(0);
		drawHexagon(7+x+MENU_SLIDER_WIDTH, 7+y, 6, 5, true);
		setColor(tempColor);
		if(menuSliderDown[index*2+1]) setColor((tempColor==MENU_COLOR_TEXT?MENU_COLOR_ACTIVE:MENU_COLOR_TEXT));
		drawString(FONT_TRUETYPE, ">", x+MENU_SLIDER_WIDTH+HEXAGON_WIDTH-1, y, align);
	}
	else
	{
		char str[8]; sprintf(str, "%d", value);
		drawString(FONT_TRUETYPE, str, x+MENU_SLIDER_WIDTH, y, align);
		if(menuSliderDown[index*2+1]) setColor((tempColor==MENU_COLOR_TEXT?MENU_COLOR_ACTIVE:MENU_COLOR_TEXT));
		drawString(FONT_TRUETYPE, ">", x+MENU_SLIDER_WIDTH+stringWidth(FONT_TRUETYPE, str)+2, y, align);
	}
	setColor(tempColor);
}

void menuPaint(int menuIndex)
{
	// DEBUG test drawing:
//	menuUpdated = true;
	
	if(menuUpdated)
	{
		menuUpdated = false;
		int add=0;
		if(getMenuData(menuIndex, 0)!=MENU_DATA_UNDEFINED) // draw bg image
			drawImage(getMenuData(menuIndex, 0), 0, 0, ALIGN_LT);
		else // TODO - placeholder pic for rest of the menus
			drawImage(IMAGE_INSTRUCTIONS_BG_INDEX, 0, 0, ALIGN_LT);
			
		if((!networkConnected||networkInitCounter<7)&&(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX||currentMenu==MENU_NETWORK_LOBBY_INDEX))
			return;

		setColor(MENU_COLOR_NORMAL);
		for(int i=0; i<getMenuLength(menuIndex); i++)
		{
			if(getMenuType(currentMenu)==MENU_TYPE_NORMAL||getMenuType(currentMenu)==MENU_TYPE_GAME)
			{
				if(i>0||(currentMenu==MENU_NETWORK_LOBBY_INDEX&&game[LOCAL].maxPlayers==8))
				{
					if(getMenuType(currentMenu)==MENU_TYPE_NORMAL&&(currentMenu!=MENU_PLAYERS_INDEX||hotseatPlayers<8))
						add=1;
						
					if(currentMenu==MENU_NETWORK_LOBBY_INDEX)
					{
						int index = 1;
						if(game[LOCAL].maxPlayers==8) // don't show title if we're waiting for 8 players
							index = 0;
						if(i-index>=game[LOCAL].numPlayers||!menuPlayerReady(i-index))
							setColor(MENU_COLOR_INACTIVE);
						else
							setColor(MENU_COLOR_NORMAL);
					}
					else
					{
						if(i==currentMenuIndex)
							setColor(MENU_COLOR_ACTIVE);
						else
							setColor(MENU_COLOR_NORMAL);
					}
				}
			}
			else if(currentMenu==MENU_ABOUT_INDEX)
			{
				if(i%2==0)
					setColor(MENU_COLOR_ACTIVE);
				else
					setColor(MENU_COLOR_NORMAL);
			}
			else
				setColor(MENU_COLOR_TEXT);
	
			#ifdef DISABLE_NETWORK_GAME
				if(menuIndex==MENU_MAIN_INDEX&&i==2)
					setColor(MENU_COLOR_INACTIVE);
			#endif
			
			if(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX&&networkStatus==NETWORK_STATUS_CONNECTED&&numOpenGames>=i&&openGameStatus[i-1]==GAME_STATUS_PLAYING)
			{
				if((game[LOCAL].gameTime>>8)%2==0)
					setColor(0xffffff);
				else
				{
					if(i==currentMenuIndex)
						setColor(MENU_COLOR_ACTIVE);
					else
						setColor(0x000000);
				}
			}

			int img = getMenuImageIndex(menuIndex, i);
			if(img!=-1)
			{
				drawSubImage(MENU_IMAGES[img+1],
			            getMenuAttribute(menuIndex, MENU_ATTRIBUTE_X)+MENU_IMAGES[img+2]+6,
//			            stringWidth((i==0?FONT_TRUETYPE:getMenuAttribute(menuIndex, MENU_ATTRIBUTE_FONT)), getMenuRow(menuIndex, i))+5,
//			            getMenuAttribute(menuIndex, MENU_ATTRIBUTE_X),
			            getMenuAttribute(menuIndex, MENU_ATTRIBUTE_Y)+(i+add)*getMenuAttribute(menuIndex, MENU_ATTRIBUTE_HEIGHT)-1,
			            getMenuAttribute(menuIndex, MENU_ATTRIBUTE_ALIGN),
			            MENU_IMAGES[img+2],
			            MENU_IMAGES[img+3],
			            MENU_IMAGES[img+4],
			            MENU_IMAGES[img+5]);
				if(i==currentMenuIndex)
					drawRect(getMenuAttribute(menuIndex, MENU_ATTRIBUTE_X)+6,
			             getMenuAttribute(menuIndex, MENU_ATTRIBUTE_Y)+(i+add)*getMenuAttribute(menuIndex, MENU_ATTRIBUTE_HEIGHT)-1,
			             MENU_IMAGES[img+2],
			             MENU_IMAGES[img+3]);
/*					drawRect(getMenuAttribute(menuIndex, MENU_ATTRIBUTE_X)+5,
			             getMenuAttribute(menuIndex, MENU_ATTRIBUTE_Y)+(i+add)*getMenuAttribute(menuIndex, MENU_ATTRIBUTE_HEIGHT)-2,
			             MENU_IMAGES[img+2]+2,
			             MENU_IMAGES[img+3]+2);*/
			}
			drawString((i==0?FONT_TRUETYPE:getMenuAttribute(menuIndex, MENU_ATTRIBUTE_FONT)),
			           getMenuRow(menuIndex, i),
			           getMenuAttribute(menuIndex, MENU_ATTRIBUTE_X),
//			           getMenuAttribute(menuIndex, MENU_ATTRIBUTE_X)-(img!=-1?MENU_IMAGES[img+2]+4:0),
			           getMenuAttribute(menuIndex, MENU_ATTRIBUTE_Y)+(i+add)*getMenuAttribute(menuIndex, MENU_ATTRIBUTE_HEIGHT),
			           getMenuAttribute(menuIndex, MENU_ATTRIBUTE_ALIGN));

			if(getMenuAction(menuIndex, i)==MENU_ACTION_EDIT_SLIDER) // draw slider options
			{
				switch(getMenuData(menuIndex, i))
				{
					case MENU_EDIT_FAVOURITE_COLOR: // color options
					{
						drawSlider(FONT_TRUETYPE, -1, getMenuAttribute(menuIndex, MENU_ATTRIBUTE_X)+stringWidth(FONT_TRUETYPE, getMenuRow(menuIndex, i)),
						           getMenuAttribute(menuIndex, MENU_ATTRIBUTE_Y)+(i+add)*getMenuAttribute(menuIndex, MENU_ATTRIBUTE_HEIGHT),
				           		 getMenuAttribute(menuIndex, MENU_ATTRIBUTE_ALIGN), i);
					}
					break;

					case MENU_EDIT_NUM_PLAYERS:
					{
						drawSlider(FONT_TRUETYPE, game[LOCAL].numPlayers, getMenuAttribute(menuIndex, MENU_ATTRIBUTE_X)+stringWidth(FONT_TRUETYPE, getMenuRow(menuIndex, i)),
						           getMenuAttribute(menuIndex, MENU_ATTRIBUTE_Y)+(i+add)*getMenuAttribute(menuIndex, MENU_ATTRIBUTE_HEIGHT),
				           		 getMenuAttribute(menuIndex, MENU_ATTRIBUTE_ALIGN), i);
					}
					break;

					case MENU_EDIT_HOTSEAT_PLAYERS:
					{
						drawSlider(FONT_TRUETYPE, hotseatPlayers, getMenuAttribute(menuIndex, MENU_ATTRIBUTE_X)+stringWidth(FONT_TRUETYPE, getMenuRow(menuIndex, i)),
						           getMenuAttribute(menuIndex, MENU_ATTRIBUTE_Y)+(i+add)*getMenuAttribute(menuIndex, MENU_ATTRIBUTE_HEIGHT),
				           		 getMenuAttribute(menuIndex, MENU_ATTRIBUTE_ALIGN), i);
					}
					break;

					case MENU_EDIT_MIN_PLAYERS:					
					{
						drawSlider(FONT_TRUETYPE, minPlayers, getMenuAttribute(menuIndex, MENU_ATTRIBUTE_X)+stringWidth(FONT_TRUETYPE, getMenuRow(menuIndex, i)),
						           getMenuAttribute(menuIndex, MENU_ATTRIBUTE_Y)+(i+add)*getMenuAttribute(menuIndex, MENU_ATTRIBUTE_HEIGHT),
				           		 getMenuAttribute(menuIndex, MENU_ATTRIBUTE_ALIGN), i);
					}
					break;

					case MENU_EDIT_MAX_PLAYERS:					
					{
						drawSlider(FONT_TRUETYPE, maxPlayers, getMenuAttribute(menuIndex, MENU_ATTRIBUTE_X)+stringWidth(FONT_TRUETYPE, getMenuRow(menuIndex, i)),
						           getMenuAttribute(menuIndex, MENU_ATTRIBUTE_Y)+(i+add)*getMenuAttribute(menuIndex, MENU_ATTRIBUTE_HEIGHT),
				           		 getMenuAttribute(menuIndex, MENU_ATTRIBUTE_ALIGN), i);
					}
					break;
				}
			}
		}

		if(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX&&networkStatus<=NETWORK_STATUS_CONNECTED&&numOpenGames<1)
		{
			setColor(MENU_COLOR_INACTIVE);
			if(numOpenGames<=0)
				drawString(FONT_TRUETYPE, "No open games", X_CEN, FONT_TRUETYPE_HEIGHT+2*FONT_TRUETYPE_HEIGHT, ALIGN_HCT);
			else
				drawString(FONT_TRUETYPE, "Fetching games list", X_CEN, FONT_TRUETYPE_HEIGHT+2*FONT_TRUETYPE_HEIGHT, ALIGN_HCT);
		}
		
		if(currentMenu!=MENU_MAIN_INDEX&&currentMenu!=MENU_QUIT_INDEX) // draw back arrow
		{
			drawSubImage(IMAGE_ARROWS_INDEX, 1, Y_RES-1, ALIGN_LB, IMAGE_ARROWS_WIDTH>>2, IMAGE_ARROWS_HEIGHT, (menuHoverLeft?IMAGE_ARROWS_WIDTH>>2:0)+(IMAGE_ARROWS_WIDTH>>1), 0);
		}
	
		if(getMenuType(currentMenu)==MENU_TYPE_GAME&&currentMenu!=MENU_QUIT_INDEX) // draw forward arrow
		{
			drawSubImage(IMAGE_ARROWS_INDEX, X_RES-1, Y_RES-1, ALIGN_RB, IMAGE_ARROWS_WIDTH>>2, IMAGE_ARROWS_HEIGHT, (menuHoverRight?IMAGE_ARROWS_WIDTH>>2:0), 0);
		}

		if(currentMenu==MENU_QUIT_INDEX)
		{
			drawImage(IMAGE_NUMBERS_BOX_INDEX, X_CEN, Y_CEN, ALIGN_HCVC);
			setColor(0xffffff);
			drawString(FONT_TRUETYPE, "Quit game?", X_CEN, Y_CEN-16, ALIGN_HCT);
			if(menuHoverLeft) setColor(MENU_COLOR_ACTIVE);
			else setColor(0xffffff);
			drawString(FONT_TRUETYPE, "Yes", X_CEN-22, Y_CEN+4, ALIGN_HCT);
			if(menuHoverRight) setColor(MENU_COLOR_ACTIVE);
			else setColor(0xffffff);
			drawString(FONT_TRUETYPE, "No", X_CEN+22, Y_CEN+4, ALIGN_HCT);
		}		
	}
}
