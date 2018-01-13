
void initGame()
{
	// TEST:
	//gameState = GAME_STATE_KEYBOARD;
	//prevState = GAME_STATE_MENU;
	
	// ORIG:
	gameState = GAME_STATE_MENU;
	
	currentMenu = MENU_MAIN_INDEX;
	currentMenuIndex = 1;
	kbdInit();
	initFont();
	for(int i=0;i<GAME_HISTORY_LENGTH;i++)
	{
		gameHistory[i] = (char*)malloc(256);
		memset(gameHistory[i], 0, 256);
	}
	gameHistoryCount = 5;
	#ifdef DEBUG
	for(int i=0;i<GAME_HISTORY_LENGTH;i++)
	{
		debugHistory[i] = (char*)malloc(256);
		memset(debugHistory[i], 0, 256);
	}
	debugHistoryCount = 5;
	#endif

  game[LOCAL].numPlayers=7;
  game[LOCAL].minPlayers=2;
  game[LOCAL].maxPlayers=7;
  dbStatisticsReset();

	networkRequestSent = false;
//	playSoundLooping(SOUND_MENU_LOOP_INDEX, 0);
}

void startGame(int gameid, int mode)
{
	networkMode = mode;
	
	curArea=attackArea=AREA_NONE;
	numDice=1;

	initGameField(LOCAL, game[LOCAL].gameField);
	
	// DEBUG end of the game
	//for(int i=0;i<NUM_AREAS-1;i++)
	//	game[gameid].playerArea[i]=playerId;

	game[gameid].pot = 0;		
	game[gameid].curTurn = 0;
	for(int i=0;i<MAX_PLAYERS;i++) 
	{
		game[gameid].diceReserve[i]=0;
		game[gameid].playerScore[i] = game[gameid].gameScore[i] = 0;
		game[gameid].playerScore[i] -= 2;
		game[gameid].pot += 2;
		game[gameid].playerSurrended[i] = false;
		
		// DEBUG end of the game
		/*if(i!=playerId&&i!=game[gameid].playerArea[NUM_AREAS-1]) 
		{
			logPrintf("marking %d dead (playerid: %d, last area: %d)\n", i, playerId, game[gameid].playerArea[NUM_AREAS-1]);
			game[gameid].playerStatus[i]=PLAYER_STATUS_DEAD;
		}*/
	}
	game[gameid].playerScore[0] -= 2; // first turn automated
	game[gameid].pot += 2;		
	initPressArea();
	updatePlayerPositions();

	game[LOCAL].position = game[LOCAL].numPlayers;
	game[LOCAL].gameStatus = GAME_STATUS_PLAYING;
	game[LOCAL].curPlayer = 0;
	game[LOCAL].stateTimer = game[LOCAL].gameTime;
	
	sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], " ");
	sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], " ");
	if(mode==NETWORK_MODE_LOCAL)
		sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], "*** STARTING A NEW LOCAL GAME ***");
	else
		sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], "*** STARTING A NEW NETWORK GAME ***");
	sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], "* You're player #%d", (playerId+1));
	sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], "* Your player color is %s", colorName[game[LOCAL].playerColor[playerId]]);
	
	endTurnHover = false;
	surrenderHover = false;
	playerDataUpdated = true;
	gameFieldUpdated = true;	
	
//	stopSound(SOUND_MENU_LOOP_INDEX);
	playSound(SOUND_START_GAME_INDEX);
}

void keyPressed(int key)
{
//	printf("key pressed: %d, state: %d\n", key, gameState);
	if(gameState==GAME_STATE_KEYBOARD)
	{
		if(key==KEY_POINTER)
			pointerDown = true;
		else if(key==KEY_SELECT)
		{
			kbdDown = 0x21; // simulate enter button on the virtual keyboard
			kbdPressed();
		}
		else if(key==KEY_BACK)
		{
			kbdDown = 0x16; // simulate backspace button on the virtual keyboard
			kbdPressed();
		}
		else if(key==KEY_ESCAPE)
			gameState = prevState; // abort writing
	}
	else if(gameState>=GAME_STATE_INGAME_START&&gameState<=GAME_STATE_INGAME_POST_ATTACK&&gameState!=GAME_STATE_INGAME_MENU)
	{
		if(key==KEY_ESCAPE) // goto ingame menu & pause game (pause only if local game - TODO!)
		{
			playSound(SOUND_SELECT_INDEX);
			pauseTime = game[LOCAL].gameTime;
			prevState = gameState;
			gameState = GAME_STATE_INGAME_MENU;
		}
		if((gameState==GAME_STATE_INGAME||gameState==GAME_STATE_INGAME_END)&&key==KEY_POINTER)
		{
			pointerDown = true;
			if(curArea==AREA_NONE)
				readyToAttack = false;
		}
		#ifdef DEBUG
		// allow choosing game field on the fly
		if(key==KEY_RIGHT)
		{
			if(++game[LOCAL].gameField>=NUM_FIELDS) game[LOCAL].gameField=0;
			initGameField(LOCAL, game[LOCAL].gameField);
			initPressArea();
			gameFieldUpdated = playerDataUpdated = true;
		}
		if(key==KEY_LEFT)
		{
			if(--game[LOCAL].gameField<0) game[LOCAL].gameField=NUM_FIELDS-1;
			initGameField(LOCAL, game[LOCAL].gameField);
			initPressArea();
			gameFieldUpdated = playerDataUpdated = true;
			
		}
		if(key==KEY_UP)
		{
			if(--debugHistoryCount<0) debugHistoryCount = GAME_HISTORY_LENGTH-1;
		}
		if(key==KEY_DOWN)
		{
			debugHistoryCount++;
		}
		#endif
	}
	else if(gameState==GAME_STATE_INGAME_MENU)
	{
		if(key==KEY_ESCAPE) // goto back to game
		{
			playSound(SOUND_RESELECT_INDEX);
			gameState = prevState;
			game[LOCAL].gameTime = pauseTime;
			gameFieldUpdated = true;
		}
		if(key==KEY_SELECT) // end game and go to game menu
		{
			playSound(SOUND_SELECT_INDEX);
			clientQuitGame();
			currentMenu = MENU_MAIN_INDEX;
			currentMenuIndex = 1;
			menuUpperDrawn = false;
			networkRequestSent = false;
			gameState = GAME_STATE_MENU;
//			playSoundLooping(SOUND_MENU_LOOP_INDEX, 0);
		}
		if(key==KEY_POINTER)
		{
			pointerDown = true;
		}
	}
	else if(gameState==GAME_STATE_MENU)
	{
		menuKeyPressed(key);
	}
	
	//printf("key pressed done.\n");
	//fflush(stdout);
}

void keyReleased(int key)
{
	if(gameState==GAME_STATE_KEYBOARD)
	{
		if(key==KEY_POINTER)
		{
			pointerDown = false;
			kbdPressed();
		}
	}
	else if(gameState==GAME_STATE_INGAME)
	{
		if(key==KEY_POINTER)
		{
			if(endTurnHover) // end turn
			{
				endTurnHover = false;
				endTurnPressed = true;
				endTurn();
			}
			else if(surrenderHover) // surrender?
			{
				surrenderHover = false;
				menuHoverLeft = false;
				menuHoverRight = false;
				surrenderBox = true;
			}
			else if(menuHoverLeft) // surrender!
			{
				playSound(SOUND_SELECT_INDEX);
				surrenderBox = false;
				surrenderHover = false;
				menuHoverLeft = false;
				menuHoverRight = false;
				gameFieldUpdated = true;
				surrender();
			}
			else if(menuHoverRight) // back to the game
			{
				surrenderBox = false;
				surrenderHover = false;
				menuHoverLeft = false;
				menuHoverRight = false;
				gameFieldUpdated = true;
			}
			pointerDown = false;
			readyToAttack = true;
		}
	}
	if(gameState==GAME_STATE_INGAME_END&&key==KEY_POINTER&&endTurnHover)
	{
		playSound(SOUND_SELECT_INDEX);
		clientQuitGame();
		currentMenu = MENU_MAIN_INDEX;
		currentMenuIndex = 0;
		menuUpperDrawn = false;
		networkRequestSent = false;
		gameState = GAME_STATE_MENU;
//		playSoundLooping(SOUND_MENU_LOOP_INDEX, 0);
	}
	if(gameState==GAME_STATE_INGAME_MENU)
	{
		if(key==KEY_POINTER)
		{
			pointerDown = false;
			if(menuHoverLeft) // end game
			{
				playSound(SOUND_SELECT_INDEX);
				clientQuitGame();
				currentMenu = MENU_MAIN_INDEX;
				currentMenuIndex = 0;
				menuUpperDrawn = false;
				networkRequestSent = false;
				gameState = GAME_STATE_MENU;
			}
			else // back to the game
			{
				playSound(SOUND_RESELECT_INDEX);
				gameState = prevState;
				game[LOCAL].gameTime = pauseTime;
				gameFieldUpdated = true;
			}
		}
	}
	if(gameState==GAME_STATE_MENU)
		menuKeyReleased(key);

//	printf("key released done.\n");
//	fflush(stdout);
}

