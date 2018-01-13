//#include "network.h"

//byte localData[NETWORK_MAX_DATA];
//byte localCommand;

#ifndef CLIENT

char *MAChexed(byte *MAC)
{
	char* MAChexed = (char*)malloc(13); // 1 byte extra for the eol - memory leak
	sprintf(MAChexed, "%02X%02X%02X%02X%02X%02X", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
	return MAChexed;
}

void receiveClientData(SOCKET sockfd)
{
  int n,i;
  int gameid = -1;
  int playerid = -1;
  byte line[NETWORK_MAX_DATA*10]; // in case lots of data being received
	pthread_t curThread = pthread_self();
	
  //  while(1) {
	// TODO: this is terribly wrong, isn't it? but it just seems to work...
	ioctl(sockfd,FIONBIO,&bMode); // set blocking
 	n = recv(sockfd,(char*)line,NETWORK_MAX_DATA+1,0);
//  logPrintf("Received %d bytes from client: %d:%d\n", n, pthread_self(), line[0]);
	// TODO - should be a while loop ^, in case recv doesn't receive everything...
	ioctl(sockfd,FIONBIO,&nbMode); // set non-blocking
  if ( n <= 0 )
  {
	  for(i=0;i<NETWORK_MAX_THREADS;i++)
	  {
	    if(threadIds[i]==curThread) // player found by thread id
	    {
	      gameid=threadGameIndex[i];
	      playerid=threadPlayerIndex[i];
	      logPrintf("!!! PLAYER [%d:%d] DROPPED! - deleting thread: %d\n", gameid, playerid, i);
	      if(gameid>=0&&game[gameid].playerStatus[playerid]!=PLAYER_STATUS_QUIT) // player was already in a game
	      {
		      game[gameid].playerStatus[playerid]=PLAYER_STATUS_DROPPED;
		      byte data[NETWORK_MAX_DATA];
		      memset(data, 0, NETWORK_MAX_DATA);
		    	data[0] = threadPlayerIndex[i];
		    	boolean onlyPlayer = true;
				  for(int ii=0;ii<NETWORK_MAX_THREADS;ii++)
				  {
				    if(threadGameIndex[ii]==gameid)
				    {
				      int j = threadPlayerIndex[ii];
							if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_QUIT)
						  {
						    logPrintf("SERVER >>> %d.%d MESSAGE_PLAYER_DROPPED [%d]\n", gameid, j, ii);
						    serverMessage(threadIds[ii], MESSAGE_PLAYER_DROPPED, data);
						    onlyPlayer = false;
						  }
				    }
				  }
				  if(onlyPlayer&&(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS|| // if the player was only one left in the game, drop the game as well
				     game[gameid].gameStatus==GAME_STATUS_PLAYING))
				  {
				  	threadRequestGameTimer[gameid] = getTime()+5000;
				  	threadRequestGameEnd[gameid] = true;
/*			      logPrintf("!!! ERASE GAME [%d]\n", gameid);
						if(game[gameid].gameStatus==GAME_STATUS_PLAYING)
						{
							dbPlayerPosition(gameid, playerid, game[gameid].position--);
				  		dbGameErase(gameid);
				  	}
				  	else if(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS)
						  gameStatistics.openGames--;
				  	
				  	game[gameid].gameStatus = GAME_STATUS_NONEXISTING;*/
				  }
			  	if(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS)
			  	{
						logPrintf("!!! Decrease the number of players in game %d from %d to %d\n", gameid, game[gameid].numPlayers, game[gameid].numPlayers-1);
			  		game[gameid].numPlayers--;
			  	}
			  		//if(game[gameid].gameStatus==GAME_STATUS_PLAYING&&game[gameid].curPlayer==playerid) // end turn properly
						//	endTurnLogic(gameid);
				}
	    	threadIds[i] = 0; // delete thread association to this player
	    	threadGameIndex[i] = -1; // delete game index association as well
	    	if(threadConnected[i]) gameStatistics.playersOnline--;
	    	threadConnected[i] = false; // delete connection status
	    	close(sockfd); // close client socket
	      pthread_exit((void *)0); // curThread);
	      break;
	    }
	  }    
  }
  else
  {
    clientMessage(curThread, line[0], line+1);
  }
}

void sendServerData(SOCKET sockfd, byte *sendBuffer)
{
	int i;
  int gameid = -1;
  int playerid = -1;
  int n = NETWORK_MAX_DATA;
  pthread_t curThread = pthread_self();
  
//  logPrintf("... Send server data\n");

	// TODO - block & make sure everything is being sent
//  if (send(sockfd, (const char*)sendBuffer, n, 0) != n) // error condition
  if (send(sockfd, (const char*)sendBuffer, n, MSG_NOSIGNAL) != n) // error condition - NOTE! MSG_NOSIGNAL is Linux-only (no SIG_PIPE is being made)
  {
	  logPrintf("!!! Send error: %s\n", strerror(errno));
	  for(i=0;i<NETWORK_MAX_THREADS;i++)
	  {
	    if(threadIds[i]==curThread) // player found by thread id
	    {
	      gameid=threadGameIndex[i];
	      playerid=threadPlayerIndex[i];
	      logPrintf("!!! PLAYER [%d:%d] DROPPED!\n", gameid, playerid);
	      if(gameid>=0&&game[gameid].playerStatus[playerid]!=PLAYER_STATUS_QUIT) // player was already in a game
	      {
		      game[gameid].playerStatus[playerid]=PLAYER_STATUS_DROPPED;
		      byte data[NETWORK_MAX_DATA];
		      memset(data, 0, NETWORK_MAX_DATA);
		    	data[0] = threadPlayerIndex[i];
		    	boolean onlyPlayer = true;
				  for(int ii=0;ii<NETWORK_MAX_THREADS;ii++)
				  {
				    if(threadGameIndex[ii]==gameid)
				    {
				      int j = threadPlayerIndex[ii];
							if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_QUIT)
						  {
						    logPrintf("SERVER >>> %d.%d MESSAGE_PLAYER_DROPPED [%d]\n", gameid, j, ii);
						    serverMessage(threadIds[ii], MESSAGE_PLAYER_DROPPED, data);
						    onlyPlayer = false;
						  }
				    }
				  }
				  if(onlyPlayer&&(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS|| // if the player was only one left in the game, drop the game as well
				     game[gameid].gameStatus==GAME_STATUS_PLAYING))
				  {
				  	threadRequestGameTimer[gameid] = getTime()+5000;
				  	threadRequestGameEnd[gameid] = true;
/*			      logPrintf("!!! ERASE GAME [%d]\n", gameid);
						if(game[gameid].gameStatus==GAME_STATUS_PLAYING)
						{
							dbPlayerPosition(gameid, playerid, game[gameid].position--);
				  		dbGameErase(gameid);
				  	}
				  	else if(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS)
						  gameStatistics.openGames--;
				  	game[gameid].gameStatus = GAME_STATUS_NONEXISTING;*/
				  }
			  	if(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS)
			  	{
						logPrintf("!!! Decrease the number of players in game %d from %d to %d\n", gameid, game[gameid].numPlayers, game[gameid].numPlayers-1);
			  		game[gameid].numPlayers--;
			  	}
			  		//if(game[gameid].gameStatus==GAME_STATUS_PLAYING&&game[gameid].curPlayer==playerid) // end turn properly
						//	endTurnLogic(gameid);
				}
				logPrintf("delete thread data & exit\n");
	    	threadIds[i] = 0; // delete thread association to this player
	    	threadGameIndex[i] = -1; // delete game index association as well
	    	if(threadConnected[i]) gameStatistics.playersOnline--;
	    	threadConnected[i] = false; // delete connection status
	    	close(sockfd); // close client socket
	      pthread_exit((void *)0); // curThread);
	      break;
	    }
	  }    
  }
  /*else
  {
	  logPrintf("Send ok - done!\n");
  }*/
}
#endif

