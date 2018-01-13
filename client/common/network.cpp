int byte2Signed(byte data)
{
	if(data>=128)
		return -(256-data);
	else
		return data;
}

void sendClientData(byte *sendBuffer)
{
  int n = NETWORK_MAX_DATA+1;

	if(networkMode==NETWORK_MODE_TCP) // send to server
	{
		if(networkConnected||networkStatus==NETWORK_STATUS_CONNECTING||networkStatus==NETWORK_STATUS_CONNECTED)
		{
		  if (send(client_sock,(const char*)sendBuffer,n,0) != n)
		  {
		    logPrintf("send error: %s\n", strerror(errno));
		  	networkStatus = NETWORK_STATUS_CONNECTION_LOST;
				reconnectNetwork = true;
				resetNetworkConnection();
		  }
/*		  else 
		  {
		    logPrintf(">>> %d:%d (len: %d)\n", sendBuffer[0], sendBuffer[1], n);
		  }*/
		}
		else
		{
			logPrintf("!!! Trying to send network message while not connected to server: %d.%d, status: %d.\n", sendBuffer[0], sendBuffer[1], networkStatus);
		}
	}
	#ifdef LOBBY
	else if(networkMode==NETWORK_MODE_LOBBY) // ad-hoc networking
	{
		if(roomOwner!=NULL&&roomOwner!=pthread_self()) // client -> server
		{
			LOBBY_SendToUser(roomOwner, 0x0001, sendBuffer, NETWORK_MAX_DATA+1);
		}
		else if(roomOwner!=NULL) // server -> server
		{
	  	clientMessage(pthread_self(), sendBuffer[0], sendBuffer+1);
		}
		else // not in any room, skip sending messages
		{
			// TODO
		}
	}
	#endif
	else // bypass network layer
	{
//		logPrintf("Local mode.. Bypass network layer\n");
  	clientMessage(pthread_self(), sendBuffer[0], sendBuffer+1);
  }
}