void updatePlayerPositions()
{
	int i,j,k;
	int positions[game[LOCAL].numPlayers];
	int numPositions = 0;
	for(i=0;i<game[LOCAL].numPlayers;i++)
	{
		positions[i] = -1;
		game[LOCAL].playerAreaCount[i] = game[LOCAL].playerDiceCount[i] = 0;
	}

	// DEBUG - ensure player area & dice count
	for(i=0;i<NUM_AREAS;i++)
	{
		game[LOCAL].playerAreaCount[game[LOCAL].playerArea[i]]++;
		game[LOCAL].playerDiceCount[game[LOCAL].playerArea[i]] += game[LOCAL].areaDice[i];
	}
	
	for(i=0;i<game[LOCAL].numPlayers;i++)
	{
		if(game[LOCAL].playerStatus[i]!=PLAYER_STATUS_DEAD&&!game[LOCAL].playerSurrended[i]) // still in the game, calculate position
		{
			for(j=0;j<=numPositions;j++)
			{
				if(positions[j]==-1) // free slot, this is our position
				{
					positions[j] = i;
					numPositions++;
//					logPrintf("free position %d now: Player %d (%d areas)\n", (j+1), (positions[j]+1), game[LOCAL].playerAreaCount[positions[j]]);
					break;
				}
				else // compare if we're better or not
				{
//					logPrintf("P%d:%d vs. P%d:%d\n", (i+1), game[LOCAL].playerAreaCount[i], (positions[j]+1), game[LOCAL].playerAreaCount[positions[j]]);

					if((game[LOCAL].playerAreaCount[i]>game[LOCAL].playerAreaCount[positions[j]])||
					   (game[LOCAL].playerAreaCount[i]==game[LOCAL].playerAreaCount[positions[j]]&&
					    game[LOCAL].playerDiceCount[i]>game[LOCAL].playerDiceCount[positions[j]]))
					{
						// update positions
						for(k=numPositions;k>j;k--)
						{
							positions[k] = positions[k-1];
//							logPrintf("position %d now: Player %d (%d areas)\n", (k+1), (positions[k]+1), game[LOCAL].playerAreaCount[positions[k]]);
						}
						positions[j] = i;
						numPositions++;
//						logPrintf("position end %d now: Player %d (%d areas)\n", (j+1), (positions[j]+1), game[LOCAL].playerAreaCount[positions[j]]);
						break;
					}
				}
			}
		}
	}
	
	int tempPot = game[LOCAL].pot;
	for(i=numPositions;i>0;i--)
	{
//		logPrintf("Final position %d: Player %d (%d areas)\n", i, (positions[i-1]+1), game[LOCAL].playerAreaCount[positions[i-1]]);
		game[LOCAL].playerPosition[positions[i-1]] = (i);
//		logPrintf("Current pot: %d, factor: %d, multiplier: %d\n", tempPot, playerScoreFactor[numPositions-1], (game[LOCAL].numPlayers-i-1));
		int points = (scoreTab[game[LOCAL].numPlayers-2][game[LOCAL].numPlayers-i]*tempPot)/10000;
//		printf("%d. points: %d\n", i, points);
//		logPrintf("::: player points %d: %d+%d out of %d = %d\n", i, game[LOCAL].playerScore[positions[i-1]], points, tempPot, (game[LOCAL].playerScore[positions[i-1]]+points));
		tempPot -= points;
//		logPrintf("::: pot now: %d\n", tempPot);

		game[LOCAL].gameScore[positions[i-1]] = game[LOCAL].playerScore[positions[i-1]]+points;
	}
}

void processAttack()
{
	boolean endGame = false;

	if(playerWon==PLAYER_ATTACKER&&game[LOCAL].playerAreaCount[game[LOCAL].playerArea[attackArea]]==1)
	{
		// player died
		game[LOCAL].playerStatus[game[LOCAL].playerArea[attackArea]]=PLAYER_STATUS_DEAD;
		playSound(SOUND_GAME_OVER_INDEX); // TODO: get better dead sound instead
		if(networkMode==NETWORK_MODE_LOCAL)
		{
			if(game[LOCAL].playerArea[attackArea]==playerId&&hotseatPlayers==1) // it was you who died! boohoo..
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> You are dead! <<");
			else
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> Player %s is dead! <<", 
				        game[LOCAL].playerName[game[LOCAL].playerArea[attackArea]]);
		}
				
		if(networkMode==NETWORK_MODE_LOCAL)
		{
			// if player is the only one left, end game
			int notDead = 0;
			int notDeadLocal = 0;
			int playerWon = -1;

			logPrintf("gamescore before: %d\n", game[LOCAL].gameScore[game[LOCAL].playerArea[attackArea]]);
			game[LOCAL].gameScore[game[LOCAL].playerArea[attackArea]] = dbPlayerPosition(LOCAL, game[LOCAL].playerArea[attackArea], game[LOCAL].position--);
			logPrintf("gamescore after: %d\n", game[LOCAL].gameScore[game[LOCAL].playerArea[attackArea]]);
			
			for(int i=0;i<game[LOCAL].numPlayers;i++)
			{
				if(game[LOCAL].playerStatus[i]!=PLAYER_STATUS_DEAD&&!game[LOCAL].playerSurrended[i])
				{
					if(game[LOCAL].playerType[i]==PLAYER_TYPE_LOCAL)
						notDeadLocal++;
					notDead++;
					playerWon = i;
				}
			}
			if(notDeadLocal==0||notDead==1)
			{
				game[LOCAL].gameScore[game[LOCAL].playerArea[curArea]] = dbPlayerPosition(LOCAL, game[LOCAL].playerArea[curArea], game[LOCAL].position--);
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> GAME ENDED <<");
				if(hotseatPlayers==1)
				{
					if(notDeadLocal==1)
					{
						sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> You won! <<");
						playSound(SOUND_GAME_WON_INDEX);
					}
					else
					{
						sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> You lost! <<");
						playSound(SOUND_GAME_OVER_INDEX);
					}
				}
				else
				{
					sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> Player %s won! <<", game[LOCAL].playerName[playerWon]);
					playSound(SOUND_GAME_WON_INDEX);
				}
				gameFieldUpdated = true;
				endGame = true;
				gameState = GAME_STATE_INGAME_END;
				game[LOCAL].stateTimer=game[LOCAL].gameTime;
				game[LOCAL].gameStatus = GAME_STATUS_ENDED;
			}
		}
	}
	else
	{
		logPrintf("Area count for the lost one: %d\n", game[LOCAL].playerAreaCount[game[LOCAL].playerArea[attackArea]]);
		logPrintf("attack area: %d\n", attackArea);
	}

	if(playerWon==PLAYER_ATTACKER) // attacker won
	{
		playSound(SOUND_ATTACK_WON_INDEX);
		game[LOCAL].playerAreaCount[game[LOCAL].playerArea[curArea]]++;
		game[LOCAL].playerAreaCount[game[LOCAL].playerArea[attackArea]]--;
		game[LOCAL].playerDiceCount[game[LOCAL].playerArea[attackArea]] -= game[LOCAL].areaDice[attackArea];
		game[LOCAL].playerArea[attackArea] = game[LOCAL].curPlayer;
	}
	else
	{
		playSound(SOUND_ATTACK_LOST_INDEX);
		game[LOCAL].playerDiceCount[game[LOCAL].playerArea[curArea]] -= game[LOCAL].areaDice[curArea]-1;
	}

	game[LOCAL].areaDice[curArea] = attackDice[0];
	game[LOCAL].areaDice[attackArea] = attackDice[1];
	if(!endGame)
	{
		if(playerId==game[LOCAL].curPlayer||game[LOCAL].playerType[game[LOCAL].curPlayer]==PLAYER_TYPE_LOCAL)
			gameState = GAME_STATE_INGAME;
		else
		{
			logPrintf("change game state INGAME_WAITING...\n");
			gameState = GAME_STATE_INGAME_WAITING;
		}
	}

	curArea = attackArea = AREA_NONE;
	readyToAttack = false;
	logPrintf("attack done, back to normal ingame.\n");
	playerDataUpdated = true;
	attackProcessed = true;
}