void clientMessage(pthread_t curThread, byte message, byte *data)
{
  int i;
  int j;
  int gameid = -1;
  int playerid = -1;
	int threadIndex = -1;
//  pthread_t curThread = pthread_self();
  
  // thread index checks
	for(i=0;i<NETWORK_MAX_THREADS;i++)
	{
		if(threadIds[i]==curThread) // found thread id
		{
			threadIndex = i;
			break;
		}
	}
	if(threadIndex==-1) // fresh thread, create index for it
	{
		for(i=0;i<NETWORK_MAX_THREADS;i++)
		{
			if(!threadConnected[i]) // free slot
			{
				logPrintf("!!! Created a thread index in clientMessage: %d\n", i);
				threadIds[i]=curThread;
				threadGameIndex[i]=-1;
				threadPlayerIndex[i]=-1;
				threadIndex=i;
				#ifdef LOBBY
					threadConnected[i] = true;
				#endif
				break;
			}
		}
	}

	#ifndef CLIENT
	if(message==MESSAGE_INIT_CONNECTION) // should be first message received, check if the version matches and store client MAC address
	{
		logPrintf("CLIENT %d.%d <<< INIT_CONNECTION\n", gameid, playerid);
		if(data[0]>=requiredVersionMajor&&data[1]>=requiredVersionMinor&&data[2]>=requiredVersionRevision) // all ok!
		{
			if(!threadConnected[threadIndex]) // TODO - last connection mac
			{
	      gameStatistics.lastPlayerConnected = gameStatistics.lastPlayerConnectedBuffer;
	      gameStatistics.lastPlayerConnectedBuffer = time(NULL);
	    	gameStatistics.playersOnline++;
				threadConnected[threadIndex] = true;
			}
			for(i=0;i<6;i++)
				threadMACAddress[threadIndex][i] = data[3+i];
			for(i=0;i<MAX_NAME_LENGTH;i++)
				threadPlayerName[threadIndex][i] = data[9+i];

			memset(data, 0, NETWORK_MAX_DATA);
			logPrintf("SERVER >>> %d.%d INIT_CONNECTION\n", gameid, playerid);
	    serverMessage(curThread, MESSAGE_INIT_CONNECTION, data);
	    return;
		}
		else // send an error message
		{
			logPrintf("SERVER >>> %d.%d MESSAGE_ERROR [ERROR_INIT_CONNECTION_FAILED - client v%d.%d.%d, required v%d.%d.%d]\n", gameid, playerid, data[0], data[1], data[2], requiredVersionMajor, requiredVersionMinor, requiredVersionRevision);
			memset(data, 0, NETWORK_MAX_DATA);
	  	data[0] = ERROR_INIT_CONNECTION_FAILED;
	  	data[1] = requiredVersionMajor; // required version
	  	data[2] = requiredVersionMinor;
	  	data[3] = requiredVersionRevision;
	  		
	    serverMessage(curThread, MESSAGE_ERROR, data);
	    return;
		}
	}
	else if(message==MESSAGE_SERVER_STATUS) // allow querying server status (TODO: we should set an ip restriction here, though)
	{
		logPrintf("CLIENT %d.%d <<< SERVER_STATUS\n", gameid, playerid);
		long seconds   = time(NULL);
		memset(data, 0, NETWORK_MAX_DATA);		
		data[0]  = ((seconds-gameStatistics.serverStarted)/60)%256;
		data[1]  = ((seconds-gameStatistics.serverStarted)/60)>>8;
		data[2]  = gameStatistics.gamesPlayed%256;
		data[3]  = gameStatistics.gamesPlayed>>8;
		data[4]  = gameStatistics.openGames;
		data[5]  = gameStatistics.activeGames;
		data[6]  = ((seconds-gameStatistics.lastPlayerConnected)/60)%256;
		data[7]  = ((seconds-gameStatistics.lastPlayerConnected)/60)>>8;
		data[8]  = ((seconds-gameStatistics.lastGameStarted)/60)%256;
		data[9]  = ((seconds-gameStatistics.lastGameStarted)/60)>>8;
		data[10] = ((seconds-gameStatistics.lastGameEnded)/60)%256;
		data[11] = ((seconds-gameStatistics.lastGameEnded)/60)>>8;
		data[12] = gameStatistics.playersOnline%256;
		data[13] = gameStatistics.playersOnline>>8;
    serverMessage(curThread, MESSAGE_SERVER_STATUS, data);
    return;
	}
	
	logPrintf("received message: %d\n", data[0]);
	
	if(!threadConnected[threadIndex]) // not connected, don't allow receiving further messages
	{
		logPrintf("SERVER >>> %d.%d MESSAGE_ERROR [ERROR_INIT_CONNECTION_REQUIRED]\n", gameid, playerid);
		memset(data, 0, NETWORK_MAX_DATA);
  	data[0] = ERROR_INIT_CONNECTION_REQUIRED;
  		
    serverMessage(curThread, MESSAGE_ERROR, data);
    return;
	}
	#endif

	// ok, finally we can start checking other messages, too
  if(message==MESSAGE_CREATE_GAME)
  {
  	#ifdef LOBBY
  		gameid=LOCAL;
  	#else
			for(i=0;i<NETWORK_MAX_GAMES;i++)
			{
				// found a free slot for a game
				if(game[i].gameStatus==GAME_STATUS_NONEXISTING)
				{
					gameid=i;
					gameIndexDB[gameid] = -1;
					break;
				}
			}
		#endif
		for(i=0;i<MAX_PLAYERS;i++) // reset player statuses & names
		{
			game[gameid].playerStatus[i] = PLAYER_STATUS_UNCONFIRMED;
			for(j=0;j<MAX_NAME_LENGTH;j++)
				game[gameid].playerName[i][j] = 0;
		}
			
		game[gameid].gameStatus=GAME_STATUS_WAITING_PLAYERS;
		game[gameid].minPlayers=data[0];
		game[gameid].maxPlayers=data[1];
		playerid=rand()%game[gameid].maxPlayers;
		game[gameid].gameOwner=playerid;
		game[gameid].gameField=data[2];
		game[gameid].playerColor[playerid]=data[3];
		for(i=0;i<MAX_NAME_LENGTH;i++)
			game[gameid].playerName[playerid][i]=data[4+i];
		game[gameid].playerType[playerid]=PLAYER_TYPE_NETWORK;
		game[gameid].playerStatus[playerid]=PLAYER_STATUS_WAITING;
		game[gameid].numPlayers=1;
		for(i=0;i<6;i++)
			game[gameid].playerMACAddress[playerid][i]=threadMACAddress[threadIndex][i];
		threadGameIndex[threadIndex]=gameid;
		threadPlayerIndex[threadIndex]=playerid;
	  #ifdef LOBBY
			#ifdef DEBUG
			sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! threadplayer[%d]: %d/%d", threadIndex, playerid, game[gameid].maxPlayers);
			#endif
		#endif
		logPrintf("CLIENT %d.%d <<< CREATE_GAME - min players: %d, max players: %d, field id: %d, player: %s\n", gameid, playerid, data[0], data[1], data[2], game[gameid].playerName[playerid]);
	  #ifdef LOBBY
			#ifdef DEBUG
			sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! min:%d, max:%d", game[gameid].minPlayers, game[gameid].maxPlayers);
			#endif
		#endif

		data[0] = gameid;
		data[1] = playerid;
		data[2] = game[gameid].playerColor[playerid];
		for(i=0;i<MAX_NAME_LENGTH;i++)
		  data[3+i] = game[gameid].playerName[playerid][i];

		logPrintf("SERVER >>> %d.%d MESSAGE_PLAYER_JOINED\n", gameid, playerid);
		serverMessage(curThread, MESSAGE_PLAYER_JOINED, data);

		#ifndef CLIENT
		dbPlayerLink(gameid, playerid, MAChexed(threadMACAddress[threadIndex]));
		#endif
		
		gameStatistics.openGames++;
	}
  else if(message==MESSAGE_JOIN_GAME)
	{
	  #ifdef LOBBY
			#ifdef DEBUG
			sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< S:JOIN");
			#endif
		#endif
	  gameid=data[0];
//	  logPrintf("to be joined gameid: %d:%d\n", gameid, data[1]);
  	boolean rejoin = false;
  	if(game[gameid].gameStatus==GAME_STATUS_PLAYING) // check first if we're re-joining after a drop
  	{
		  for(i=0;i<MAX_PLAYERS;i++)
		  {
		  	if(game[gameid].playerStatus[i]==PLAYER_STATUS_QUIT||game[gameid].playerStatus[i]==PLAYER_STATUS_DROPPED)
		  	{
		  		rejoin = true;
		  		for(j=0;j<6;j++)
		  		{
				  	if(game[gameid].playerMACAddress[i][j]!=threadMACAddress[threadIndex][j])
				  		rejoin = false;
				  }
				  if(rejoin)
				  {
				  	playerid=i;
				  	break;
					}
				}
		  }
		}
		else if(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS) // check if we've already joined
		{
		  for(i=0;i<MAX_PLAYERS;i++)
		  {
		  	if(game[gameid].playerStatus[i]==PLAYER_STATUS_WAITING)
		  	{
					boolean joinError = true;
		  		for(j=0;j<6;j++)
		  		{
				  	if(game[gameid].playerMACAddress[i][j]!=threadMACAddress[threadIndex][j])
				  		joinError = false;
				  }
				  if(joinError) // check if the names match (this is only for debugging pc / mac clients)
				  {
				  	logPrintf("!!! Found mac address - double join?\n");
				  	for(j=0;j<MAX_NAME_LENGTH;j++)
			  		{
					  	if(game[gameid].playerName[i][j]!=threadPlayerName[threadIndex][j])
					  	{
					  		logPrintf("No double... at %d:%d: %c vs. %c\n", i, j, game[gameid].playerMACAddress[i][j], threadPlayerName[threadIndex][j]);
					  		joinError = false;
					  	}
					  }
				  }
				  if(joinError)
				  {
						logPrintf("SERVER >>> %d.%d MESSAGE_ERROR [ERROR_JOIN_GAME_FAILED - player has already joined the game!]\n", gameid, playerid);
				  	data[0] = ERROR_JOIN_GAME_FAILED;
				  	data[1] = 3;
				  		
				    serverMessage(curThread, MESSAGE_ERROR, data);
				    return;
					}
				}
		  }
		}
	  if(game[gameid].gameStatus!=GAME_STATUS_WAITING_PLAYERS&&!rejoin)
	  {
			logPrintf("SERVER >>> %d.%d MESSAGE_ERROR [ERROR_JOIN_GAME_FAILED - game does not exist]\n", gameid, playerid);
	  	data[0] = ERROR_JOIN_GAME_FAILED;
	  	if(game[gameid].gameStatus==GAME_STATUS_NONEXISTING) data[1] = 0;
	  	else data[1] = 1;
	  		
	    serverMessage(curThread, MESSAGE_ERROR, data);
	    return;
	  }
	  else
	  {
		  if(!rejoin) // find a free slot for the new player & set player color
		  {
		  	int amt=0;
		  	int tmpPlayers[MAX_PLAYERS];
			  for(i=0;i<game[gameid].maxPlayers;i++) 
			  {
			  	if(game[gameid].playerStatus[i]==PLAYER_STATUS_UNCONFIRMED||game[gameid].playerStatus[i]>=PLAYER_STATUS_QUIT)
			  		tmpPlayers[amt++]=i;
			  }
		  	if(amt==0) // max player limit reached
		  	{
					logPrintf("SERVER >>> %d.%d MESSAGE_ERROR [ERROR_JOIN_GAME_FAILED - max. players limit %d reached]\n", gameid, playerid, game[gameid].maxPlayers);
			  	data[0] = ERROR_JOIN_GAME_FAILED;
			  	data[1] = 2;
			  		
			    serverMessage(curThread, MESSAGE_ERROR, data);
			    return;
		  	}
			  #ifdef LOBBY
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< playerid1: %d", playerid);
					#endif
				#endif
			  playerid=tmpPlayers[rand()%amt];
			  #ifdef LOBBY
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< playerid2: %d", playerid);
					#endif
				#endif
			  game[gameid].playerColor[playerid]=-1; // unset for now
			  int playerColor = data[1]; // color player wishes to use
			  boolean colorFound;
			  while(game[gameid].playerColor[playerid]==-1) // find a color for the player
			  {
			  	colorFound = true;
				  for(i=0;i<MAX_PLAYERS;i++)
				  {
				  	if(game[gameid].playerColor[i]==playerColor)
				  		colorFound = false;
				  }
				  if(!colorFound) // no color found, increment player color
				  {
				  	if(++playerColor>7) playerColor=0;
				  }
				  else
				  {
				  	game[gameid].playerColor[playerid]=playerColor;
				  }
				}
				for(i=0;i<MAX_NAME_LENGTH;i++)
					game[gameid].playerName[playerid][i]=data[2+i];
			}
			game[gameid].playerLastActiveTurn[playerid] = game[gameid].curTurn; // update immediately - in case this was a dropped player, we don't want to stop him from playing
			game[gameid].playerType[playerid]=PLAYER_TYPE_NETWORK;
			game[gameid].playerStatus[playerid]=PLAYER_STATUS_WAITING;
			for(i=0;i<6;i++)
				game[gameid].playerMACAddress[playerid][i]=threadMACAddress[threadIndex][i];
			threadGameIndex[threadIndex]=gameid;
			threadPlayerIndex[threadIndex]=playerid;
			logPrintf("CLIENT %d.%d <<< JOIN_GAME\n", gameid, playerid);
	
			// send newly joined player a full player status message
			for(i=0;i<NETWORK_MAX_DATA;i++) data[i]=0;
			data[0] = playerid;
			if(game[gameid].gameStatus!=GAME_STATUS_PLAYING)
				data[1] = game[gameid].numPlayers+1;
			else
				data[1] = game[gameid].numPlayers;
			for(i=0;i<MAX_PLAYERS;i++) // was: numPlayers
			{
				data[2+i*(4+MAX_NAME_LENGTH)] = i;
				data[3+i*(4+MAX_NAME_LENGTH)] = game[gameid].playerStatus[i];
				data[4+i*(4+MAX_NAME_LENGTH)] = game[gameid].playerSurrended[i];
				data[5+i*(4+MAX_NAME_LENGTH)] = game[gameid].playerColor[i];
				for(j=0;j<MAX_NAME_LENGTH;j++)
					data[6+i*(4+MAX_NAME_LENGTH)+j] = game[gameid].playerName[i][j];
			}
			logPrintf("SERVER >>> %d.%d MESSAGE_PLAYER_STATUS [%d]\n", gameid, playerid, threadIndex);
	    serverMessage(curThread, MESSAGE_PLAYER_STATUS, data);
	
			// send message to other players in the same game
			for(i=0;i<NETWORK_MAX_DATA;i++) data[i]=0;
			logPrintf("Joining player (%d): ", data[0]);
			data[0] = gameid;
			data[1] = playerid;
			data[2] = game[gameid].playerColor[playerid];
			for(i=0;i<MAX_NAME_LENGTH;i++)
			{
			  data[3+i] = game[gameid].playerName[playerid][i];
			  if(data[3+i]>0)
					printf("%c", data[3+i]); 
			}
			printf("\n");
	
			if(!rejoin||game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS)
				game[gameid].numPlayers++;

			for(i=0;i<NETWORK_MAX_THREADS;i++)
		  {
		    if(threadGameIndex[i]==gameid)
	      {
					j = threadPlayerIndex[i];
			    if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_QUIT) // cpu players don't have threads... &&game[gameid].playerType!=PLAYER_TYPE_CPU)
		      {
		      	if(threadIds[i]!=curThread) // don't send a join message to player who joined
		      	{
							logPrintf("SERVER >>> %d.%d MESSAGE_PLAYER_JOINED [%d]\n", gameid, j, i);
				      serverMessage(threadIds[i], MESSAGE_PLAYER_JOINED, data);
				    }
		      }
		    }
		  }
	
		  #ifdef LOBBY
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! numplayers: %d", game[gameid].numPlayers);
				#endif
			#endif

			#ifndef CLIENT	
			if(game[gameid].gameStatus==GAME_STATUS_PLAYING) // send game start & full status to player
			{
				rejoinPlayer(curThread, data, gameid, playerid);
			}
			else
			{
				// max player limit reached, start the game!
				if(game[gameid].numPlayers==game[gameid].maxPlayers)
				{
					startNetworkGame(data, gameid);
				}
			}
			#endif

			#ifndef CLIENT
			dbPlayerLink(gameid, playerid, MAChexed(threadMACAddress[threadIndex]));
			#endif
		}
	}
  else
	{
		if(networkMode!=NETWORK_MODE_LOCAL) // search game & player by threads
		{
      gameid=threadGameIndex[threadIndex];
      playerid=threadPlayerIndex[threadIndex];
		}
		#ifdef CLIENT
		else // local game
		{
			gameid = LOCAL;
			playerid = game[gameid].curPlayer;
		}
		#endif

		switch(message)
		{
			#if defined LOBBY || !defined CLIENT
			case MESSAGE_LIST_OPEN_GAMES:
			{
			  logPrintf("CLIENT %d.%d <<< LIST_OPEN_GAMES [%d]\n", gameid, playerid, threadIndex);
			  int gameCount=0;
			  int openGames[NETWORK_MAX_LISTED_GAMES];
			  #ifdef LOBBY
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< S:LIST");
					#endif
				#endif
			  // check for rejoin situation first
			  for(i=0;i<NETWORK_MAX_GAMES;i++)
			  {
			  	if(game[i].gameStatus==GAME_STATUS_PLAYING)
			  	{
			  		for(j=0;j<MAX_PLAYERS;j++)
			  		{
			  			if(game[i].playerStatus[j]==PLAYER_STATUS_DROPPED||game[i].playerStatus[j]==PLAYER_STATUS_QUIT)
			  			{
			  				boolean MACMatch=true;
			  				for(int k=0;k<6;k++)
			  				{
			  					if(game[i].playerMACAddress[j][k]!=threadMACAddress[threadIndex][k])
			  					{
			  						MACMatch = false;
			  					}
			  				}
			  				if(MACMatch) // hey, we might be re-joining this player into a game
			  				{
			  					logPrintf("!!! Client might want to re-join game %d?\n", i);
								  openGames[gameCount]=i;
									gameCount++;
			  				}
			  			}
			  		}
			  	}
			  }
			  for(i=0;i<NETWORK_MAX_GAMES;i++)
				{
			  	if(game[i].gameStatus==GAME_STATUS_WAITING_PLAYERS)
					{
					  openGames[gameCount]=i;
						gameCount++;
					}
			   	if(gameCount>=NETWORK_MAX_LISTED_GAMES)
						break;
			  }
			  for(i=0;i<NETWORK_MAX_DATA;i++) data[i]=0;
			  data[0] = gameCount;
			  if(gameCount>0) // list details of each game
			  {
			    for(i=0;i<gameCount;i++)
					{
				  	gameid = openGames[i];
				  	data[i*(6+MAX_NAME_LENGTH)+1] = gameid;
				  	data[i*(6+MAX_NAME_LENGTH)+2] = game[gameid].gameStatus;
				  	data[i*(6+MAX_NAME_LENGTH)+3] = game[gameid].numPlayers;
				  	data[i*(6+MAX_NAME_LENGTH)+4] = game[gameid].minPlayers;
				  	data[i*(6+MAX_NAME_LENGTH)+5] = game[gameid].maxPlayers;
				  	data[i*(6+MAX_NAME_LENGTH)+6] = game[gameid].gameField;
				  	logPrintf("LIST[%d]: gameid %d, field id: %d\n", i, gameid, game[gameid].gameField);
				  	int owner = game[gameid].gameOwner;
				  	
				  	if(game[gameid].playerStatus[owner]==PLAYER_STATUS_DROPPED||game[gameid].playerStatus[owner]==PLAYER_STATUS_UNCONFIRMED||game[gameid].playerType[owner]==PLAYER_TYPE_CPU)
				  	{
					  	for(j=0;j<MAX_PLAYERS;j++)
					  		if(game[gameid].playerType[j]!=PLAYER_TYPE_CPU&&(game[gameid].playerStatus[j]==PLAYER_STATUS_WAITING||game[gameid].playerStatus[j]==PLAYER_STATUS_READY))
					  			{ owner = j; break; }
					  }
				  	
				  	logPrintf("GAME OWNER: %d - %s", owner, game[gameid].playerName[owner]);
				  	for(j=0;j<MAX_NAME_LENGTH;j++)
				  	  data[i*(6+MAX_NAME_LENGTH)+7+j] = game[gameid].playerName[owner][j];
					}
			  }
			  // send data back to player
			  logPrintf("SERVER >>> %d.%d MESSAGE_LIST_OPEN_GAMES: %d\n", gameid, playerid, gameCount);
			  serverMessage(curThread, MESSAGE_LIST_OPEN_GAMES, data);
			  break;
			}
			case MESSAGE_READY_TO_START:
			{
			  #ifdef LOBBY
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< S:READY");
					#endif
				#endif
			  logPrintf("CLIENT %d.%d <<< READY_TO_START\n", gameid, playerid);
			  if(gameid<0||playerid<0) 
			  {
			  	logPrintf("!!! Can't send a message to unknown player!\n");
			  	return;
			  }
				game[gameid].playerStatus[playerid] = PLAYER_STATUS_READY;
	
				// send a message to all players about player being ready
				for(i=0;i<NETWORK_MAX_DATA;i++) data[i]=0;
				data[0] = playerid;
	
	      for(i=0;i<NETWORK_MAX_THREADS;i++)
	      {
	        if(threadGameIndex[i]==gameid)
	        {
	          j = threadPlayerIndex[i];
					  #ifdef LOBBY
							#ifdef DEBUG
							sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> thread[%d]: %d", i, j);
							#endif
						#endif
						if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_QUIT)
					  {
						  #ifdef LOBBY
								#ifdef DEBUG
								sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> S:READY %d", j);
								#endif
							#endif
					    logPrintf("SERVER >>> %d.%d MESSAGE_READY_TO_START [%d]\n", gameid, j, i);
					    serverMessage(threadIds[i], MESSAGE_READY_TO_START, data);
					  }
		      }
			  }
	
				if(game[gameid].numPlayers>=game[gameid].minPlayers) // minimum players already in the game
				{
				  boolean startGame = true;
				  for(i=0;i<MAX_PLAYERS;i++)
				  {
				    if(game[gameid].playerStatus[i]==PLAYER_STATUS_WAITING)
				    {
				      startGame = false;
				    }
				  }
				  if(startGame) // start the game!
				  {
				  	// rest of the players will be cpu controlled
			  		int cpu=0;
				  	for(i=0;i<game[gameid].maxPlayers;i++)
				  	{
				  		if(game[gameid].playerStatus[i]!=PLAYER_STATUS_READY)
				  		{
								for(j=0;j<NETWORK_MAX_DATA;j++) data[j]=0;
								logPrintf("Adding CPU player %d\n", cpu);
			      		for(j=0;j<MAX_NAME_LENGTH;j++) // empty name
			      			game[gameid].playerName[i][j] = 0;
	      			
				    		game[gameid].playerName[i][0] = 'C';
				    		game[gameid].playerName[i][1] = 'P';
				    		game[gameid].playerName[i][2] = 'U';
				    		game[gameid].playerName[i][3] = '1'+cpu;
								cpu++;
				    		game[gameid].playerType[i] = PLAYER_TYPE_CPU;
								game[gameid].playerStatus[i] = PLAYER_STATUS_WAITING;
								game[gameid].playerColor[i] = rand()%MAX_PLAYERS; // game[gameid].maxPlayers;
								boolean ret;
								do
								{
									ret = true;
									if(++game[gameid].playerColor[i]>=MAX_PLAYERS) game[gameid].playerColor[i]=0;
			
									for(int ii=0;ii<MAX_PLAYERS;ii++)
									{
										if(i!=ii&&game[gameid].playerColor[ii]==game[gameid].playerColor[i])
											ret = false;
									}
								} while(!ret);
								logPrintf("CPU color set to %d\n", game[gameid].playerColor[i]);
								// artificial mac addresses for the cpu players
								#ifndef CLIENT
								char *MAChexed = (char*)malloc(13);
								sprintf(MAChexed, "00000000000%d", cpu);
								dbPlayerLink(gameid, i, MAChexed);
								free(MAChexed);
								#endif
	
								data[0] = gameid;
								data[1] = i;
								data[2] = game[gameid].playerColor[i];
								for(j=0;j<MAX_NAME_LENGTH;j++)
								{
								  data[3+j] = game[gameid].playerName[i][j];
								}
	
								for(int k=0;k<NETWORK_MAX_THREADS;k++)
							  {
							    if(threadGameIndex[k]==gameid)
						      {
										j = threadPlayerIndex[k];
								    if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_QUIT) // cpu players don't have threads... &&game[gameid].playerType!=PLAYER_TYPE_CPU)
							      {
											logPrintf("SERVER >>> %d.%d MESSAGE_PLAYER_JOINED [%d]\n", gameid, j, i);
								      serverMessage(threadIds[k], MESSAGE_PLAYER_JOINED, data);
							      }
							    }
							  }
								game[gameid].numPlayers++;
							}
				  	}
//						long miniSleep = getTime();
//						do{ ; }while(getTime()-miniSleep<1000);
				  	//sleep(1);
				  	threadGameAI[gameid] = true; // request a new AI thread for this game
						startNetworkGame(data, gameid);
				  }
				}
				break;
			}
			#endif // ifndef CLIENT
			case MESSAGE_QUIT_GAME:
			{
			  logPrintf("CLIENT %d.%d <<< QUIT_GAME\n", gameid, playerid);
			  if(gameid==-1||playerid==-1)
			  {
			  	logPrintf("!!! Client bugs. Player tries to quit before joining a game. FIXME!\n");
			  	return;
			  }
			  for(i=0;i<NETWORK_MAX_DATA;i++) data[i]=0;
				// no data ?
	
	      game[gameid].playerStatus[playerid]=PLAYER_STATUS_QUIT;
	      byte data[NETWORK_MAX_DATA];
	      memset(data, 0, NETWORK_MAX_DATA);
	    	data[0] = playerid;
	    	boolean onlyPlayer = true;
			  for(int ii=0;ii<NETWORK_MAX_THREADS;ii++)
			  {
			    if(threadGameIndex[ii]==gameid)
			    {
			      int j = threadPlayerIndex[ii];
						if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_QUIT)
					  {
					    logPrintf("SERVER >>> %d.%d MESSAGE_PLAYER_QUIT [%d]\n", gameid, j, ii);
					    serverMessage(threadIds[ii], MESSAGE_PLAYER_QUIT, data);
					    onlyPlayer = false;
					  }
			    }
			  }
			  if(onlyPlayer&&(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS|| // if the player was only one left in the game, drop the game as well
			     game[gameid].gameStatus==GAME_STATUS_PLAYING))
			  {
			  	// no need for requests - quit is a clear signal of ending game immediately
//			  	threadRequestGameTimer[gameid] = getTime()+5000;
//			  	threadRequestGameEnd[gameid] = true;
		      logPrintf("!!! ERASE GAME [%d]\n", gameid);
					if(game[gameid].gameStatus==GAME_STATUS_PLAYING)
					{
						dbPlayerPosition(gameid, playerid, game[gameid].position--);
			  		dbGameErase(gameid);
						gameStatistics.activeGames--;
			  	}
			  	else if(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS)
					  gameStatistics.openGames--;
			  	game[gameid].gameStatus = GAME_STATUS_NONEXISTING;
			  }
			  else
			  {
			  	if(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS)
			  	{
						logPrintf("!!! Decrease the number of players in game %d from %d to %d\n", gameid, game[gameid].numPlayers, game[gameid].numPlayers-1);
			  		game[gameid].numPlayers--;
			  	}
		  		//if(game[gameid].gameStatus==GAME_STATUS_PLAYING&&game[gameid].curPlayer==playerid) // end turn properly
					//	endTurnLogic(gameid);
				}
			  break;
			}
			case MESSAGE_ATTACK:
			{
			  logPrintf("CLIENT %d.%d <<< ATTACK\n", gameid, playerid);	
				processAttack(gameid, playerid, data);
				break;
			}
			case MESSAGE_SURRENDER:
			{
				logPrintf("CLIENT %d.%d <<< SURRENDER\n", gameid, playerid);

				threadGameActivityTimer[gameid] = getTime();
				game[gameid].playerSurrended[playerid] = true;
				int score = dbPlayerPosition(gameid, playerid, game[gameid].position--);

	      byte data[NETWORK_MAX_DATA];
	      memset(data, 0, NETWORK_MAX_DATA);
	    	data[0] = playerid;
	    	data[1] = game[gameid].position+1;
	    	data[2] = score;
	    	if(networkMode==NETWORK_MODE_LOCAL)
			    serverMessage(curThread, MESSAGE_PLAYER_SURRENDED, data);
			  else
			  {
				  for(int ii=0;ii<NETWORK_MAX_THREADS;ii++)
				  {
				    if(threadGameIndex[ii]==gameid)
				    {
				      int j = threadPlayerIndex[ii];
							if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_QUIT)
						  {
						    logPrintf("SERVER >>> %d.%d MESSAGE_PLAYER_SURRENDED [%d]\n", gameid, j, ii);
						    serverMessage(threadIds[ii], MESSAGE_PLAYER_SURRENDED, data);
						  }
				    }
				  }
				}

				if(game[gameid].curPlayer==playerid)
					endTurnLogic(gameid);

				break;
			}
			case MESSAGE_END_TURN:
			{
			  logPrintf("CLIENT %d.%d <<< END_TURN\n", gameid, playerid);
				threadGameActivityTimer[gameid] = getTime();
				if(!threadRequestGameEnd[gameid]) // don't allow end turn when the game is about to end
				{
					if(game[gameid].curPlayer==playerid)
					{
						endTurnLogic(gameid);
					}
					else
						logPrintf("Client tried to end turn twice!\n");
	
			  	#ifndef CLIENT
						//long miniSleep = getTime();
						//do{ ; }while(getTime()-miniSleep<1000);
				//  	sleep(1);
			  	#endif
			  }
				break;
			}
			case MESSAGE_PING:
			{
				logPrintf("CLIENT %d.%d <<< CLIENT PING %d\n", gameid, playerid, data[0]);
				logPrintf("SERVER >>> %d.%d CLIENT PONG %d\n", gameid, playerid, data[0]);
				serverMessage(curThread, MESSAGE_PONG, data);
				break;
			}
			case MESSAGE_PONG:
			{
				logPrintf("CLIENT %d.%d <<< PONG %d\n", gameid, playerid, data[0]);
				threadPongCounter[threadIndex] = data[0];
				break;
			}
		}
	}
}