void receiveServerData()
{
	int i,j;
  int n = NETWORK_MAX_DATA;
	
  if(networkMode==NETWORK_MODE_TCP) // receive data from remote server
  {
	  memset(receiveBuffer, 0, NETWORK_MAX_DATA);
		// this is terribly wrong, isn't it? but it just seems to work...
		#ifndef NDS
		ioctl(client_sock,FIONBIO,&bMode); // set blocking
		#endif
		n = recv(client_sock, (char*)receiveBuffer, NETWORK_MAX_DATA, 0);
		#ifndef NDS
		ioctl(client_sock,FIONBIO,&nbMode); // set non-blocking
		#endif
	  if (n < 0)
	  { 
	  	// EAGAIN appears at least on DS
//	    logPrintf("readline error: %s\n", strerror(errno));
	  }
	  else if(n==0) // closed socket from the other end
	  {
	  	networkStatus = NETWORK_STATUS_CONNECTION_LOST;
			reconnectNetwork = true;
			logPrintf("!!! SERVER CLOSED SOCKET ?!?\n");
			resetNetworkConnection();
	  }
	}
	if(n>0)
  {
  	logPrintf(">>> (len: %d) ", n);
	  switch(receiveBuffer[0])
	  {
	    case 0: 
	      logPrintf("UNDEFINED / FALSE MESSAGE???\n");
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "!!! UNDEFINED");
				#endif
	      break;
	    case MESSAGE_INIT_CONNECTION:
	      logPrintf("MESSAGE_INIT_CONNECTION\n");
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< CONNECT");
				#endif
		  	networkConnected = true;
			 	networkStatus = NETWORK_STATUS_CONNECTED;
			 	menuUpdated = true;
	    	break;
	    case MESSAGE_LIST_OPEN_GAMES:
	      logPrintf("LIST_OPEN_GAMES: %d\n", receiveBuffer[1]);
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< LIST_OPEN_GAMES");
				#endif
	      numOpenGames = receiveBuffer[1];
	      for(i=0;i<receiveBuffer[1];i++)
	      {
	      	openGameIds[i] = receiveBuffer[2+i*(6+MAX_NAME_LENGTH)];
	      	openGameStatus[i] = receiveBuffer[3+i*(6+MAX_NAME_LENGTH)];
	      	if(openGameStatus[i]==GAME_STATUS_PLAYING)
	      		logPrintf("!!! REJOIN ???\n");
		      openGamePlayers[i] = receiveBuffer[4+i*(6+MAX_NAME_LENGTH)];
		      openGameMinPlayers[i] = receiveBuffer[5+i*(6+MAX_NAME_LENGTH)];
		      openGameMaxPlayers[i] = receiveBuffer[6+i*(6+MAX_NAME_LENGTH)];
		      openGameFieldIds[i] = receiveBuffer[7+i*(6+MAX_NAME_LENGTH)];
	      	logPrintf("game[%d]: ", i);
	      	for(j=0;j<MAX_NAME_LENGTH;j++)
	      	{
	      		openGames[i][j] = receiveBuffer[8+i*(6+MAX_NAME_LENGTH)+j];
	      		if(openGames[i][j]>0x10)
		      		printf("%c", openGames[i][j]);
	      	}
	      	memset(openGameNames[i], 0, MAX_NAME_LENGTH+8);
	      	sprintf(openGameNames[i], "%s    %d/%d", openGames[i], openGamePlayers[i], openGameMaxPlayers[i]);
	      	printf("\n");
	      	menuUpdated = true;
	      }
	      logPrintf("open games: %d\n", numOpenGames);
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< LIST");
				#endif
	      break;
	    case MESSAGE_PLAYER_JOINED:
	      logPrintf("PLAYER_JOINED, game %d, player: %d\n", receiveBuffer[1], receiveBuffer[2]);
	      gameId = receiveBuffer[1];
      	game[LOCAL].playerStatus[receiveBuffer[2]]=PLAYER_STATUS_WAITING;
	      game[LOCAL].playerColor[receiveBuffer[2]]=receiveBuffer[3];
	      for(j=0;j<MAX_NAME_LENGTH;j++)
	      {
	      	game[LOCAL].playerName[receiveBuffer[2]][j] = 0; // reset the name fully, just to be sure
      		if(receiveBuffer[4+j]>=0x10)
      		{
	      		game[LOCAL].playerName[receiveBuffer[2]][j] = receiveBuffer[4+j];
		      	printf("%c", game[LOCAL].playerName[receiveBuffer[2]][j]);
		      }
      	}
      	printf("\n");

				if(game[LOCAL].gameStatus==GAME_STATUS_PLAYING)
				{
					sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], "* Player %s joined", game[LOCAL].playerName[receiveBuffer[2]]);
					playerDataUpdated = true;
				}
				else
				{
					if(networkMode!=NETWORK_MODE_LOBBY||roomOwner||roomOwner!=pthread_self())
	      		game[LOCAL].numPlayers++;
	      	logPrintf("numPlayers now: %d\n", game[LOCAL].numPlayers);
				  #ifdef LOBBY
						#ifdef DEBUG
						sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! playerid1: %d", playerId);
						#endif
					#endif
	      	if(game[LOCAL].numPlayers==1) // this is us
			      playerId = receiveBuffer[2];
	      	logPrintf("playerid: %d\n", playerId);
				  #ifdef LOBBY
						#ifdef DEBUG
						sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! playerid2: %d", playerId);
						#endif
					#endif
	      }
			  #ifdef LOBBY
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! numplayers2: %d", game[LOCAL].numPlayers);
					#endif
				#endif
				
      	menuUpdated = playerDataUpdated = gameFieldUpdated = true;
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< %d.%d JOINED", receiveBuffer[1], receiveBuffer[2]);
				#endif
	      break;
	    case MESSAGE_PLAYER_QUIT:
	      logPrintf("PLAYER_QUIT: %d\n", receiveBuffer[1]);
      	game[LOCAL].playerStatus[receiveBuffer[1]]=PLAYER_STATUS_QUIT;
	      if(game[LOCAL].gameStatus<GAME_STATUS_PLAYING) // erase player name
	      {
		      for(j=0;j<MAX_NAME_LENGTH;j++)
		      {
	      		game[LOCAL].playerName[receiveBuffer[1]][j] = receiveBuffer[2+j];
	      	}
					if(networkMode!=NETWORK_MODE_LOBBY||roomOwner||roomOwner!=pthread_self())
			      game[LOCAL].numPlayers--;
		      for(j=0;j<MAX_PLAYERS;j++)
		      {
		      	logPrintf("player %d: %d (%s)\n", j, game[LOCAL].playerStatus[j], game[LOCAL].playerName[j]);
		      }
	      }
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], "* Player %s quit", game[LOCAL].playerName[receiveBuffer[1]]);
      	playerDataUpdated = true;
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< %d QUIT", receiveBuffer[1]);
				#endif
	      break;
	    case MESSAGE_PLAYER_SURRENDED:
	      logPrintf("PLAYER_SURRENDED: %d\n", receiveBuffer[1]);
	      game[LOCAL].playerSurrended[receiveBuffer[1]] = true;
	      if(game[LOCAL].playerStatus[receiveBuffer[1]]>=PLAYER_STATUS_QUIT) // automatical surrender
					sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> %s surrended due to inactivity! <<", game[LOCAL].playerName[receiveBuffer[1]]);
	      else
					sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> Player %s surrended! <<", game[LOCAL].playerName[receiveBuffer[1]]);
				if(networkMode==NETWORK_MODE_TCP)
				{
					game[LOCAL].playerPosition[receiveBuffer[1]] = receiveBuffer[2];
					game[LOCAL].gameScore[receiveBuffer[1]] = byte2Signed(receiveBuffer[3]);
	      	sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> %s finished %d. with a score of %d <<", game[LOCAL].playerName[receiveBuffer[1]], receiveBuffer[2], game[LOCAL].gameScore[receiveBuffer[1]]);
	      }
      	playerDataUpdated = true;
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< %d SURRENDED", receiveBuffer[1]);
				#endif
	      break;
	    case MESSAGE_PLAYER_DROPPED:
	      logPrintf("PLAYER_DROPPED: %d\n", receiveBuffer[1]);
      	game[LOCAL].playerStatus[receiveBuffer[1]]=PLAYER_STATUS_DROPPED;
	      if(game[LOCAL].gameStatus<GAME_STATUS_PLAYING) // erase player name
	      {
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< GSTATUS: %d !!!", game[LOCAL].gameStatus);
					#endif	      	
		      for(j=0;j<MAX_NAME_LENGTH;j++)
		      {
	      		game[LOCAL].playerName[receiveBuffer[1]][j] = receiveBuffer[2+j];
	      	}
					if(networkMode!=NETWORK_MODE_LOBBY||roomOwner||roomOwner!=pthread_self())
			      game[LOCAL].numPlayers--;
	      }
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], "* Player %s dropped", game[LOCAL].playerName[receiveBuffer[1]]);
				playerDataUpdated = true;
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< %d DROPPED", receiveBuffer[1]);
				#endif
	      break;
	    case MESSAGE_PLAYER_DIED:
	      logPrintf("PLAYER_DIED: %d\n", receiveBuffer[1]);
	      game[LOCAL].playerStatus[receiveBuffer[1]] = PLAYER_STATUS_DEAD;
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> Player %s is dead! <<", game[LOCAL].playerName[receiveBuffer[1]]);
				if(networkMode==NETWORK_MODE_TCP)
				{
					game[LOCAL].playerPosition[receiveBuffer[1]] = receiveBuffer[2];
					game[LOCAL].gameScore[receiveBuffer[1]] = byte2Signed(receiveBuffer[3]);
	      	sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> %s finished %d. with a score of %d <<", game[LOCAL].playerName[receiveBuffer[1]], receiveBuffer[2], game[LOCAL].gameScore[receiveBuffer[1]]);
					game[LOCAL].pot = receiveBuffer[4]+(receiveBuffer[5]<<8);
	      }