void gameLogic()
{
	// hovering buttons?
	if((pointerX>=X_RES-1-(IMAGE_ENDTURN_WIDTH>>1)&&pointerX<X_RES-1&&pointerY>=Y_RES-2-IMAGE_ENDTURN_HEIGHT&&pointerY<Y_RES-2)&&
	   (game[LOCAL].gameTime-game[LOCAL].stateTimer>500)) // only possible to press after half a second after turn start
		endTurnHover = true;
	else
		endTurnHover = false;

	if(game[LOCAL].curTurn>=3)
	{
		if((pointerX>=1&&pointerX<1+(IMAGE_SURRENDER_WIDTH>>1)&&pointerY>=Y_RES-2-IMAGE_ENDTURN_HEIGHT&&pointerY<Y_RES-2)&&
	     (game[LOCAL].gameTime-game[LOCAL].stateTimer>500)) // only possible to press after half a second after turn start
			surrenderHover = true;
		else
			surrenderHover = false;		
	}

	if(gameState==GAME_STATE_INGAME_START&&!gameStartProcessed)
	{
		int playOrder[MAX_PLAYERS];
		logPrintf("game state start logic()\n");
		if(networkMode==NETWORK_MODE_LOCAL) // generate a new local game
		{
      logPrintf("START_GAME: %d\n", LOCAL); // receiveBuffer[1]);				
      do // we have certain number of evil gamefields, please skip those
      {
	      game[LOCAL].gameField=rand()%NUM_FIELDS;
			} while(game[LOCAL].gameField==21||game[LOCAL].gameField==26||game[LOCAL].gameField==27||game[LOCAL].gameField==38||game[LOCAL].gameField==39);
			
			// DEBUG
//			game[LOCAL].gameField = 16;
						
//      playerId = rand()%game[LOCAL].numPlayers;
      int j=0;
      int cpuNum = 1;
      for(int i=0;i<game[LOCAL].numPlayers;i++) 
      {
      	playOrder[i] = -1;
				game[LOCAL].playerColor[i] = -1;
      }
      for(int i=0;i<game[LOCAL].numPlayers;i++)
      {
				int pos = rand()%game[LOCAL].numPlayers;
      	if(i<hotseatPlayers)
      	{
					logPrintf("pos: %d\n", pos);
					while(playOrder[pos]!=-1)
					{
						pos = rand()%game[LOCAL].numPlayers;
					}
					logPrintf("rand pos: %d out of %d\n", pos, game[LOCAL].numPlayers);
					playOrder[pos] = i;
	    		for(int k=0;k<MAX_NAME_LENGTH;k++) // empty name
	    			game[LOCAL].playerName[pos][k] = 0;

					for(int j=0;j<MAX_NAME_LENGTH;j++)
					{
						game[LOCAL].playerName[pos][j] = playerName[i][j];
					}
	    		game[LOCAL].playerType[pos] = PLAYER_TYPE_LOCAL;
					game[LOCAL].playerStatus[pos] = PLAYER_STATUS_WAITING;
      	}
      	else
      	{
      		logPrintf("adding cpu %d\n", cpuNum);
      		for(int j=0;j<game[LOCAL].numPlayers;j++)
      		{
      			if(playOrder[j]==-1)
      			{
      				playOrder[j] = i;
      				pos = j;
      				break;
      			}
      		}
    			sprintf(game[LOCAL].playerName[pos], "CPU%d", cpuNum);
	    		game[LOCAL].playerType[pos] = PLAYER_TYPE_CPU;
					game[LOCAL].playerStatus[pos] = PLAYER_STATUS_WAITING;
	    		cpuNum++;
      	}
				if(i==0) 
				{
					playerId = pos;
					game[LOCAL].playerColor[pos] = favouriteColor;
				}
				else
				{
					logPrintf("rand color\n");
					game[LOCAL].playerColor[pos] = rand()%game[LOCAL].numPlayers;
					boolean ret;
					do
					{
						ret = true;
						if(++game[LOCAL].playerColor[pos]>=game[LOCAL].numPlayers) game[LOCAL].playerColor[pos]=0;

						if(game[LOCAL].playerColor[pos]==favouriteColor)
							if(++game[LOCAL].playerColor[pos]>=game[LOCAL].numPlayers) game[LOCAL].playerColor[pos]=0;
						
						for(int ii=0;ii<game[LOCAL].numPlayers;ii++)
						{
							if(pos!=ii&&game[LOCAL].playerColor[ii]==game[LOCAL].playerColor[pos])
								ret = false;
						}
					} while(!ret);
					j++;
	    	}
      }
			game[LOCAL].playerStatus[0] = PLAYER_STATUS_READY;
			resetPlayerAreas(LOCAL);
			startGame(LOCAL, networkMode);
		}
		else
		{
			logPrintf("network game, waiting for the server...\n");
		}
		gameStartProcessed = true;
	}
	else if(gameState==GAME_STATE_INGAME)
	{
		if(game[LOCAL].gameTime-game[LOCAL].stateTimer>3000&&!endTurnChecked&&game[LOCAL].playerType[game[LOCAL].curPlayer]==PLAYER_TYPE_LOCAL)
		{
//			logPrintf("time: %d\n", (game[LOCAL].gameTime-game[LOCAL].stateTimer));
			// check if the player can still attack
			boolean endT = true;
			for(int i=0;i<NUM_AREAS;i++)
			{
				if(game[LOCAL].playerArea[i]==game[LOCAL].curPlayer&&game[LOCAL].areaDice[i]>1)
				{
					for(int j=0;j<MAX_AREA_CONNECTIONS;j++)
					{
						if(game[LOCAL].areaConnections[i][j]==AREA_NONE)
							break;
						else
						{
							if(game[LOCAL].playerArea[game[LOCAL].areaConnections[i][j]]!=game[LOCAL].curPlayer) // can still attack!
							{
								endT = false;
								break;
							}
						}
					}
				}
			}
			if(endT&&!endTurnPressed)
			{
				logPrintf("attacks done, end automatically...\n");
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], "* Out of moves, ending turn automatically!");
				playerDataUpdated = true;
				endTurn();
			}
			endTurnChecked = true;
		}
		if((networkMode==NETWORK_MODE_TCP||networkMode==NETWORK_MODE_LOBBY)&&
		   (game[LOCAL].gameTime-game[LOCAL].stateTimer)>26500&&!endTurnPressed) // timeout, end turn
		{
			sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> Turn ended automatically due to inactivity.");
			endTurn();
/*			gameFieldUpdated = true;
			playerDataUpdated = true;
			game[LOCAL].stateTimer = game[LOCAL].gameTime;*/
			gameState = GAME_STATE_INGAME_WAITING;
			return;
		}
		if(attackArea!=AREA_NONE&&readyToAttack)
		{
			gameState = GAME_STATE_INGAME_WAITING;

			logPrintf("now we're attacking from: %d to %d!\n", curArea, attackArea);
			logPrintf("player %s vs. %s\n", game[LOCAL].playerName[game[LOCAL].playerArea[curArea]], game[LOCAL].playerName[game[LOCAL].playerArea[attackArea]]);
			byte data[NETWORK_MAX_DATA];
			data[0] = MESSAGE_ATTACK; data[1] = curArea; data[2] = attackArea;
			#ifdef DEBUG
			sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> %d ATT %d", data[1], data[2]);
			#endif
			sendClientData(data);
		}
  	if(pointerDown&&!surrenderBox&&!surrenderHover&&!endTurnHover) // select attack areas
		{
			int tmp = pointerTab[pointerX+pointerY*X_RES];
	//		curArea = getCurrentArea(pointerX, pointerY);
			if(curArea!=AREA_NONE&&tmp!=AREA_NONE&&tmp!=curArea&&game[LOCAL].playerArea[tmp]!=game[LOCAL].curPlayer&&areasConnected(LOCAL, curArea, tmp)&&readyToAttack)
			{
				playSound(SOUND_SELECT_INDEX);
				attackArea = tmp;
				gameFieldUpdated = true;
			}
			else if(game[LOCAL].playerArea[tmp]==game[LOCAL].curPlayer||tmp==AREA_NONE)
			{
				if(game[LOCAL].areaDice[tmp]>1)
				{
					playSound(SOUND_SELECT_INDEX);
					curArea = tmp;
					readyToAttack = false;
				}
				else
				{
					playSound(SOUND_RESELECT_INDEX);
					curArea = AREA_NONE;
				}
				pointerDown = false; // let's see if this helps with jamming selects
				gameFieldUpdated = true;
			}
		}
	}
	else if(gameState==GAME_STATE_INGAME_WAITING||gameState==GAME_STATE_INGAME_WAITING_TURN)
	{
		if((networkMode==NETWORK_MODE_LOCAL||(networkMode==NETWORK_MODE_LOBBY&&roomOwner&&roomOwner==pthread_self()))&&
		    game[LOCAL].playerType[game[LOCAL].curPlayer]==PLAYER_TYPE_CPU) // check what AI does
		{
			if(game[LOCAL].gameTime-game[LOCAL].stateTimer>500) // Process AI every half second
			{
				boolean endT = processAI(LOCAL); // does CPU want to end turn?
				if(endT)
				{
					if(networkMode==NETWORK_MODE_LOBBY)
					{
						if(game[LOCAL].gameStatus==GAME_STATUS_PLAYING) // in case the game has ended already
						{
							game[LOCAL].playerLastActiveTurn[game[LOCAL].curPlayer] = game[LOCAL].curTurn;
							endTurnLogic(LOCAL);
						}
					}
					else
						endTurn();
				}

				gameFieldUpdated = true;
				playerDataUpdated = true;
				game[LOCAL].stateTimer = game[LOCAL].gameTime;
			}
		}
	}
	else if(gameState == GAME_STATE_INGAME_POST_ATTACK)
	{
		if(game[LOCAL].gameTime-game[LOCAL].stateTimer>1500)
		{
			processAttack();
			updatePlayerPositions();
		}
		else if(game[LOCAL].gameTime-game[LOCAL].stateTimer<(playerDiceAmount[0]+2)*100)
		{
			if(game[LOCAL].gameTime-diceTimer>80)
			{
				playSound(SOUND_DICE_INDEX);
				diceTimer = game[LOCAL].gameTime;
			}
		}
		if(game[LOCAL].gameTime-game[LOCAL].stateTimer>300&&!attackAreaShown)
		{
			attackAreaShown = true;
			gameFieldUpdated = true;
		}
	}
	else if(gameState==GAME_STATE_INGAME_POST_STATUS||(gameState==GAME_STATE_INGAME_START&&gameStartProcessed))
	{
		boolean finished = false;
		boolean first = true;
		
		int steps = game[LOCAL].gameTime-game[LOCAL].stateTimer;
		if(gameState==GAME_STATE_INGAME_POST_STATUS&&!firstTurn)
			steps = steps/75; // somewhat slower pace for turn end
		else
			steps = steps/35; // fast fill for game start
		
		if(networkMode==NETWORK_MODE_LOCAL) // ||(networkMode==NETWORK_MODE_LOBBY&&roomOwner!=NULL&&roomOwner==pthread_self()))
			if(firstTurn&&gameState==GAME_STATE_INGAME_POST_STATUS)
				firstTurn = false;

		while(steps>tempAreaCounter)
		{
			if(tempAreaCounter<dicePilingCounter)
			{
				tempAreaDice[dicePilingBuffer[tempAreaCounter]]++;
				tempAreaCounter++;
				gameFieldUpdated = true;
//				logPrintf("added %d dice.\n", tempAreaCounter);
				if(first)
				{
					playSound(SOUND_DICE_INDEX);
					first = false;
				}
				if(tempAreaCounter==dicePilingCounter)
				{
//					logPrintf("tempAreaCounter==dicePilingCounter\n");
					finished = true;
					break;
				}
			}
			else
			{
//				logPrintf("tempAreaCounter>=dicePilingCounter\n");
				finished = true;
				break;
			}
		}
		
		int waitAmt = 5+game[LOCAL].diceReserve[tempPlayer];
		if(waitAmt>10) waitAmt=10;

		if((finished&&(steps-tempAreaCounter)>waitAmt)) // ||(firstTurn&&gameState==GAME_STATE_INGAME_POST_STATUS)) // we've drawn all new dice, update the whole info
		{
//			if(networkMode==NETWORK_MODE_TCP||(networkMode==NETWORK_MODE_LOBBY&&roomOwner!=NULL&&roomOwner!=pthread_self()))
			if(networkMode!=NETWORK_MODE_LOCAL) // ||(networkMode==NETWORK_MODE_LOBBY&&roomOwner!=NULL&&roomOwner!=pthread_self()))
				if(firstTurn&&gameState==GAME_STATE_INGAME_POST_STATUS)
					firstTurn = false;

			logPrintf("current player now: %d\n", game[LOCAL].curPlayer);
			if(!firstTurn||networkMode==NETWORK_MODE_LOCAL)
			{
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> %s's turn <<", game[LOCAL].playerName[game[LOCAL].curPlayer]);
				playSound(SOUND_START_GAME_INDEX);
			}
			if(game[LOCAL].curPlayer==playerId||game[LOCAL].playerType[game[LOCAL].curPlayer]==PLAYER_TYPE_LOCAL)
			{
				logPrintf("change state to INGAME (client network)\n");
				gameState = GAME_STATE_INGAME;
			}
			else
			{
				logPrintf("change state to INGAME_WAITING_TURN (client network)\n");
				gameState = GAME_STATE_INGAME_WAITING_TURN;
			}
			updatePlayerPositions();
			game[LOCAL].stateTimer = game[LOCAL].gameTime;
			curArea = attackArea = AREA_NONE;
			playerDataUpdated = true;
			gameFieldUpdated = true;
		}
	}
	
	if(gameState==GAME_STATE_INGAME_MENU||(gameState==GAME_STATE_INGAME&&surrenderBox))
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
}