void serverMessage(pthread_t threadId, byte message, byte *data)
{
  int i;

	#ifndef CLIENT
		int j;
		// server
//	 	logPrintf("Lock thread %d for send buffering\n", pthread_self());
		pthread_mutex_lock(&mutex);
//		logPrintf("Lock succeeded for %d\n", pthread_self());
	  for(i=0;i<NETWORK_BUFFER_LENGTH;i++)
	  {
	    if(threadBufferIds[i]==0) // allocate this buffer
	    {
//	    	logPrintf("!!! Thread[%d]: Allocate index %d for message %d\n", threadId, i, message);
				threadBufferIds[i] = threadId;
				threadSendBuffer[i][0] = message;
				for(j=0;j<NETWORK_MAX_DATA-1;j++)
				  threadSendBuffer[i][1+j] = data[j];
				break; // let's send only one message, ok? 
	    }
	  }
//	 	logPrintf("Unlock thread %d from send buffering\n", pthread_self());
	  pthread_mutex_unlock(&mutex);
	#else
		// client
		receiveBuffer[0] = message;
		for(i=0;i<NETWORK_MAX_DATA-1;i++)
			receiveBuffer[i+1] = data[i];
		
		#ifdef LOBBY
		if(networkMode==NETWORK_MODE_LOBBY&&roomOwner&&threadId!=roomOwner) //pthread_self())
			LOBBY_SendToUser(threadId, 0x0001, receiveBuffer, NETWORK_MAX_DATA);
		else
		#endif
			receiveServerData();
	#endif
}