//      	playerDataUpdated = true;
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< %d PLAYER DIED", receiveBuffer[1]);
				#endif
	      break;
	    case MESSAGE_PLAYER_STATUS:
	      logPrintf("PLAYER_STATUS (%d): ", receiveBuffer[1]);
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< %d STATUS", receiveBuffer[1]);
				#endif
	      playerId = receiveBuffer[1];
	      game[LOCAL].numPlayers=receiveBuffer[2];
	      for(i=0;i<MAX_PLAYERS;i++) // was: receiveBuffer[2]
	      {
	      	game[LOCAL].playerStatus[receiveBuffer[3+i*(4+MAX_NAME_LENGTH)]] = receiveBuffer[4+i*(4+MAX_NAME_LENGTH)];
	      	game[LOCAL].playerSurrended[receiveBuffer[3+i*(4+MAX_NAME_LENGTH)]] = receiveBuffer[5+i*(4+MAX_NAME_LENGTH)];
		      game[LOCAL].playerColor[receiveBuffer[3+i*(4+MAX_NAME_LENGTH)]] = receiveBuffer[6+i*(4+MAX_NAME_LENGTH)];
	      	for(j=0;j<MAX_NAME_LENGTH;j++)
	      	{
		      	game[LOCAL].playerName[receiveBuffer[3+i*(4+MAX_NAME_LENGTH)]][j] = 0; // reset the name fully, just to be sure
	      		if(receiveBuffer[7+i*(4+MAX_NAME_LENGTH)+j]>=0x20)
	      		{
		      		if(j==0&&i!=0)
		      			printf(", ");
		      		game[LOCAL].playerName[receiveBuffer[3+i*(4+MAX_NAME_LENGTH)]][j] = receiveBuffer[7+i*(4+MAX_NAME_LENGTH)+j];
		      		printf("%c", game[LOCAL].playerName[receiveBuffer[3+i*(4+MAX_NAME_LENGTH)]][j]);
		      	}
	      	}
	      }
	      printf("\n");
	      break;
	    case MESSAGE_READY_TO_START:
	      logPrintf("READY_TO_START: %d\n", receiveBuffer[1]);
	      game[LOCAL].playerStatus[receiveBuffer[1]]=PLAYER_STATUS_READY;
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< %d READY", receiveBuffer[1]);
				#endif
	      break;
			case MESSAGE_START_GAME:
				firstTurn = true;
	      logPrintf("START_GAME: %d\n", receiveBuffer[1]);				
	      game[LOCAL].gameField=receiveBuffer[1];
	      game[LOCAL].numPlayers=receiveBuffer[2];
	      for(i=0;i<game[LOCAL].numPlayers;i++)
	      {
	      	game[LOCAL].playerType[i]=receiveBuffer[3+i];
	      	for(j=0;j<MAX_NAME_LENGTH;j++)
	      	{
	      		game[LOCAL].playerName[i][j] = receiveBuffer[3+MAX_PLAYERS+i*MAX_NAME_LENGTH+j];
	      	}
	      	if(i==playerId) // TODO: is this necessary?
		    		game[LOCAL].playerType[i] = PLAYER_TYPE_LOCAL;
//		    	else
//		    		game[LOCAL].playerType[i] = PLAYER_TYPE_NETWORK;
					game[LOCAL].playerSurrended[i] = false;
	      }
	      #ifdef LOBBY
	      	networkMode=NETWORK_MODE_LOBBY;
	      #else
	      	networkMode=NETWORK_MODE_TCP;
      	#endif
      	gameState = GAME_STATE_INGAME_START;
      	gameStartProcessed = false;
      	startGame(LOCAL, networkMode);
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> GAME STARTED <<");
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< %d STARTED", receiveBuffer[1]);
				#endif
      	break;
			case MESSAGE_FULL_STATUS:
	      logPrintf("FULL_STATUS: %d\n", receiveBuffer[1]);

				if(networkMode==NETWORK_MODE_TCP||(networkMode==NETWORK_MODE_LOBBY&&roomOwner&&roomOwner!=pthread_self())) // network game, copy old values to a temp. buffer
				{
					for(i=0;i<NUM_AREAS;i++)
						tempAreaDice[i] = game[LOCAL].areaDice[i];
					tempPlayer = game[LOCAL].curPlayer;
				}
				for(i=0;i<MAX_PLAYERS;i++)
				{
					game[LOCAL].playerAreaCount[i] = game[LOCAL].playerDiceCount[i] = 0;
					game[LOCAL].diceReserve[i] = receiveBuffer[5+MAX_PLAYERS*4+NUM_AREAS*2+i];
				}
				for(i=0;i<NUM_AREAS;i++)
				{
					game[LOCAL].playerArea[i] = receiveBuffer[5+MAX_PLAYERS*4+i];
					game[LOCAL].playerAreaCount[game[LOCAL].playerArea[i]]++;
					game[LOCAL].areaDice[i] = receiveBuffer[5+MAX_PLAYERS*4+NUM_AREAS+i];
					game[LOCAL].playerDiceCount[game[LOCAL].playerArea[i]] += game[LOCAL].areaDice[i];
				}
	      
	      // prepare dice piling
	      dicePilingCounter = 0;
				for(i=0;i<NUM_AREAS;i++)
				{
					if(tempAreaDice[i]<game[LOCAL].areaDice[i])
					{
						for(j=0;j<game[LOCAL].areaDice[i]-tempAreaDice[i];j++)
						{
							dicePilingBuffer[dicePilingCounter] = i;
							dicePilingCounter++;
						}
					}
				}
				dicePilingTotal = dicePilingCounter+game[LOCAL].diceReserve[tempPlayer];
				tempAreaCounter = 0;
	      game[LOCAL].stateTimer = game[LOCAL].gameTime;