void endTurn()
{
	byte data[NETWORK_MAX_DATA];
	data[0] = MESSAGE_END_TURN;
	#ifdef DEBUG
	sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> END TURN");
	#endif
	playSound(SOUND_END_TURN_INDEX);
  sendClientData(data);
	logPrintf("[client] end turn done\n");
	logPrintf("state now: %d, player now: %d\n", gameState, game[LOCAL].curPlayer);
}

void surrender()
{
	byte data[NETWORK_MAX_DATA];
	data[0] = MESSAGE_SURRENDER;
	#ifdef DEBUG
	sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> SURRENDER");
	#endif
	playSound(SOUND_END_TURN_INDEX);
  sendClientData(data);
	logPrintf("[client] surrender done\n");
}

void drawAreaBorder(int area)
{
	if(area<0) return;
	
	for(int i=0;i<MAX_AREA_BORDERS-1;i++)
	{
		if(game[LOCAL].borderListX[area][i+1]!=-1)
		{
			drawLine(xAnchor+xPos+game[LOCAL].borderListX[area][i], yAnchor+yPos+game[LOCAL].borderListY[area][i], xAnchor+xPos+game[LOCAL].borderListX[area][i+1], yAnchor+yPos+game[LOCAL].borderListY[area][i+1]);
		}
	//	else // done drawing
		//	break;
	}
}


void drawAreaDice(int area)
{
	int i,x,y,xAdd,yAdd;;
	
	if(area<0)
		return;

	if(game[LOCAL].areaDiceWide[area])
	{
		xAdd = 5;
		yAdd = -3;
	}
	else
	{
		xAdd = -3;
		yAdd = -2;
	}
		
	x = game[LOCAL].diceIndex[area]%FIELD_X;
	y = game[LOCAL].diceIndex[area]/FIELD_X;
	if(y%2==1) xAdd += hexagonWidth>>1;
	
	int numDice = game[LOCAL].areaDice[area];
	if(gameState==GAME_STATE_INGAME_START||gameState==GAME_STATE_INGAME_POST_STATUS)
		numDice = tempAreaDice[area];

	if(numDice>4)
	{
//		xAdd--;
		yAdd -= 2;
		if(!game[LOCAL].areaDiceWide[area])
		{
			xAdd -= 3;
			yAdd -= 3;
		}
	}

	if(area==curArea||(area==attackArea&&(attackAreaShown||game[LOCAL].playerType[game[LOCAL].curPlayer]==PLAYER_TYPE_LOCAL)))
	{
		yAdd -= (area==curArea?4:2);
	}
	
	int colorIndex = game[LOCAL].playerColor[game[LOCAL].playerArea[area]];
	if(game[LOCAL].playerSurrended[game[LOCAL].playerArea[area]])
		colorIndex = 8; // all gray
	
	if(numDice>0&&numDice<5)
		drawImage(IMAGE_DICE_SHADOW_INDEX, xAnchor+xPos+x*hexagonWidth+hexagonWidth+xAdd-2, yAnchor+yPos+y*hexagonHeight+hexagonHeight+yAdd+1, ALIGN_LT);
	for(i=0;i<numDice;i++)
	{
		if(i==4)
		{
			xAdd += 4;
			yAdd += 3;
			drawImage(IMAGE_DICE_SHADOW_INDEX, xAnchor+xPos+x*hexagonWidth+hexagonWidth+xAdd-2, yAnchor+yPos+y*hexagonHeight+hexagonHeight+yAdd+1, ALIGN_LT);
		}
		drawSubImage(IMAGE_DICE_SMALL_INDEX, xAnchor+xPos+x*hexagonWidth+hexagonWidth+xAdd, yAnchor+yPos+y*hexagonHeight+hexagonHeight-(i%4)*4+yAdd, ALIGN_HCVC, 9, 9, colorIndex*9, 0);
	}
}