#if defined LOBBY || !defined CLIENT
void rejoinPlayer(pthread_t threadId, byte *data, int gameid, int playerid)
{
	int i,j;
	logPrintf("!!! Re-join player!\n");
	for(i=0;i<NETWORK_MAX_DATA;i++) data[i]=0;
	data[0] = game[gameid].gameField;
	data[1] = game[gameid].numPlayers;
	for(i=0;i<game[gameid].numPlayers;i++)
	{
		logPrintf("Player[%d] name: ", i);
		for(j=0;j<MAX_NAME_LENGTH;j++)
		{
			data[2+MAX_PLAYERS+i*MAX_NAME_LENGTH+j] = game[gameid].playerName[i][j];
			if(data[2+MAX_PLAYERS+i*MAX_NAME_LENGTH+j]>13)
				printf("%c", data[2+MAX_PLAYERS+i*MAX_NAME_LENGTH+j]);
		}
		printf("\n");
		data[2+i] = game[gameid].playerType[i];
		logPrintf("Player type: %d\n", data[2+i]);
	}
  logPrintf("SERVER >>> %d.%d MESSAGE_START_GAME\n", gameid, playerid);
  serverMessage(threadId, MESSAGE_START_GAME, data);

	data[0] = game[gameid].curTurn;
	data[1] = game[gameid].curPlayer;
	data[2] = game[gameid].pot%256;
	data[3] = game[gameid].pot>>8;
  for(i=0;i<NUM_AREAS;i++)
  {
  	data[4+MAX_PLAYERS*4+i] = game[gameid].playerArea[i];
  	data[4+MAX_PLAYERS*4+NUM_AREAS+i] = game[gameid].areaDice[i];
  }
	for(i=0;i<MAX_PLAYERS;i++)
	{
		data[4+i] = game[gameid].playerPosition[i];
		data[4+MAX_PLAYERS+i] = game[gameid].playerScore[i];
		data[4+MAX_PLAYERS*2+i] = game[gameid].playerStatus[i];
		data[4+MAX_PLAYERS*3+i] = game[gameid].playerSurrended[i];
		data[4+MAX_PLAYERS*4+NUM_AREAS*2+i] = game[gameid].diceReserve[i];
	}
  logPrintf("SERVER >>> %d.%d MESSAGE_FULL_STATUS\n", gameid, playerid);
  serverMessage(threadId, MESSAGE_FULL_STATUS, data);
}