//	      printf("BEFORE STEPS: gameTime: %d, stateTimer: %d\n", game[LOCAL].gameTime, game[LOCAL].stateTimer);

				if((!firstTurn||networkMode==NETWORK_MODE_LOCAL)&&!game[LOCAL].playerSurrended[tempPlayer])
					sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> %s ended turn <<", game[LOCAL].playerName[tempPlayer]);
				
				game[LOCAL].curTurn = receiveBuffer[1];
				game[LOCAL].curPlayer = receiveBuffer[2];
				game[LOCAL].pot = receiveBuffer[3]+(receiveBuffer[4]<<8);
				for(i=0;i<game[LOCAL].numPlayers;i++)
				{
					game[LOCAL].playerPosition[i] = receiveBuffer[5+i];
					game[LOCAL].playerScore[i] = byte2Signed(receiveBuffer[5+MAX_PLAYERS+i]);
					game[LOCAL].playerStatus[i] = receiveBuffer[5+MAX_PLAYERS*2+i];
					game[LOCAL].playerSurrended[i] = receiveBuffer[5+MAX_PLAYERS*3+i];
				}
				if(game[LOCAL].curPlayer<tempPlayer) // new turn
				{
					sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> Game turn %d started <<", (game[LOCAL].curTurn+1));
				}
				logPrintf("FULL_STATUS, curPlayer now: %d vs. playerId %d\n", game[LOCAL].curPlayer, playerId);
	      gameState = GAME_STATE_INGAME_POST_STATUS; // add dice one by one
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< FULL STATUS");
				#endif
				endTurnPressed = false;
				endTurnChecked = false;
				break;
			case MESSAGE_ATTACK:
	      logPrintf("ATTACK: %d -> %d\n", receiveBuffer[1], receiveBuffer[2]);
	      if(!attackProcessed) // process the old attack first
	      {
//	      	if(networkMode!=NETWORK_MODE_LOBBY||roomOwner==NULL||roomOwner!=pthread_self())
					processAttack();
				}

				attackProcessed = false;
				if(game[LOCAL].curPlayer!=playerId)
		      attackAreaShown = false;
	      curArea = receiveBuffer[1];
	      attackArea = receiveBuffer[2];
				for(i=0;i<8;i++)
				{
					playerDice[0][i] = receiveBuffer[8+i];
					if(playerDice[0][i]>0) playerDiceAmount[0] = i;
					playerDice[1][i] = receiveBuffer[16+i];
					if(playerDice[1][i]>0) playerDiceAmount[1] = i;
				}
				playerTotal[0] = receiveBuffer[6];
				playerTotal[1] = receiveBuffer[7];