void fillTriangleChar(int x0, int y0, int x1, int y1, int x2, int y2, unsigned char val)
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

	for (int y=ymi; y<yma; y++) {
		int mi = triangleEdges[y*2]+32768>>16, ma = triangleEdges[y*2+1]+32768>>16;
		if (mi>ma) { int t = mi; mi = ma; ma = t; }
		if (mi<0) mi = 0;
		if (ma>X_RES) ma = X_RES;
		for (int x = mi; x<ma; x++) 
		{
//			if(x>=0&&x<X_RES&&y>=0&&y<Y_RES)
			pointerTab[x+y*X_RES] = val;
		}
	}
}

void fillHexagonChar(int x, int y, int width, int height, unsigned char val)
{
	int xinc = width;
	int yinc = height>>1; // *3/7;
	
	fillTriangleChar(x, y, x, y-height, x+xinc, y-yinc, val); // 12 - 2
	fillTriangleChar(x, y, x+xinc, y-yinc, x+xinc, y+yinc, val); // 2 - 4
	fillTriangleChar(x, y, x+xinc, y+yinc, x, y+height, val); // 4 - 6
	fillTriangleChar(x, y, x, y+height, x-xinc, y+yinc, val); // 6 - 8 
	fillTriangleChar(x, y, x-xinc, y+yinc, x-xinc, y-yinc, val); // 8 - 10
	fillTriangleChar(x, y, x-xinc, y-yinc, x, y-height, val); // 10 - 12
}

void initPressArea()
{
	int i;
	for(i=0;i<X_RES*Y_RES;i++)
	{
		pointerTab[i] = FIELD_NONE;
	}

	for(int y=0;y<FIELD_Y;y++)
	{
		for(int x=0;x<FIELD_X;x++)
		{
			i = game[LOCAL].fieldArea[x+y*FIELD_X];
			if(i>FIELD_NONE)
			{
				int xAdd = 0;
				if(y%2==1) xAdd = hexagonWidth>>1;
				fillHexagonChar(xAnchor+xPos+x*hexagonWidth+hexagonWidth+xAdd, yAnchor+yPos+y*hexagonHeight+hexagonHeight, hexagonWidth>>1, ((hexagonHeight>>1)+(hexagonHeight>>2)), i);
			}
		}
	}
}

void drawHexagonLine(int f, int angle)
{
	//					void drawHexagon(int x, int y, int width, int height, boolean horizontal)
	//drawHexagon(xAnchor+xPos+x*hexagonWidth+hexagonWidth+xAdd, yAnchor+yPos+y*hexagonHeight+hexagonHeight, hexagonWidth>>1, ((hexagonHeight>>1)+(hexagonHeight>>2)), true);
	int y = yAnchor+yPos+(f/FIELD_X)*hexagonHeight+hexagonHeight-1;
	int xAdd = 0;
	if((f/FIELD_X)%2==1) xAdd += hexagonWidth>>1;
	int x = xAnchor+xPos+(f%FIELD_X)*hexagonWidth+hexagonWidth+xAdd-1;
	int xinc = hexagonWidth>>1;
	int height = ((hexagonHeight>>1)+(hexagonHeight>>2));
	int yinc = height>>1;
	
	switch(angle)
	{
		case 0:
			drawLine(x, y-height, x+xinc-1, y-yinc); // 12 - 2
		break;
		case 1:
			drawLine(x+xinc, y-yinc, x+xinc, y+yinc-1); // 2 - 4
		break;
		case 2:
			drawLine(x+xinc, y+yinc, x, y+height); // 4 - 6
			drawLine(x+xinc, y+yinc+1, x, y+height+1); // 4 - 6 double
		break;
		case 3:
			drawLine(x-1, y+height, x-xinc, y+yinc); // 6 - 8 
			drawLine(x-1, y+height+1, x-xinc, y+yinc+1); // 6 - 8 double
		break;
		case 4:
			drawLine(x-xinc, y+yinc, x-xinc, y-yinc+1); // 8 - 10
		break;
		case 5:
			drawLine(x-xinc, y-yinc, x, y-height); // 10 - 12
		break;
	}
}

void drawHexagonBorders(int area, int f, boolean drawAll)
{
	int row = (f/FIELD_X);
	int rowAdd = 0;
	if(row%2==0) rowAdd = -1;
	
	if(drawAll) if(f<FIELD_X||(f%FIELD_X==FIELD_X-1&&rowAdd==0)             || (f>=FIELD_X           && game[LOCAL].fieldArea[f-FIELD_X+rowAdd+1]!=area)) drawHexagonLine(f, 0);
	if(drawAll) if(f%FIELD_X==FIELD_X-1                                     || (f%FIELD_X<FIELD_X    && game[LOCAL].fieldArea[f+1]               !=area)) drawHexagonLine(f, 1);
	            if(f>=FIELD_SIZE-FIELD_X||(f%FIELD_X==FIELD_X-1&&rowAdd==0) || (f<FIELD_SIZE-FIELD_X && game[LOCAL].fieldArea[f+FIELD_X+rowAdd+1]!=area)) drawHexagonLine(f, 2);
	            if(f>=FIELD_SIZE-FIELD_X||(f%FIELD_X==0&&rowAdd==-1)        || (f<FIELD_SIZE-FIELD_X && game[LOCAL].fieldArea[f+FIELD_X+rowAdd]  !=area)) drawHexagonLine(f, 3);
	if(drawAll) if(f%FIELD_X==0                                             || (f%FIELD_X>0          && game[LOCAL].fieldArea[f-1]               !=area)) drawHexagonLine(f, 4);
	if(drawAll) if(f<FIELD_X||(f%FIELD_X==0&&rowAdd==-1)                    || (f>=FIELD_X           && game[LOCAL].fieldArea[f-FIELD_X+rowAdd]  !=area)) drawHexagonLine(f, 5);
}                                                                                                                                                                 
                                                                                                                                                                     