void startNetworkGame(byte *data, int gameid)
{
	int i,j;
	logPrintf("!!! Start network game!\n");
	for(i=0;i<NETWORK_MAX_DATA;i++) data[i]=0;
	threadGameActivityTimer[gameid] = getTime();
	game[gameid].gameStatus = GAME_STATUS_PLAYING; // removes the game from the open games lists
	initGameField(gameid, game[gameid].gameField);
	game[gameid].curTurn=0;
	game[gameid].curPlayer=0;
	data[0] = game[gameid].gameField;
	data[1] = game[gameid].numPlayers;
	game[gameid].position = game[gameid].numPlayers;
	for(i=0;i<game[gameid].numPlayers;i++)
	{
		logPrintf("Player[%d] name: ", i);
		for(j=0;j<MAX_NAME_LENGTH;j++)
		{
			data[2+MAX_PLAYERS+i*MAX_NAME_LENGTH+j] = game[gameid].playerName[i][j];
			if(data[2+MAX_PLAYERS+i*MAX_NAME_LENGTH+j]>13)
				printf("%c", data[2+MAX_PLAYERS+i*MAX_NAME_LENGTH+j]);
		}
		printf("\n");
		data[2+i] = game[gameid].playerType[i];
		logPrintf("Player type: %d\n", data[2+i]);
		game[gameid].diceReserve[i]=0;
		game[gameid].playerSurrended[i]=false;
		game[gameid].playerLastActiveTurn[i]=-1;
		game[gameid].playerScore[i] = 0;
		game[gameid].playerScore[i] -= 2;
		game[gameid].pot += 2;
	}
	game[gameid].playerScore[0] -= 2; // first turn automated
	game[gameid].pot += 2;		

  for(i=0;i<NETWORK_MAX_THREADS;i++)
  {
    if(threadGameIndex[i]==gameid)
    {
      j = threadPlayerIndex[i];
			if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_DROPPED)
		  {
			  #ifdef LOBBY
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> START: %d", j);
					#endif
				#endif
		    logPrintf("SERVER >>> %d.%d MESSAGE_START_GAME [%d]\n", gameid, j, i);
		    serverMessage(threadIds[i], MESSAGE_START_GAME, data);
		  }
    }
  }