//	      printf("player total %d & %d\n", playerTotal[0], playerTotal[1]);
//	      fflush(stdout);
				
				attackDice[0] = receiveBuffer[3];
				attackDice[1] = receiveBuffer[4];
				
				playerWon = receiveBuffer[5];
				game[LOCAL].stateTimer = game[LOCAL].gameTime;
				gameState = GAME_STATE_INGAME_POST_ATTACK;
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], "* %s attacks %s - %d vs. %d", 
				        game[LOCAL].playerName[game[LOCAL].playerArea[curArea]],
				       (game[LOCAL].playerSurrended[game[LOCAL].playerArea[attackArea]]?
				        "no man's land":game[LOCAL].playerName[game[LOCAL].playerArea[attackArea]]),
				        playerTotal[0], playerTotal[1]);

/*				if(playerWon==PLAYER_ATTACKER) // attacker won
				{
					game[LOCAL].playerAreaCount[game[LOCAL].playerArea[receiveBuffer[1]]]++;
					game[LOCAL].playerAreaCount[game[LOCAL].playerArea[receiveBuffer[2]]]--;
					game[LOCAL].playerDiceCount[game[LOCAL].playerArea[receiveBuffer[2]]] -= game[LOCAL].areaDice[receiveBuffer[2]];
					game[LOCAL].playerArea[receiveBuffer[1]] = game[LOCAL].curPlayer;
				}
				else
					game[LOCAL].playerDiceCount[game[LOCAL].playerArea[receiveBuffer[1]]] -= game[LOCAL].areaDice[receiveBuffer[1]]-1;
	
				game[LOCAL].areaDice[receiveBuffer[1]] = receiveBuffer[3];
				game[LOCAL].areaDice[receiveBuffer[2]] = receiveBuffer[4];
	*/