void fieldPaint()
{
	int i,j,counter;

	if(gameFieldUpdated||(networkMode==NETWORK_MODE_LOBBY&&gameState==GAME_STATE_INGAME_END))
	{
		drawImage(IMAGE_GAME_BG_INDEX, 0, 0, ALIGN_LT);

		counter=0;
		for(int y=0;y<FIELD_Y;y++)
		{
			for(int x=0;x<FIELD_X;x++)
			{
				i = game[LOCAL].fieldArea[x+y*FIELD_X];
				if(i>FIELD_NONE)
				{
					int xAdd = 0;
					if(y%2==1) xAdd += hexagonWidth>>1;

					if(i==curArea||(i==attackArea&&(attackAreaShown||game[LOCAL].playerType[game[LOCAL].curPlayer]==PLAYER_TYPE_LOCAL)))
					{
						//// 1.0 draw hexagon bottom borders * as many as needed - 1
						setColor(0);
						for(j=0;j<(i==curArea?2:1);j++)
						{
							drawHexagonBorders(i, counter, false);
							yPos -= 2;
						}
					}

					//// 1.1 fill hexagon 
					setColor(0xA0A0A0);
					if(!game[LOCAL].playerSurrended[game[LOCAL].playerArea[i]]&&tempAreaDice[i]>0)
						setColor(playerColor[game[LOCAL].playerColor[game[LOCAL].playerArea[i]]]);
					fillHexagon(xAnchor+xPos+x*hexagonWidth+hexagonWidth+xAdd, yAnchor+yPos+y*hexagonHeight+hexagonHeight, hexagonWidth>>1, ((hexagonHeight>>1)+(hexagonHeight>>2)), true);

					//// 1.2 draw hexagon borders
					setColor(0);
					drawHexagonBorders(i, counter, true);
					yPos = 0; // reset y position
				}					

				// draw dice for the previous tile
				for(j=0;j<NUM_AREAS;j++) { if(game[LOCAL].diceIndex[j]==counter-1) { drawAreaDice(j); } }
				counter++;
			}
		}
		gameFieldUpdated = false;
	}

	if((networkMode==NETWORK_MODE_TCP||networkMode==NETWORK_MODE_LOBBY)&&
	   game[LOCAL].curPlayer==playerId&&game[LOCAL].gameTime-game[LOCAL].stateTimer>12000&&gameState!=GAME_STATE_INGAME_END) // show turn timer
	{
		setColor(0);
		drawRect(2, 2, X_RES-4, 8);
		int turnTimer = (game[LOCAL].gameTime-game[LOCAL].stateTimer-11500)/60; // -1500)/80
		setColor(0xffffff);
		fillRect(3, 3, X_RES-6, 6);
		setColor(playerColor[game[LOCAL].playerColor[game[LOCAL].curPlayer]]);
		fillRect(3, 3, X_RES-6-turnTimer, 6);
	}

	if(gameState == GAME_STATE_INGAME_END)
	{
		if(game[LOCAL].gameTime-game[LOCAL].stateTimer>200)
		{
			drawImage(IMAGE_NUMBERS_BOX_INDEX, X_CEN, Y_CEN, ALIGN_HCVC);
	
			if(game[LOCAL].gameTime-game[LOCAL].stateTimer<600)
				setColor(((game[LOCAL].gameTime>>6)%2?MENU_COLOR_ACTIVE:MENU_COLOR_HIGHLIGHT));
			else
				setColor(MENU_COLOR_HIGHLIGHT);
			if(((networkMode==NETWORK_MODE_TCP||networkMode==NETWORK_MODE_LOBBY)&&playerWon==playerId)||
			   (networkMode==NETWORK_MODE_LOCAL&&game[LOCAL].playerType[game[LOCAL].curPlayer]==PLAYER_TYPE_LOCAL)) // hotseat mode - human player means always a win!
				drawString(FONT_TRUETYPE, "YOU WON!", X_CEN, Y_CEN-(FONT_TRUETYPE_HEIGHT>>1), ALIGN_HCT);
			else
				drawString(FONT_TRUETYPE, "YOU LOST!", X_CEN, Y_CEN-(FONT_TRUETYPE_HEIGHT>>1), ALIGN_HCT);
		}
	}
	else if(gameState == GAME_STATE_INGAME_POST_ATTACK)
	{
		if(game[LOCAL].gameTime-game[LOCAL].stateTimer<1500&&attackAreaShown)
		{
			drawImage(IMAGE_DICE_BOX_INDEX, 7, 4, ALIGN_LT);
			drawImage(IMAGE_DICE_BOX_INDEX, X_RES-7, 4, ALIGN_RT);
	
			if(game[LOCAL].gameTime-game[LOCAL].stateTimer>500)
			{
				gameFieldUpdated = true; // TODO - this redraws everything all the time
				for(i=0;i<8;i++)
				{
					if(playerDice[0][i]>0)
						drawSubImage(IMAGE_DICE_INDEX, 8, 6+i*22, ALIGN_LT, 24, 24, 24*game[LOCAL].playerColor[game[LOCAL].playerArea[curArea]], (6-playerDice[0][i])*24);
					if(playerDice[1][i]>0)
						drawSubImage(IMAGE_DICE_INDEX, X_RES-32, 6+i*22, ALIGN_LT, 24, 24, 24*game[LOCAL].playerColor[game[LOCAL].playerArea[attackArea]], (6-playerDice[1][i])*24);
				}
				int yLoc = Y_RES-(IMAGE_NUMBERS_BOX_HEIGHT>>1)-5; // Y_CEN;
				drawImage(IMAGE_NUMBERS_BOX_INDEX, X_CEN, yLoc, ALIGN_HCVC);
				int xAdd=0;
				if(playerTotal[0]<10)
					xAdd -= 11;
				if(playerTotal[1]<10)
					xAdd += 11;
				drawSubImage(IMAGE_NUMBERS_INDEX, X_CEN+xAdd, yLoc, ALIGN_HCVC, 26, 36, 260, 0); // dash
				drawSubImage(IMAGE_NUMBERS_INDEX, X_CEN+xAdd-22, yLoc, ALIGN_HCVC, 26, 36, (playerTotal[0]%10)*26, 0); // ones, attacker
				if(playerTotal[0]>=10)
					drawSubImage(IMAGE_NUMBERS_INDEX, X_CEN+xAdd-44, yLoc, ALIGN_HCVC, 26, 36, (playerTotal[0]/10)*26, 0); // tens, attacker
				if(playerTotal[1]>=10)
				{
					drawSubImage(IMAGE_NUMBERS_INDEX, X_CEN+xAdd+44, yLoc, ALIGN_HCVC, 26, 36, (playerTotal[1]%10)*26, 0); // ones, defender
					drawSubImage(IMAGE_NUMBERS_INDEX, X_CEN+xAdd+22, yLoc, ALIGN_HCVC, 26, 36, (playerTotal[1]/10)*26, 0); // tens, defender
				}
				else
					drawSubImage(IMAGE_NUMBERS_INDEX, X_CEN+xAdd+22, yLoc, ALIGN_HCVC, 26, 36, (playerTotal[1]%10)*26, 0); // ones, defender
			}
			else
			{
				for(i=0;i<8;i++)
				{
					if(playerDice[0][i]>0)
						drawSubImage(IMAGE_DICE_INDEX, 8, 6+i*22, ALIGN_LT, 24, 24, 24*game[LOCAL].playerColor[game[LOCAL].playerArea[curArea]], (rand()%6)*24);
					if(playerDice[1][i]>0)
						drawSubImage(IMAGE_DICE_INDEX, X_RES-32, 6+i*22, ALIGN_LT, 24, 24, 24*game[LOCAL].playerColor[game[LOCAL].playerArea[attackArea]], (rand()%6)*24);
				}
			}
		}
	}

	if(gameState==GAME_STATE_INGAME_POST_STATUS&&!firstTurn) // draw dice on the bottom left corner while adding
	{
		for(i=0;i<dicePilingTotal-tempAreaCounter;i++) // +game[LOCAL].diceReserve[tempPlayer];i++)
		{
//			logPrintf("amount: %d, total: %d, counter: %d, reserve: %d, tempPlayer: %d\n", 
//			         (dicePilingTotal-tempAreaCounter+game[LOCAL].diceReserve[tempPlayer]), dicePilingTotal, tempAreaCounter, game[LOCAL].diceReserve[tempPlayer], tempPlayer);
			if(i>=56) break; // draw max. 2 rows of dice
			if(i>=28) // (X_RES-4)/9)
				drawSubImage(IMAGE_DICE_SMALL_INDEX, 2+(i-28)*9, Y_RES-12, ALIGN_LB, 9, 9, game[LOCAL].playerColor[tempPlayer]*9, 0);
			else
				drawSubImage(IMAGE_DICE_SMALL_INDEX, 2+i*9, Y_RES-2, ALIGN_LB, 9, 9, game[LOCAL].playerColor[tempPlayer]*9, 0);
		}
	}

	if((gameState==GAME_STATE_INGAME&&!endTurnPressed)||gameState==GAME_STATE_INGAME_END)
	{
		drawSubImage(IMAGE_ENDTURN_INDEX, X_RES-1, Y_RES-2, ALIGN_RB, 30, 33, (1-endTurnHover)*30, 0);
	}
	if(gameState==GAME_STATE_INGAME&&game[LOCAL].curTurn>=3) // can't surrender before 3 turns have passed
	{
		drawSubImage(IMAGE_SURRENDER_INDEX, 1, Y_RES-2, ALIGN_LB, 30, 33, (1-surrenderHover)*30, 0);
		if(surrenderBox)
		{
			drawImage(IMAGE_NUMBERS_BOX_INDEX, X_CEN, Y_CEN, ALIGN_HCVC);
			setColor(0xffffff);
			drawString(FONT_TRUETYPE, "Surrender?", X_CEN, Y_CEN-16, ALIGN_HCT);
			if(menuHoverLeft) setColor(MENU_COLOR_ACTIVE);
			else setColor(0xffffff);
			drawString(FONT_TRUETYPE, "Yes", X_CEN-22, Y_CEN+4, ALIGN_HCT);
			if(menuHoverRight) setColor(MENU_COLOR_ACTIVE);
			else setColor(0xffffff);
			drawString(FONT_TRUETYPE, "No", X_CEN+22, Y_CEN+4, ALIGN_HCT);
		}
	}
	
	if(gameState==GAME_STATE_INGAME_MENU)
	{
		drawImage(IMAGE_NUMBERS_BOX_INDEX, X_CEN, Y_CEN, ALIGN_HCVC);
		setColor(0xffffff);
		drawString(FONT_TRUETYPE, "End game?", X_CEN, Y_CEN-16, ALIGN_HCT);
		if(menuHoverLeft) setColor(MENU_COLOR_ACTIVE);
		else setColor(0xffffff);
		drawString(FONT_TRUETYPE, "Yes", X_CEN-22, Y_CEN+4, ALIGN_HCT);
		if(menuHoverRight) setColor(MENU_COLOR_ACTIVE);
		else setColor(0xffffff);
		drawString(FONT_TRUETYPE, "No", X_CEN+22, Y_CEN+4, ALIGN_HCT);
	}
}