//	long miniSleep = getTime();
//	do{ ; }while(getTime()-miniSleep<1000);
// 	sleep(1);

	// init game data and send first full status
	resetPlayerAreas(gameid);
	data[0] = game[gameid].curTurn;
	data[1] = game[gameid].curPlayer;
	data[2] = game[gameid].pot%256;
	data[3] = game[gameid].pot>>8;
  for(i=0;i<NUM_AREAS;i++)
  {
  	data[4+MAX_PLAYERS*4+i] = game[gameid].playerArea[i];
  	data[4+MAX_PLAYERS*4+NUM_AREAS+i] = game[gameid].areaDice[i];
  }
	for(i=0;i<MAX_PLAYERS;i++)
	{
		data[4+i] = game[gameid].playerPosition[i];
		data[4+MAX_PLAYERS+i] = game[gameid].playerScore[i];
		data[4+MAX_PLAYERS*2+i] = game[gameid].playerStatus[i];
		data[4+MAX_PLAYERS*3+i] = game[gameid].playerSurrended[i];
		data[4+MAX_PLAYERS*4+NUM_AREAS*2+i] = game[gameid].diceReserve[i];
	}
  for(i=0;i<NETWORK_MAX_THREADS;i++)
  {
    if(threadGameIndex[i]==gameid)
    {
      j = threadPlayerIndex[i];
			if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_DROPPED)
		  {
			  #ifdef LOBBY
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> STATUS: %d", j);
					#endif
				#endif
		    logPrintf("SERVER >>> %d.%d MESSAGE_FULL_STATUS [%d]\n", gameid, j, i);
		    serverMessage(threadIds[i], MESSAGE_FULL_STATUS, data);
		  }
    }
  }
  
 	dbGameStart(gameid);
 	
 	#ifdef LOBBY
 	if(networkMode==NETWORK_MODE_LOBBY) // hide game
 		LOBBY_SetRoomVisibility(false);
 	#endif
}
#endif

void resetPlayerAreas(int gameid)
{
	int playerIndex = 0;
	int diceAmt = (int)floor(MAX_DICE_TOTAL/game[gameid].numPlayers);
//	int playerAreas = 29; // DEBUG
	int playerAreas = (int)floor(NUM_AREAS/game[gameid].numPlayers);
	int j=0;
	int i,index,inc;
	boolean rerun = false;
	index = 0;

	for(i=0;i<MAX_PLAYERS;i++) { game[gameid].playerAreaCount[i] = game[gameid].playerDiceCount[i] = 0; }
	for(i=0;i<NUM_AREAS;i++) game[gameid].playerArea[i] = 69;
	for(i=0;i<NUM_AREAS;i++)
	{
		inc = rand()%8+5;
		index += inc;
		if(++index>=NUM_AREAS) index -= NUM_AREAS;
		while(game[gameid].playerArea[index]!=69) {
			if(++index>=NUM_AREAS) index = 0;
		}
		game[gameid].playerArea[index] = playerIndex;
		game[gameid].playerAreaCount[game[gameid].playerArea[index]]++;
		game[gameid].areaDice[index] = 1; 
		game[gameid].playerDiceCount[game[gameid].playerArea[index]] += game[gameid].areaDice[index];
		j++;
		if(j>=playerAreas||rerun) {
			playerIndex++;
			j=0;
		}
		if(playerIndex>=game[gameid].numPlayers)
		{
			rerun=true;
			playerIndex=0;
		}
	}

	for(j=0;j<game[gameid].numPlayers;j++)
	{
		index = inc = 0;
//		logPrintf("player %d: %d\n", j, (diceAmt-playerAreas));
		for(i=0;i<diceAmt-playerAreas;i++)
		{
			inc = rand()%8+5;
			index += inc;
			if(++index>=NUM_AREAS) index -= NUM_AREAS;
			int counter=0;
			while(game[gameid].playerArea[index]!=j||game[gameid].areaDice[index]==MAX_DICE)
			{
				if(++index>=NUM_AREAS) index -= NUM_AREAS;
//				logPrintf("inc area index: %d\n", index);
				counter++;
				if(counter>NUM_AREAS) // never should end up here
				{
//					logPrintf("ERROR: COULDN'T FIND FREE AREAS FOR DICE!\n");
					break;
				}
			}
			game[gameid].areaDice[index] = game[gameid].areaDice[index]+1;
			game[gameid].playerDiceCount[game[gameid].playerArea[index]] += 1;
		}
	}
}