//				playSound(SOUND_DICE_INDEX);
				diceTimer = game[LOCAL].gameTime;
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< %d ATT %d", receiveBuffer[1], receiveBuffer[2]);
				playerDataUpdated = true;
				#endif
				endTurnChecked = false;

//				gameFieldUpdated = true;
				break;
			case MESSAGE_END_GAME:
				logPrintf("END_GAME: %d\n", receiveBuffer[1]);
				
				// reset last attack to normal
				curArea = attackArea = AREA_NONE;
				if(game[LOCAL].gameTime-game[LOCAL].stateTimer<1500)
					processAttack();
				
				playerWon = receiveBuffer[2];
				game[LOCAL].stateTimer = game[LOCAL].gameTime;
				game[LOCAL].gameStatus = GAME_STATUS_ENDED;
				gameFieldUpdated = true;
				playerDataUpdated = true;
				gameState = GAME_STATE_INGAME_END;
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> GAME ENDED <<");
				sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> Winner: %s, duration %d turns <<", game[LOCAL].playerName[playerWon], receiveBuffer[1]);
				if(networkMode==NETWORK_MODE_TCP)
				{
					game[LOCAL].gameScore[playerWon] = byte2Signed(receiveBuffer[3]);
      		sprintf(gameHistory[(gameHistoryCount++)%GAME_HISTORY_LENGTH], ">> %s finished 1. with a score of %d <<", game[LOCAL].playerName[playerWon], game[LOCAL].gameScore[playerWon]);
      	}
				if(playerWon==playerId)
					playSound(SOUND_GAME_WON_INDEX);
				else
					playSound(SOUND_GAME_OVER_INDEX);
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< %d ENDED", receiveBuffer[1]);
				#endif
				endTurnPressed = false;
				break;
			case MESSAGE_ERROR:
				if(receiveBuffer[1]==ERROR_JOIN_GAME_FAILED) // can't join, return to game listings
				{
					logPrintf("ERROR_JOIN_GAME_FAILED: %d\n", receiveBuffer[2]);
					gameState = GAME_STATE_MENU;
					currentMenu = MENU_NETWORK_JOIN_GAME_INDEX;
					playerJoined = false;

					// refresh list of open games
					memset(sendBuffer, 0, NETWORK_MAX_DATA);
					sendBuffer[0] = MESSAGE_LIST_OPEN_GAMES;
					sendClientData(sendBuffer);
				}
				else if(receiveBuffer[1]==ERROR_INIT_CONNECTION_FAILED)
				{
					logPrintf("ERROR INITING CONNECTION: version required %d.%d.%d\n", receiveBuffer[2], receiveBuffer[3], receiveBuffer[4]);
					requiredVersionMajor = receiveBuffer[2];
					requiredVersionMinor = receiveBuffer[3];
					requiredVersionRevision = receiveBuffer[4];
					networkStatus = NETWORK_STATUS_WRONG_VERSION;
					networkConnected = false;
					gameFieldUpdated = true;
					menuUpdated = true;
					// TODO: close connection
					//close(client_sock);
				}
				else if(receiveBuffer[1]==ERROR_INIT_CONNECTION_REQUIRED)
				{
					logPrintf("INIT CONNECTION REQUIRED! hmmm...\n");
				}
				else
		      logPrintf("UNKNOWN ERROR: %d\n", receiveBuffer[1]);

				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< ERROR %d:%d", receiveBuffer[1], receiveBuffer[2]);
				playerDataUpdated = true;
				#endif
				break;
			case MESSAGE_PING:
				memset(sendBuffer, 0, NETWORK_MAX_DATA);
				sendBuffer[0] = MESSAGE_PONG;
				sendBuffer[1] = receiveBuffer[1];
				sendClientData(sendBuffer);
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< PING %d", receiveBuffer[1]);
				playerDataUpdated = true;
				#endif

				if(game[LOCAL].gameStatus<=GAME_STATUS_WAITING_PLAYERS)
				{
					gameStatistics.gamesPlayed = (receiveBuffer[2]%256)+(receiveBuffer[3]<<8);
					gameStatistics.openGames = receiveBuffer[4];
					gameStatistics.activeGames = receiveBuffer[5];
					gameStatistics.lastPlayerConnected = (receiveBuffer[6]%256)+(receiveBuffer[7]<<8);
					gameStatistics.lastGameStarted = (receiveBuffer[8]%256)+(receiveBuffer[9]<<8);
					gameStatistics.lastGameEnded = (receiveBuffer[10]%256)+(receiveBuffer[11]<<8);
					gameStatistics.playersOnline = (receiveBuffer[12]%256)+(receiveBuffer[13]<<8);
					menuUpperDrawn = false;
				}
				break;
			case MESSAGE_PONG:
				pongCounter = receiveBuffer[1];
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< PONG %d", receiveBuffer[1]);
				playerDataUpdated = true;
				#endif
				break;
			default:
	      logPrintf("UNKNOWN MESSAGE: %d\n", receiveBuffer[0]);
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< UNKNOWN %d", receiveBuffer[0]);
				#endif
	      break;
	  }
	}
}