void gamePaint()
{
	#ifndef DEBUGSCREEN
	int i;
	if(playerDataUpdated) // PLAYER STATISTICS SCREEN
	{
		setActiveScreen(SCREEN_UPPER);
		drawImage(IMAGE_DATA_BG_INDEX, 0, 0, ALIGN_LT);

		setColor(0);
		drawString(FONT_FIXED, "PLAYER", 20, 4, ALIGN_LT);		
		drawString(FONT_FIXED, "PLAYER", 21, 4, ALIGN_LT);		
		drawString(FONT_FIXED, "pos.", 153-3, 4, ALIGN_HCVC);
		drawString(FONT_FIXED, "pos.", 154-3, 4, ALIGN_HCVC);
		drawImage(IMAGE_HEXAGON_ICON_INDEX, 180-4, 9, ALIGN_HCVC);
		drawImage(IMAGE_DICE_ICON_INDEX, 205-4, 9, ALIGN_HCVC);
/*		#ifdef DEBUG
		drawString(FONT_FIXED, "AI", 210, 4, ALIGN_LT);		
		drawString(FONT_FIXED, "AI", 211, 4, ALIGN_LT);		
		#endif
*/
		if(networkMode==NETWORK_MODE_TCP)
		{
			drawString(FONT_FIXED, "score", 219, 4, ALIGN_LT);
			drawString(FONT_FIXED, "score", 220, 4, ALIGN_LT);		
		}

		for(i=0;i<game[LOCAL].numPlayers;i++)
		{
			if(game[LOCAL].playerStatus[i]==PLAYER_STATUS_DEAD||game[LOCAL].playerSurrended[i])
				setColor(MENU_COLOR_INACTIVE);
			else if(game[LOCAL].playerStatus[i]==PLAYER_STATUS_QUIT||game[LOCAL].playerStatus[i]==PLAYER_STATUS_DROPPED)
				setColor(MENU_COLOR_INACTIVE); // could have different color for dropped and dead players?
			else
				setColor(0);
			if(i==game[LOCAL].curPlayer)
				drawImage(IMAGE_SELECTED_INDEX, 6, 16+i*(FONT_FIXED_HEIGHT+1), ALIGN_LT);
			if(game[LOCAL].playerType[i]==PLAYER_TYPE_CPU)
				drawSubImage(IMAGE_DICE_SMALL_INDEX, 8, 17+i*(FONT_FIXED_HEIGHT+1), ALIGN_LT, 9, 9, game[LOCAL].playerColor[i]*9, 0);
			else // draw a player image
				drawSubImage(IMAGE_HEADS_SMALL_INDEX, 8, 17+i*(FONT_FIXED_HEIGHT+1), ALIGN_LT, 9, 9, game[LOCAL].playerColor[i]*9, 0);
			drawString(FONT_FIXED, game[LOCAL].playerName[i], 20, 16+i*(FONT_FIXED_HEIGHT+1), ALIGN_LT);
			drawString(FONT_FIXED, positionName[game[LOCAL].playerPosition[i]], 152-3, 16+i*(FONT_FIXED_HEIGHT+1), ALIGN_HCT);
			drawString(FONT_FIXED, int2str(game[LOCAL].playerAreaCount[i]), 180-3, 16+i*(FONT_FIXED_HEIGHT+1), ALIGN_HCT);
			drawString(FONT_FIXED, int2str(game[LOCAL].playerDiceCount[i]), 205-3, 16+i*(FONT_FIXED_HEIGHT+1), ALIGN_HCT);
			if(game[LOCAL].diceReserve[i]>0)
			{
				char str2[4]; sprintf(str2, "+%d", game[LOCAL].diceReserve[i]);
				drawString(FONT_FIXED, str2, 205+(stringWidth(FONT_FIXED, int2str(game[LOCAL].playerDiceCount[i]))>>1)-3, 16+i*(FONT_FIXED_HEIGHT+1), ALIGN_LT);
			}
			if(networkMode==NETWORK_MODE_TCP)
				drawString(FONT_FIXED, int2str(game[LOCAL].gameScore[i]), 232, 16+i*(FONT_FIXED_HEIGHT+1), ALIGN_HCT);

			/*#ifdef DEBUG
			drawString(FONT_FIXED, int2str(game[LOCAL].AIattackPlayer[i]), 215, 16+i*(FONT_FIXED_HEIGHT+1), ALIGN_HCT);
			#endif*/
		}
		/*#ifdef DEBUG
		drawString(FONT_FIXED, (game[LOCAL].AIattackAll?"All":"Selected"), 215, 16+i*(FONT_FIXED_HEIGHT+1), ALIGN_HCT);
		#endif*/

		setColor(0);
		for(i=0;i<5;i++)
		{
			if(gameHistory[(gameHistoryCount+i)%GAME_HISTORY_LENGTH]!=NULL)
				drawString(FONT_FIXED, gameHistory[(gameHistoryCount+i-5)%GAME_HISTORY_LENGTH], 12, (Y_RES>>1)+(Y_RES>>3)+2+i*(FONT_FIXED_HEIGHT+1), ALIGN_LT);
		}
		#ifdef DEBUG
		setColor(0x808080);
		for(i=0;i<5;i++)
		{
			if(debugHistory[(debugHistoryCount+i)%GAME_HISTORY_LENGTH]!=NULL)
				drawString(FONT_FIXED, debugHistory[(debugHistoryCount+i-5)%GAME_HISTORY_LENGTH], X_RES-12, (Y_RES>>1)+(Y_RES>>3)+2+i*(FONT_FIXED_HEIGHT+1), ALIGN_RT);
		}
		#endif
		
		/*setColor(0);
		char xxx[32]; sprintf(xxx, "xMin: %d, xMax: %d, xAnchor: %d", xMin, xMax, xAnchor);
		drawString(FONT_FIXED, xxx, 10, 87, ALIGN_LT);
		char yyy[32]; sprintf(yyy, "yMin: %d, yMax: %d, yAnchor: %d", yMin, yMax, yAnchor);
		drawString(FONT_FIXED, yyy, 10, 98, ALIGN_LT);*/
		setActiveScreen(SCREEN_LOWER);	

		playerDataUpdated = false;
	}
	#endif
	
	fieldPaint();
	
/*	#ifdef DEBUG
	setColor(0);
	char str[128];
	sprintf(str, "field id: %d", game[LOCAL].gameField);
	drawString(FONT_FIXED, str, 30, Y_RES-10, ALIGN_LT);
	#endif
	*/
}

void connectionStatusPaint()
{
	if(networkMode!=NETWORK_MODE_LOCAL&&networkStatus>NETWORK_STATUS_CONNECTED)
	{
		drawImage(IMAGE_NUMBERS_BOX_INDEX, X_CEN, Y_CEN, ALIGN_HCVC);
		setColor(MENU_COLOR_ERROR);
		if(networkStatus==NETWORK_STATUS_CONNECTION_LOST)
			drawString(FONT_TRUETYPE, "Connection lost!", X_CEN, Y_CEN-(FONT_TRUETYPE_HEIGHT>>1), ALIGN_HCT);
		else if(networkStatus==NETWORK_STATUS_CONNECTING)
		{
			setColor(MENU_COLOR_HIGHLIGHT);
			char str[32];
			#if defined DEBUG | defined DEBUGSCREEN
			sprintf(str, "Connecting %d.", networkInitCounter);
			#else
			sprintf(str, "Connecting.");
			#endif
			if((getTime()>>8)%4==0)
				strcat(str, ".");
			else if((getTime()>>8)%4==1)
				strcat(str, "..");
			else if((getTime()>>8)%4==2)
				strcat(str, "...");
			#if defined DEBUG | defined DEBUGSCREEN
				drawString(FONT_TRUETYPE, str, X_CEN-54, Y_CEN-(FONT_TRUETYPE_HEIGHT>>1), ALIGN_LT);					
			#else
				drawString(FONT_TRUETYPE, str, X_CEN-50, Y_CEN-(FONT_TRUETYPE_HEIGHT>>1), ALIGN_LT);					
			#endif
		}
		else if(networkStatus==NETWORK_STATUS_WRONG_VERSION)
		{
			char str[32]; sprintf(str, "Version %d.%d.%d", requiredVersionMajor, requiredVersionMinor, requiredVersionRevision);
			drawString(FONT_TRUETYPE, str, X_CEN, Y_CEN-(FONT_TRUETYPE_HEIGHT>>1)-7, ALIGN_HCT);
			drawString(FONT_TRUETYPE, "required!", X_CEN, Y_CEN-(FONT_TRUETYPE_HEIGHT>>1)+10, ALIGN_HCT);
		}
		else
		{
			drawString(FONT_TRUETYPE, "Connection", X_CEN, Y_CEN-(FONT_TRUETYPE_HEIGHT>>1)-7, ALIGN_HCT);
			drawString(FONT_TRUETYPE, "failed!", X_CEN, Y_CEN-(FONT_TRUETYPE_HEIGHT>>1)+10, ALIGN_HCT);
		}
	}
}