boolean findNextPlayer(int gameid)
{
	boolean foundPlayer = false;
	int counter = 0;
	
	while(!foundPlayer) // search for next player until found
	{
		if(++game[gameid].curPlayer>=game[gameid].numPlayers) // MAX_PLAYERS
		{
//			logPrintf("At player %d, go back to 0\n", game[gameid].curPlayer);
			game[gameid].curPlayer = 0;
			game[gameid].curTurn++;
		}
		if(game[gameid].playerAreaCount[game[gameid].curPlayer]>0&&!game[gameid].playerSurrended[game[gameid].curPlayer]&&
		  (game[gameid].playerStatus[game[gameid].curPlayer]==PLAYER_STATUS_READY||
		   game[gameid].playerStatus[game[gameid].curPlayer]==PLAYER_STATUS_DROPPED|| // <- handled by CPU passively
		   game[gameid].playerStatus[game[gameid].curPlayer]==PLAYER_STATUS_WAITING))
		{
			#ifndef CLIENT
			// inactive surrender not possible in local games
			if(!game[gameid].playerSurrended[game[gameid].curPlayer]&&game[gameid].curTurn-game[gameid].playerLastActiveTurn[game[gameid].curPlayer]>=3)
			{
				logPrintf("!!! Player didn't do anything for 3 rounds, automatical surrender!\n");
				int score = dbPlayerPosition(gameid, game[gameid].curPlayer, game[gameid].position--);
	      byte data[NETWORK_MAX_DATA];
	      memset(data, 0, NETWORK_MAX_DATA);
	    	data[0] = game[gameid].curPlayer;
	    	data[1] = game[gameid].position+1;
	    	data[2] = score;
			  for(int ii=0;ii<NETWORK_MAX_THREADS;ii++)
			  {
			    if(threadGameIndex[ii]==gameid)
			    {
			      int j = threadPlayerIndex[ii];
						if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_QUIT)
					  {
					    logPrintf("SERVER >>> %d.%d MESSAGE_PLAYER_SURRENDED [%d]\n", gameid, j, ii);
					    serverMessage(threadIds[ii], MESSAGE_PLAYER_SURRENDED, data);
					  }
			    }
			  }

				game[gameid].playerSurrended[game[gameid].curPlayer] = true;
			}
			else
			#endif
				foundPlayer = true;
		}
		else
		{
			logPrintf("Player %d dead, skip to next one\n", game[gameid].curPlayer);
		}
		
		counter++;
		if(counter>=MAX_PLAYERS) // no "next player" found
			break;
	}
	
	return foundPlayer;
}

void processAttack(int gameid, int playerid, byte *data)
{
	int i,j;
  for(i=2;i<NETWORK_MAX_DATA;i++) data[i]=0;
	threadGameActivityTimer[gameid] = getTime();
	boolean playerWon = false;
	int cmp=0;
	if(networkMode==NETWORK_MODE_LOBBY) cmp = 1;

	int att=0;
	int def=0;
	for(i=0;i<game[gameid].areaDice[data[0]];i++)
	{
		data[7+i] = (rand()%6)+1;
		att += data[7+i];
	}
	for(i=0;i<game[gameid].areaDice[data[1]];i++)
	{
		data[15+i] = (rand()%6)+1;
		def += data[15+i];
	}
	
	data[2] = 1;
	if(att>def)
	{
		data[3] = game[gameid].areaDice[data[0]]-1;
		data[4] = PLAYER_ATTACKER;
	}
	else
	{
		data[3] = game[gameid].areaDice[data[1]];
		data[4] = PLAYER_DEFENDER;
	}
	data[5] = att;
	data[6] = def;

	if(networkMode!=NETWORK_MODE_LOCAL)
	{
		boolean died = false;
		int deadPlayer = 0;
		int deadScore = 0;
		int deadPosition = 0;
		if(data[4]==PLAYER_ATTACKER) // attacker won
		{
			playerWon = true;
			if(networkMode!=NETWORK_MODE_LOBBY) // don't update server's data quite yet
			{
				game[gameid].playerAreaCount[game[gameid].playerArea[data[0]]]++;
				game[gameid].playerAreaCount[game[gameid].playerArea[data[1]]]--;
				game[gameid].playerDiceCount[game[gameid].playerArea[data[1]]] -= game[gameid].areaDice[data[1]];
			}

			if(game[gameid].playerAreaCount[game[gameid].playerArea[data[1]]]==cmp&&
			   !game[gameid].playerSurrended[game[gameid].playerArea[data[1]]]) // player died
			{
				logPrintf("!!! PLAYER DIED: [%d.%d]\n", gameid, game[gameid].playerArea[data[1]]);
				deadPlayer = game[gameid].playerArea[data[1]];
				deadPosition = game[gameid].position;
				game[gameid].playerStatus[game[gameid].playerArea[data[1]]]=PLAYER_STATUS_DEAD;
				deadScore = dbPlayerPosition(gameid, game[gameid].playerArea[data[1]], game[gameid].position--);
				died = true;
			}

			if(networkMode!=NETWORK_MODE_LOBBY)
				game[gameid].playerArea[data[1]] = game[gameid].curPlayer;
		}
		else if(networkMode!=NETWORK_MODE_LOBBY)
			game[gameid].playerDiceCount[game[gameid].playerArea[data[0]]] -= game[gameid].areaDice[data[0]]-1;

		if(networkMode!=NETWORK_MODE_LOBBY)
		{
			game[gameid].areaDice[data[0]] = data[2];
			game[gameid].areaDice[data[1]] = data[3];
		}

    for(i=0;i<NETWORK_MAX_THREADS;i++)
    {
      if(threadGameIndex[i]==gameid)
      {
        j = threadPlayerIndex[i];
				if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_QUIT)
			  {
			    logPrintf("SERVER >>> %d.%d MESSAGE_ATTACK [%d]\n", gameid, j, i);
			    serverMessage(threadIds[i], MESSAGE_ATTACK, data);
			  }
      }
	  }
	  if(died)
	  {
	  	memset(data, 0, NETWORK_MAX_DATA);
	  	data[0] = deadPlayer;
	  	data[1] = deadPosition;
	  	data[2] = deadScore;
			data[3] = game[gameid].pot%256;
			data[4] = game[gameid].pot>>8;
	  	
	    for(i=0;i<NETWORK_MAX_THREADS;i++)
	    {
	      if(threadGameIndex[i]==gameid)
	      {
	        j = threadPlayerIndex[i];
					if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_QUIT)
				  {
				    logPrintf("SERVER >>> %d.%d MESSAGE_PLAYER_DIED [%d]\n", gameid, j, i);
				    serverMessage(threadIds[i], MESSAGE_PLAYER_DIED, data);
				  }
	      }
		  }
	  }
	}
	#ifdef CLIENT
	else // local game, send outcome just back to player
	{
		logPrintf("SERVER >>> %d.%d MESSAGE_ATTACK\n", gameid, playerid);
		serverMessage(pthread_self(), MESSAGE_ATTACK, data);
	}
	#endif
  
  // check if attacker is the only one left
	if(playerWon)
	{
		int numAreas=0;
		boolean gameEnded = true;
		for(i=0;i<NUM_AREAS;i++)
		{
			if(game[gameid].playerArea[i]!=playerid)
				numAreas++;
			int cmp = 0;
			if(networkMode==NETWORK_MODE_LOBBY) cmp = 1;
			if(numAreas>cmp)
				gameEnded = false;
		}
		if(gameEnded) // only player areas left - send end game message!
		{
			if(networkMode==NETWORK_MODE_TCP)
			{
				threadRequestGameTimer[gameid] = getTime()+2000;
				threadRequestGameEnd[gameid] = true;
			}
			#ifdef CLIENT
			else if(networkMode==NETWORK_MODE_LOBBY) // send end game messages to everyone
			{
			  #ifdef LOBBY
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! L:END GAME");
					#endif
				#endif
			  for(i=0;i<NETWORK_MAX_DATA;i++) data[i]=0;			  
				data[0] = game[gameid].curTurn;
				data[1] = game[gameid].curPlayer;
				data[2] = dbPlayerPosition(gameid, game[gameid].curPlayer, game[gameid].position--);
			  for(j=0;j<NETWORK_MAX_THREADS;j++)
			  {
			    if(threadGameIndex[j]==gameid)
			    {
			      int k = threadPlayerIndex[j];
						if(game[gameid].playerStatus[k]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[k]<PLAYER_STATUS_QUIT)
					  {
					    logPrintf("SERVER >>> %d.%d MESSAGE_END_GAME [%d]\n", gameid, k, j);
					    serverMessage(threadIds[j], MESSAGE_END_GAME, data);
					  }
			    }
			  }
			}
			else // local game, send data just back to player
			{
			  for(i=0;i<NETWORK_MAX_DATA;i++) data[i]=0;			  
				data[0] = game[gameid].curTurn;
				data[1] = game[gameid].curPlayer;
				data[2] = dbPlayerPosition(gameid, game[gameid].curPlayer, game[gameid].position--);
				logPrintf("SERVER >>> %d.%d MESSAGE_END_GAME\n", gameid, playerid);
		    serverMessage(pthread_self(), MESSAGE_END_GAME, data);
		  }
		  #endif

		  // clean up game
      logPrintf("!!! POST-ATTACK END GAME [%d]\n", gameid);
	  	game[gameid].gameStatus = GAME_STATUS_ENDED;
		}
	}
}