void flushReceiveBuffer()
{
	if(networkMode==NETWORK_MODE_TCP)
	{
		FD_ZERO(&client_set);
	  FD_SET(client_sock, &client_set);
	  struct timeval tval;
	  tval.tv_sec  = 0;
	  tval.tv_usec = 1;
	
		#ifndef NDS
	  int result = select(client_sock + 1, &client_set, NULL, NULL, &tval);
	  #endif
	  if (FD_ISSET(client_sock, &client_set))
	  {
	  	receiveServerData();
	  }
	}
}


void clientJoinGame(int gameId)
{
	if(networkConnected) // &&numOpenGames>0)
	{
		logPrintf("client joining...\n");
		#ifdef LOBBY
			roomOwner = openGameOwners[gameId];
			if(roomOwner!=NULL)
			{
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> L:JOIN %d", gameId);
				#endif
				LPLOBBY_ROOM room = LOBBY_GetRoomByUser(roomOwner);
				LOBBY_JoinRoom(room);
				gameId=0;
			}
			else
				return;
		#endif
	  memset(sendBuffer, 0, NETWORK_MAX_DATA);
	  sendBuffer[0] = MESSAGE_JOIN_GAME;
//		sendBuffer[1] = openGameIds[currentMenuIndex-1];
		sendBuffer[1] = gameId;
		sendBuffer[2] = favouriteColor;
		for(int i=0;i<MAX_NAME_LENGTH;i++)
			sendBuffer[3+i] = playerName[LOCAL][i];
		#ifdef DEBUG
		sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> JOIN %d", sendBuffer[1]);
		#endif

		if(game[LOCAL].gameStatus!=GAME_STATUS_PLAYING)
			game[LOCAL].gameStatus = openGameStatus[currentMenuIndex-1];

		sendClientData(sendBuffer);
		playerJoined = true;
	
		if(game[LOCAL].gameStatus!=GAME_STATUS_PLAYING)
		{
			game[LOCAL].maxPlayers = openGameMaxPlayers[currentMenuIndex-1];
			game[LOCAL].minPlayers = openGameMinPlayers[currentMenuIndex-1];
			game[LOCAL].numPlayers = openGamePlayers[currentMenuIndex-1];
			
			parentMenu[currentMenuLevel] = currentMenu;
			parentMenuIndex[currentMenuLevel] = currentMenuIndex;
			currentMenuLevel++;
			currentMenu = MENU_NETWORK_LOBBY_INDEX;
			menuUpperDrawn = false;
			currentMenuIndex = 1;
		}
		else
		{
			#ifdef DEBUG
			sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "::: IN-GAME?");
			#endif
		}
		game[LOCAL].gameStatus = GAME_STATUS_WAITING_PLAYERS;
	}
	else
	{
		logPrintf("!!! not connected - can't join client!\n");
	}
}