char *timeStampToText(int timeStamp)
{
	char *str = (char*)malloc(32);
	if(timeStamp<60)
		sprintf(str, "%d minutes", timeStamp);
	else if(timeStamp<1440)
		sprintf(str, "%d:%02d hours", timeStamp/60, timeStamp%60);
	else if(timeStamp<2880)
		sprintf(str, "%d day %d:%02d hours", timeStamp/1440, (timeStamp/60)%24, timeStamp%60);
	else
		sprintf(str, "%d days %d:%02d hours", timeStamp/1440, (timeStamp/60)%24, timeStamp%60);
	return str;
}

void paint()
{
	if(gameState==GAME_STATE_KEYBOARD)
	{
		kbdPaint();
	}
	else if(gameState==GAME_STATE_MENU)
	{
		#ifndef DEBUGSCREEN
		if(!menuUpperDrawn)
		{
			setActiveScreen(SCREEN_UPPER);
			if(networkConnected&&(currentMenu==MENU_NETWORK_LOBBY_INDEX||currentMenu==MENU_NETWORK_NEW_GAME_INDEX||currentMenu==MENU_NETWORK_JOIN_GAME_INDEX))
			{
				drawImage(IMAGE_INSTRUCTIONS_BG_INDEX, 0, 0, ALIGN_LT);
				setColor(MENU_COLOR_ACTIVE);
				drawString(FONT_TRUETYPE, "NETWORK GAME LOBBY", X_CEN, 20, ALIGN_HCT);
				setColor(0);
				char str[64]; 
			
				if(currentMenu==MENU_NETWORK_LOBBY_INDEX)
				{
					for(int i=0;i<MENU_LOBBY_INSTRUCTIONS_LENGTH;i++)
					{
						if(i==2)
						{
							sprintf(str, "(%d) have joined to the game. Players with", game[LOCAL].minPlayers);
							drawString(FONT_FIXED, str, 22, 40+FONT_FIXED_HEIGHT*i, ALIGN_LT);
						}
						else if(i==6)
						{
							sprintf(str, "number of players (%d) have joined.", game[LOCAL].maxPlayers);
							drawString(FONT_FIXED, str, 22, 40+FONT_FIXED_HEIGHT*i, ALIGN_LT);
						}
						else
							drawString(FONT_FIXED, menuLobbyInstructionsTexts[i], 23, 40+FONT_FIXED_HEIGHT*i, ALIGN_LT);
					}
				}
				else if(currentMenu==MENU_NETWORK_NEW_GAME_INDEX)
				{
					for(int i=0;i<MENU_NEW_GAME_INSTRUCTIONS_LENGTH;i++)
					{
						drawString(FONT_FIXED, menuNewGameInstructionsTexts[i], 23, 40+FONT_FIXED_HEIGHT*i, ALIGN_LT);
					}
				}
				else if(currentMenu==MENU_NETWORK_JOIN_GAME_INDEX)
				{
					for(int i=0;i<MENU_JOIN_GAME_INSTRUCTIONS_LENGTH;i++)
					{
						drawString(FONT_FIXED, menuJoinGameInstructionsTexts[i], 23, 40+FONT_FIXED_HEIGHT*i, ALIGN_LT);
					}
				}
				memset(str, 0, 64); sprintf(str, "Online players: %d, ongoing games: %d", gameStatistics.playersOnline, gameStatistics.activeGames);
				drawString(FONT_FIXED, str, 22, 40+FONT_FIXED_HEIGHT*(MENU_LOBBY_INSTRUCTIONS_LENGTH+1), ALIGN_LT);
				memset(str, 0, 64); sprintf(str, "Last connection: %s ago", timeStampToText(gameStatistics.lastPlayerConnected));
				drawString(FONT_FIXED, str, 22, 40+FONT_FIXED_HEIGHT*(MENU_LOBBY_INSTRUCTIONS_LENGTH+2), ALIGN_LT);
				memset(str, 0, 64); sprintf(str, "Last game played: %s ago", timeStampToText(gameStatistics.lastGameEnded));
				drawString(FONT_FIXED, str, 22, 40+FONT_FIXED_HEIGHT*(MENU_LOBBY_INSTRUCTIONS_LENGTH+3), ALIGN_LT);
			}
			else
			{
				drawImage(IMAGE_LOGO_INDEX, 0, 0, ALIGN_LT);
				setColor(0);
				char ver[16]; sprintf(ver, "v%s", DICEWARS_VERSION);
			  drawString(FONT_FIXED, ver, 1, Y_RES, ALIGN_LB);
			}
			menuUpperDrawn = true;
			setActiveScreen(SCREEN_LOWER);
		}
		#endif
		#ifdef DEBUG
		setActiveScreen(SCREEN_UPPER);
		if(debugHistoryCount!=debugDrawCount)
		{		
			if(currentMenu==MENU_NETWORK_LOBBY_INDEX||currentMenu==MENU_NETWORK_JOIN_GAME_INDEX||currentMenu==MENU_NETWORK_NEW_GAME_INDEX)
				drawSubImage(IMAGE_INSTRUCTIONS_BG_INDEX, 130, 120, ALIGN_LT, 126, 60, 130, 120);
			else
				drawSubImage(IMAGE_LOGO_INDEX, 130, 120, ALIGN_LT, 126, 60, 130, 120);
			setColor(0);
			for(int i=0;i<5;i++)
			{
				if(debugHistory[(debugHistoryCount+i)%GAME_HISTORY_LENGTH]!=NULL)
					drawString(FONT_FIXED, debugHistory[(debugHistoryCount+i-5)%GAME_HISTORY_LENGTH], X_RES-12, (Y_RES>>1)+(Y_RES>>3)+2+i*(FONT_FIXED_HEIGHT+1), ALIGN_RT);
			}
			debugDrawCount = debugHistoryCount;
		}
		
/*		drawSubImage(IMAGE_LOGO_INDEX, 0, 120, ALIGN_LT, 80, 60, 0, 120);
		char nwC[6]; sprintf(nwC, "NW: %d", networkConnected);
		drawString(FONT_FIXED, nwC, 2, 120, ALIGN_LT);
		char nwS[16]; sprintf(nwS, "STATUS: %d", networkStatus);
		drawString(FONT_FIXED, nwS, 2, 130, ALIGN_LT);
		char nwI[16]; sprintf(nwI, "COUNTER: %d", networkInitCounter);
		drawString(FONT_FIXED, nwI, 2, 140, ALIGN_LT);
		#ifdef LOBBY
			int max = LOBBY_GetNumberOfKnownUsers();
			for(int i=0;i<max;i++)
			{
				LPLOBBY_USER user = LOBBY_GetUserByID(i) ;
				char jees[32]; sprintf(jees, "user: %s", LOBBY_GetUserName(user));
				drawString(FONT_FIXED, jees, 2, 150+i*10, ALIGN_LT);
			}
		#endif*/
		setActiveScreen(SCREEN_LOWER);
		#endif
		menuPaint(currentMenu);
	}
	else /*if(gameState==GAME_STATE_INGAME_START)
	{
		printf("game state start paint()\n");
		setColor(0);
		fillRect(0,0,X_RES,Y_RES);
		drawString(FONT_TRUETYPE, "Starting a new game...", X_CEN, Y_CEN, ALIGN_HCT);
	}
	else*/
		gamePaint();

	if(gameState!=GAME_STATE_MENU||currentMenu==MENU_NETWORK_NEW_GAME_INDEX||currentMenu==MENU_NETWORK_JOIN_GAME_INDEX||currentMenu==MENU_NETWORK_LOBBY_INDEX)
		connectionStatusPaint();
		
	#if defined DEBUG | defined DEBUGSCREEN
/*		setColor(0);
		fillRect(2, Y_RES-6, 4, 4);
		setColor(0xffffff);
		int x = (getTime()>>6)%4;
		if(x>1) x = 3-x;
		drawRect(x+3, Y_RES-4-((getTime()>>7)%2), 1, 1);*/
	#endif
}