void endTurnLogic(int gameid)
{
	int i, j, k, maxConn, index;
	byte data[NETWORK_MAX_DATA];
	int playerid = game[gameid].curPlayer;
	#ifdef CLIENT
  pthread_t curThread = pthread_self();
  #endif
	maxConn = 1;
	index = 0;

	if(game[gameid].gameStatus!=GAME_STATUS_PLAYING) // in case someone's trying to end a turn while game is not on
		return;

	game[gameid].playerLastActiveTurn[playerid] = game[gameid].curTurn;
	logPrintf("!!! END TURN LOGIC: %d.%d\n", gameid, game[gameid].curPlayer);

	for(i=0;i<MAX_PLAYERS;i++) // TODO: seems like we have bug somewhere? better to reset player area count in the end of each turn!
		game[gameid].playerAreaCount[i] = 0;
	for(i=0;i<NUM_AREAS;i++)
	{
		game[gameid].playerAreaCount[game[gameid].playerArea[i]]++;
	}
//	for(i=0;i<MAX_PLAYERS;i++) // seems like we have bug somewhere, better to reset player area count in the end of each turn
//		logPrintf("Player[%d] areas: %d\n", i, game[gameid].playerAreaCount[i]);

//	logPrintf("End turn logic...\n");	

	#ifdef CLIENT
		if(networkMode!=NETWORK_MODE_TCP)
		{
			// local game, copy old values to a temp. buffer
			for(i=0;i<NUM_AREAS;i++)
				tempAreaDice[i] = game[gameid].areaDice[i];
			tempPlayer = playerid;
		}
	#endif
	
	if(!game[gameid].playerSurrended[playerid]) // don't add dice to surrended players
	{
		k=0;
		for(i=0;i<NUM_AREAS;i++)
		{
			if(game[gameid].playerArea[i]==playerid)
			{
				for(j=0;j<NUM_AREAS;j++) game[gameid].areaVisited[j] = false;
				game[gameid].areaConnectionCount=0;
		
				game[gameid].playerAreaIndex[playerid][k] = i;
				k++;
				connectionCount(gameid, i);
				if(game[gameid].areaConnectionCount>maxConn)
				{
					maxConn = game[gameid].areaConnectionCount;
					index = i;
				}
			}
		}
	
		int dReserve = game[gameid].diceReserve[playerid];
		for(i=0;i<maxConn+dReserve;i++) // add as many dice as we have max. area connections + the ones in reserve
		{
			boolean diceAdded = false;
			k = rand()%game[gameid].playerAreaCount[playerid];
			for(j=0;j<game[gameid].playerAreaCount[playerid];j++)
			{
				index = game[gameid].playerAreaIndex[playerid][(k+j)%game[gameid].playerAreaCount[playerid]];
				if(game[gameid].areaDice[index]<MAX_DICE)
				{
					game[gameid].areaDice[index]++;
					diceAdded = true;
					if(i>=maxConn) // added from the reserve
					{
						logPrintf("Using dice reserve for %d...\n", playerid);
						game[gameid].diceReserve[playerid]--;
					}
					break;
				}
			}
			if(!diceAdded&&i<maxConn) // no more areas where to add dice, put them in reserve instead
			{
				logPrintf("Adding to dice reserve for %d at %d...\n", playerid, i);
				game[gameid].diceReserve[playerid]++;
				if(game[gameid].diceReserve[playerid]>MAX_DICE_RESERVE)
				{
					logPrintf("MAX. dice reserve limit reached for %d!\n", playerid);
					game[gameid].diceReserve[playerid] = MAX_DICE_RESERVE;
					break;
				}
			}
		}
	
//		logPrintf("!!! DICE RESERVE for player %d: %d\n", playerid, game[gameid].diceReserve[playerid]);
		threadGameSleep[gameid] = getTime()+(maxConn*75)+1000; // how much we're waiting after piling all the dice
	}
	
	int prevPlayer = game[gameid].curPlayer;
	findNextPlayer(gameid);
	game[gameid].playerScore[game[gameid].curPlayer] -= 2;
	game[gameid].pot += 2;		

	int numDead = 0;
	for(i=0;i<MAX_PLAYERS;i++)
	{
		if(game[gameid].playerStatus[i]==PLAYER_STATUS_DEAD||game[gameid].playerSurrended[i])
			numDead++;
	}
	
	if(game[gameid].curPlayer==prevPlayer||numDead==game[gameid].numPlayers-1)
	{
		logPrintf("Only one player left - time to quit!\n");

		#ifndef CLIENT
			threadRequestGameTimer[gameid] = getTime()+1500;
			threadRequestGameEnd[gameid] = true;
      logPrintf("!!! POST-TURN END GAME [%d]\n", gameid);
	  	game[gameid].gameStatus = GAME_STATUS_ENDED;
	  #else
			// local game, send data just back to player
			int score = 0;
			if(game[gameid].gameStatus==GAME_STATUS_PLAYING)
				score = dbPlayerPosition(gameid, game[gameid].curPlayer, game[gameid].position--);
					
			data[0] = game[gameid].curTurn;
			data[1] = game[gameid].curPlayer;
			data[2] = score;

	    logPrintf("SERVER >>> %d.%d MESSAGE_END_GAME\n", gameid, game[gameid].curPlayer);
	    serverMessage(curThread, MESSAGE_END_GAME, data);
	  #endif
	}
	else // full status
	{
	  for(i=0;i<NETWORK_MAX_DATA;i++) data[i]=0;
	
	  data[0] = game[gameid].curTurn;
	  data[1] = game[gameid].curPlayer;
		data[2] = game[gameid].pot%256;
		data[3] = game[gameid].pot>>8;
	  for(i=0;i<NUM_AREAS;i++)
	  {
	  	data[4+MAX_PLAYERS*4+i] = game[gameid].playerArea[i];
	  	data[4+MAX_PLAYERS*4+NUM_AREAS+i] = game[gameid].areaDice[i];
	  }
		for(i=0;i<MAX_PLAYERS;i++)
		{
			data[4+i] = game[gameid].playerPosition[i];
			data[4+MAX_PLAYERS+i] = game[gameid].playerScore[i];
			data[4+MAX_PLAYERS*2+i] = game[gameid].playerStatus[i];
			data[4+MAX_PLAYERS*3+i] = game[gameid].playerSurrended[i];
			data[4+MAX_PLAYERS*4+NUM_AREAS*2+i] = game[gameid].diceReserve[i];
		}

		if(networkMode==NETWORK_MODE_TCP||(networkMode==NETWORK_MODE_LOBBY&&roomOwner!=0&&roomOwner==pthread_self()))
		{
	    for(i=0;i<NETWORK_MAX_THREADS;i++)
	    {
	      if(threadGameIndex[i]==gameid)
	      {
	        j = threadPlayerIndex[i];
					if(game[gameid].playerStatus[j]>PLAYER_STATUS_UNCONFIRMED&&game[gameid].playerStatus[j]<PLAYER_STATUS_QUIT)
				  {
				    logPrintf("SERVER >>> %d.%d MESSAGE_FULL_STATUS (end turn) [%d]\n", gameid, j, i);
				    serverMessage(threadIds[i], MESSAGE_FULL_STATUS, data);
				  }
	      }
		  }
		}
		#ifdef CLIENT
		else
		{
			// local game, send data just back to player
	    logPrintf("SERVER >>> %d.%d MESSAGE_FULL_STATUS\n", gameid, playerid);
	    serverMessage(curThread, MESSAGE_FULL_STATUS, data);
	  }
	  #endif
	}
}