void clientQuitGame()
{
	if(networkConnected&&playerJoined)
	{
		playerJoined = false;
		logPrintf("QUIT GAME\n");
	  memset(sendBuffer, 0, NETWORK_MAX_DATA);
	  sendBuffer[0] = MESSAGE_QUIT_GAME;
		#ifdef DEBUG
		sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> QUIT");
		#endif
		sendClientData(sendBuffer);
		networkRequestSent = false;
		#ifdef LOBBY
		if(networkMode==NETWORK_MODE_LOBBY)
		{
			LOBBY_LeaveRoom();
		}
		#endif
	}
	game[LOCAL].gameStatus=GAME_STATUS_NONEXISTING;
}

void clientReadyToStart()
{
	logPrintf("Ready to start!\n");
	if(networkConnected&&(!readyToStart||game[LOCAL].playerStatus[playerId]!=PLAYER_STATUS_READY))
	{
		logPrintf("Indeed!\n");
	  memset(sendBuffer, 0, NETWORK_MAX_DATA);
	  sendBuffer[0] = MESSAGE_READY_TO_START;
		sendBuffer[1] = playerId;
		for(int i=0;i<MAX_NAME_LENGTH;i++)
		{
			sendBuffer[2+i] = playerName[LOCAL][i];
		}
		#ifdef DEBUG
		sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> READY");
		#endif
		sendClientData(sendBuffer);
		readyToStart = true;
	}
}

void clientListOpenGames()
{
	networkRequestSent = true;

	#ifdef LOBBY
		int index=0;
		LPLOBBY_ROOM room = LOBBY_GetRoomByGame(0, LOBBY_GAMECODE);
		while(room)
		{
    	openGameOwners[index] = LOBBY_GetRoomUserBySlot(room, 0);
			openGameIds[index] = index;
			openGameStatus[index] = GAME_STATUS_WAITING_PLAYERS;
			openGamePlayers[index] = LOBBY_GetUsercountInRoom(room);
			openGameMinPlayers[index] = 2; // TODO
			openGameMaxPlayers[index] = LOBBY_GetMaxUsercountInRoom(room);
    	memset(openGameNames[index], 0, MAX_NAME_LENGTH+8);
    	sprintf(openGameNames[index], "%s    %d/%d", LOBBY_GetUserName(LOBBY_GetRoomUserBySlot(room, 0)), openGamePlayers[index], openGameMaxPlayers[index]);
			
			#ifdef DEBUG
			sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "G[%d] %s", index, openGameNames[index]);
			#endif
			index++;
			room = LOBBY_GetRoomByGame(room, LOBBY_GAMECODE);
		}
		numOpenGames=index;
		menuUpdated = true;
	#else	
		memset(sendBuffer, 0, NETWORK_MAX_DATA);
		sendBuffer[0] = MESSAGE_LIST_OPEN_GAMES;
		sendClientData(sendBuffer);	
	#endif

	#ifdef DEBUG
	sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> LIST");
	#endif
}
