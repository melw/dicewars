#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>

#ifdef USE_DB
	#include "mysql/mysql.h"
#endif

#include "../../common/common.h"
#include "../../common/dicewars_version.h"
#include "../../common/gamelogic.h"
#include "../../common/network.h"
#include "../../common/inifile.h"
#include "../../common/database.h"
#include "../../common/gamestatistics.h"

gameData game[NETWORK_MAX_GAMES];
gameStatistics gameStatistics;
boolean serverQuitRequested = false;

FILE *logFile;
time_t seconds;

static void *gameThread(void *);
static void *clientThread(void *);
static void *aiThread(void *arg);
static void *consoleThread(void *arg);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void logPrintf(char *msg, ...)
{
	struct tm *ptr;
	time_t tm;
	char str[256];
	
	tm = time(NULL);
	ptr = localtime(&tm);
	strftime(str, 100, "[%H:%M:%S %d.%m.%Y] ", ptr);
	printf(str);

	fprintf(logFile, str);
	
	if(msg)
	{
		va_list argp;
		va_start(argp, msg);
		vprintf(msg, argp);
		vfprintf(logFile, msg, argp);
		va_end(argp);
	}

	fflush(stdout);
}

#include "../../common/gamelogic.cpp"
#include "../../common/network.cpp"
#include "../../common/inifile.cpp"
#include "../../common/database.cpp"

int main(int argc, char *argv[])
{
	logFile = fopen("dicewars.log", "a");
	logPrintf("\n");
	logPrintf("\n");
	logPrintf("\n");
	logPrintf("*******************************************\n");
  logPrintf("*** Initializing DiceWars server v%s ***\n", DICEWARS_VERSION);
	logPrintf("*******************************************\n");

  networkMode = NETWORK_MODE_TCP;

  int       *iptr;
  pthread_t  tid;

  SOCKET listen_sock, client_len;
  int i;
  struct sockaddr_in serv_addr;
  struct sockaddr *cli_addr;

  initIniFile("dicewars.ini");
  serverPort = readIniFileInt("serverPort");
  #ifdef USE_DB
		char *dbAddress = readIniFileString("dbAddress");
	  char *dbUser = readIniFileString("dbUser");
	  char *dbPassword = readIniFileString("dbPassword");

		logPrintf("*** Initializing MySQL...\n");
		mysql_init(&mysql);

		// enable automatic reconnecting
		my_bool reconnect = 1;
		mysql_options(&mysql, MYSQL_OPT_RECONNECT, &reconnect);
		
		logPrintf("*** Connecting to MySQL database...\n");
		
		/* Connect to database */
		if (!mysql_real_connect(&mysql, dbAddress, dbUser, dbPassword, "dicewars", 0, NULL, 0)) {
			logPrintf("!!! MySQL ERROR: %s\n", mysql_error(&mysql));
			exit(0);
		}
		else
			logPrintf("*** Connected to database at %s\n", dbAddress);
	#endif
	closeIniFile();
	
	if(argc>1)
		serverPort = atoi(argv[1]);

  for(i=0;i<NETWORK_BUFFER_LENGTH;i++)
    threadBufferIds[i]=0;
  for(i=0;i<NETWORK_MAX_THREADS;i++)
  {
    threadGameIndex[i]=threadPingCounter[i]=threadPongCounter[i]=-1;
    threadPingTimer[i]=0;
    threadIds[i]=0;
    threadConnected[i]=false;
  }
  dbStatisticsReset();

  if ( (listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    logPrintf("!!! Couldn't create socket: %s\n", strerror(errno));
    exit(1);
  }

  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port = htons(serverPort);

	i=1;
	setsockopt( listen_sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i) );
  
  if ( (bind(listen_sock,(struct sockaddr *) &serv_addr, sizeof(serv_addr))) != 0) {
    logPrintf("!!! Socket bind error: %s\n", strerror(errno));
    exit(1);
  }
  listen(listen_sock, 5);

  cli_addr = (struct sockaddr*)malloc(sizeof(serv_addr));

	// create one thread for processing game logic
	pthread_create(&tid, NULL, &gameThread, NULL);

	// create one thread for processing the console input
	pthread_create(&tid, NULL, &consoleThread, NULL);

	logPrintf("*** Server init ok, listening port: %d\n", serverPort);

  while(true)
  {
    for ( ; ; ) {
      client_len = sizeof(cli_addr);
      iptr = (int*)malloc(sizeof(int));
      *iptr = accept(listen_sock, (struct sockaddr *)&cli_addr, (socklen_t*)&client_len);
      logPrintf("Client connected...\n");

      pthread_create(&tid, NULL, &clientThread, iptr);
    }
  }

	fclose(logFile);
  #ifdef USE_DB
  	mysql_close(&mysql);
  #endif
  return 0;
}

static void *clientThread(void *arg)
{
//	logPrintf("beginning of client thread\n");
  SOCKET connfd;
  int i;
  int threadIndex=-1;
  int gameid=-1;
  int playerid=-1;
  byte pingBuffer[NETWORK_MAX_DATA];
  pthread_t curThread = pthread_self();

  connfd = *((SOCKET *) arg);
  // TODO: crashing here?
  free(arg);

	for(i=0;i<NETWORK_MAX_THREADS;i++)
	{
		if(threadIds[i]==curThread) // found thread id
		{
		  logPrintf("We already have a thread index: %d\n", i);
			threadIndex = i;
			break;
		}
	}
	for(i=0;i<NETWORK_MAX_GAMES;i++)
	{
		threadGameAI[i] = false;
		threadRequestGameEnd[i] = false;
	}
	if(threadIndex==-1) // fresh thread, create an index
	{
		for(i=0;i<NETWORK_MAX_THREADS;i++)
		{
			if(!threadConnected[i]) // free slot
			{
			  logPrintf("Assign index %d for the thread\n", i);
				threadIds[i]=curThread;
				threadPingCounter[i]=threadPongCounter[i]=-1;
				threadIndex=i;
				break;
			}
		}
	}

	ioctl(connfd,FIONBIO,&nbMode); // set non-blocking
	
  pthread_detach(curThread);

  while(1) // loop forever --- TODO: exit when player drops / quits (partially done)
  {
    int result=0;
    FD_ZERO(&server_set);
    FD_SET(connfd, &server_set);
    struct timeval tval;
    tval.tv_sec  = 0;
    tval.tv_usec = 1;
	
	  // TODO - could do better...
	  result = select(connfd + 1, &server_set, NULL, NULL, &tval);
	  if (FD_ISSET(connfd, &server_set))
	    receiveClientData(connfd);
    for(i=0;i<NETWORK_BUFFER_LENGTH;i++)
    {
      if(threadBufferIds[i]==curThread) // we have data to send!
      {
//      	logPrintf("Lock CLIENT thread %d for send\n", pthread_self());
				pthread_mutex_lock(&mutex);
//				logPrintf("Lock CLIENT succeeded for %d\n", pthread_self());
				
//				logPrintf("Thread CLIENT [%d]: Sending from index: %d\n", pthread_self(), i);
				sendServerData(connfd, threadSendBuffer[i]);
//				logPrintf(".. free threadbuffer\n");
				threadBufferIds[i]=0; // free buffer for others to use
//      	logPrintf("Unlock CLIENT thread %d after send\n", pthread_self());
				pthread_mutex_unlock(&mutex);
      }
    }

		// check if we have any ping timeouts
		if(threadConnected[threadIndex])
		{
			if(threadPongCounter[threadIndex]!=threadPingCounter[threadIndex]&&getTime()>threadPingTimer[threadIndex]+NETWORK_PING_TIMEOUT) // no answer from client
			{
				logPrintf("!!! %d.%d PING TIMEOUT! %d\n", gameid, playerid, threadPongCounter[threadIndex]);
	      game[gameid].playerStatus[playerid]=PLAYER_STATUS_DROPPED;
	      byte data[NETWORK_MAX_DATA];
	      memset(data, 0, NETWORK_MAX_DATA);
	    	data[0] = threadPlayerIndex[threadIndex];
	    	boolean onlyPlayer = true;
	    	if(gameid!=-1&&playerid!=-1)
	    	{
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
				  else if(game[gameid].numPlayers>1)
				  {
			  		if(game[gameid].gameStatus==GAME_STATUS_PLAYING&&game[gameid].curPlayer==playerid) // end turn properly
							endTurnLogic(gameid);
					}
			  	if(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS)
			  	{
						logPrintf("!!! Decrease the number of players in game %d from %d to %d\n", gameid, game[gameid].numPlayers, game[gameid].numPlayers-1);
			  		game[gameid].numPlayers--;
			  	}
				}
	    	threadIds[threadIndex] = 0; // delete thread association to this player
	    	threadGameIndex[threadIndex] = -1; // delete game index association as well
	    	if(threadConnected[threadIndex]) gameStatistics.playersOnline--;
	    	threadConnected[threadIndex] = false; // delete connection status
				break; // raw game -> close socket connection
			}
			
			// time to send a new ping?
			if(threadPingTimer[threadIndex]+NETWORK_PING_INTERVAL<getTime())
			{
	      gameid=threadGameIndex[threadIndex];
	      playerid=threadPlayerIndex[threadIndex];
				if(++threadPingCounter[threadIndex]>255) threadPingCounter[threadIndex]=0;
				threadPingTimer[threadIndex]=getTime();
				logPrintf("SERVER >>> %d.%d MESSAGE_PING: %d\n", gameid, playerid, threadPingCounter[threadIndex]);
				pingBuffer[0] = threadPingCounter[threadIndex];

				if(gameid==-1||game[gameid].gameStatus<=GAME_STATUS_WAITING_PLAYERS||threadPingCounter[threadIndex]==0) //  include running statistics:
				{
					long seconds   = time(NULL);
					pingBuffer[1]  = gameStatistics.gamesPlayed%256;
					pingBuffer[2]  = gameStatistics.gamesPlayed>>8;
					pingBuffer[3]  = gameStatistics.openGames;
					pingBuffer[4]  = gameStatistics.activeGames;
					pingBuffer[5]  = ((seconds-gameStatistics.lastPlayerConnected)/60)%256;
					pingBuffer[6]  = ((seconds-gameStatistics.lastPlayerConnected)/60)>>8;
					pingBuffer[7]  = ((seconds-gameStatistics.lastGameStarted)/60)%256;
					pingBuffer[8]  = ((seconds-gameStatistics.lastGameStarted)/60)>>8;
					pingBuffer[9]  = ((seconds-gameStatistics.lastGameEnded)/60)%256;
					pingBuffer[10] = ((seconds-gameStatistics.lastGameEnded)/60)>>8;
					pingBuffer[11] = gameStatistics.playersOnline%256;
					pingBuffer[12] = gameStatistics.playersOnline>>8;
					
					logPrintf("games played: %d\n", gameStatistics.gamesPlayed);
					logPrintf("open games: %d\n", gameStatistics.openGames);
					logPrintf("active games: %d\n", gameStatistics.activeGames);
				}

				serverMessage(curThread, MESSAGE_PING, pingBuffer);
			}
		}

  }
  close(connfd);                  /* done with connected socket */
  return(NULL);
}

static void *gameThread(void *arg)
{
  pthread_t  tid;
/* 	logPrintf("Before game free()\n");
	free(arg);
 	logPrintf("After game free()\n");*/
  logPrintf("*** Initialize game thread!\n");
  byte data[NETWORK_MAX_DATA];

	int lastUpdate = getTime();
	
  while(true)
  {
  	if(getTime()-lastUpdate>299000)
  	{
  		lastUpdate = getTime();
  		logPrintf("... Game thread idle\n");
  	}
  	for(int i=0;i<NETWORK_MAX_GAMES;i++)
  	{
  		int gameid = i;
  		if(threadGameAI[gameid])
  		{
				logPrintf("*** Create AI thread for the game %d\n", i);
				int er = pthread_create(&tid, NULL, &aiThread, (void *)i);
				if(er!=0)
					logPrintf("!!! Thread create error: %s", strerror(errno));
				threadGameAI[gameid] = false;
			}
			if(game[gameid].gameStatus==GAME_STATUS_PLAYING&&(threadGameActivityTimer[gameid]+30000)<getTime()) // trigger after 30 seconds of inactivity
			{
				threadGameActivityTimer[gameid] = getTime(); // reset timer
				logPrintf("*** Game %d activity timer - end turn!\n", gameid); 
				endTurnLogic(gameid);
			}
			if(threadRequestGameEnd[gameid]&&threadRequestGameTimer[gameid]<getTime())
			{
				logPrintf("*** Request game end for the game: %d\n", gameid);
				
				if(game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS)
				{
					if(game[gameid].numPlayers<=0) // no players left
					{
						logPrintf("Was waiting players - just update open games\n");
					  gameStatistics.openGames--;				  	
				  	game[gameid].gameStatus = GAME_STATUS_NONEXISTING;
				  }
				  else
				  {
				  	logPrintf("Player rejoined? (numPlayers: %d) Skip erasing the game...\n", game[gameid].numPlayers);
				  }
				}
				else
				{
					if(game[gameid].gameStatus==GAME_STATUS_ENDED)
					{
						logPrintf("END game\n");
					  for(int j=0;j<NETWORK_MAX_DATA;j++) data[j]=0;

						data[0] = game[gameid].curTurn;
						data[1] = game[gameid].curPlayer;
						data[2] = dbPlayerPosition(gameid, game[gameid].curPlayer, game[gameid].position--);
					  for(int j=0;j<NETWORK_MAX_THREADS;j++)
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

				  	dbGameEnd(gameid);
				  }
				  else
				  {
				  	boolean endGame = true;
				  	for(int j=0;j<MAX_PLAYERS;j++)
				  	{
				  		if(game[gameid].playerType[j]==PLAYER_TYPE_NETWORK&&game[gameid].playerStatus[j]>=PLAYER_STATUS_READY&&game[gameid].playerStatus[j]<=PLAYER_STATUS_WAITING)
				  			endGame = false;
				  	}

						if(endGame)
						{
				      logPrintf("!!! ERASE GAME [%d]\n", gameid);
							if(game[gameid].gameStatus==GAME_STATUS_PLAYING)
							{
								dbPlayerPosition(gameid, game[gameid].curPlayer, game[gameid].position--);
					  		dbGameErase(gameid);
				  		}
				  		game[gameid].gameStatus = GAME_STATUS_ENDED;
				  		dbGameEnd(gameid); // should we do this? let's try...
				  	}
				  	else
				  	{
				  		logPrintf("player rejoin? don't erase the game...\n");
				  	}
				  }
			  }
				
				threadRequestGameEnd[gameid]=false;
			}
		}
		sleep(1);
		if(serverQuitRequested) break;
	}
  logPrintf("*** Exit game thread!\n");
  return NULL;
}

static void *consoleThread(void *arg)
{
  char c;
  logPrintf("*** Initialize console thread!\n");
  while(true)
  {
  	c = getchar();
  	switch(c)
  	{
  		// game info
  		case 'g':
  		case 'G':
			{
				int counter=0;
				for(int i=0;i<NETWORK_MAX_THREADS;i++)
				{
					if(threadGameIndex[i]!=-1)
					{
						printf("[CONSOLE] Game %d: status %d, players: %d\n", threadGameIndex[i], game[threadGameIndex[i]].gameStatus, game[threadGameIndex[i]].numPlayers);
						counter++;
					}
				}
				printf("[CONSOLE] Active games: %d\n", counter);
			}
  		break;

  		// help
  		case 'h':
  		case 'H':
			{
				printf("[CONSOLE] Help - H\n");
				printf("[CONSOLE] Games info - G\n");
				printf("[CONSOLE] Player info - P\n");
				printf("[CONSOLE] Quit server - Q\n");
				printf("[CONSOLE] Server statistics - S\n");
			}
  		break;

  		// player/connection info
  		case 'p':
  		case 'P':
			{
				int counter=0;
				for(int i=0;i<NETWORK_MAX_THREADS;i++)
				{
					if(threadIds[i])
					{
						printf("[CONSOLE] Player[%d]: %s, connected: %d, status: %d, game: %d, score: %d, areas: %d, dice: %d\n", 
						       i, threadPlayerName[i], threadConnected[i], game[threadGameIndex[i]].playerStatus[threadPlayerIndex[i]], threadGameIndex[i],
						       game[threadGameIndex[i]].playerScore[threadPlayerIndex[i]], game[threadGameIndex[i]].playerAreaCount[threadPlayerIndex[i]],
						       game[threadGameIndex[i]].playerDiceCount[threadPlayerIndex[i]]);
						counter++;
					}
				}
				printf("[CONSOLE] Connected players: %d\n", counter);
			}
  		break;

  		// quit server
  		case 'q':
  		case 'Q':
			{
				printf("[CONSOLE] Requested server quit!\n");
				serverQuitRequested = true;
			}
  		break;

  		// statistics
  		case 's':
  		case 'S':
			{
				printf("[CONSOLE] Open games: %d\n", gameStatistics.openGames);
				printf("[CONSOLE] Active games: %d\n", gameStatistics.activeGames);
				printf("[CONSOLE] Games played: %d\n", gameStatistics.gamesPlayed);
				printf("[CONSOLE] Players online: %d\n", gameStatistics.playersOnline);
				printf("[CONSOLE] Last player connected: %d\n", gameStatistics.lastPlayerConnected);
				printf("[CONSOLE] Last game started %d\n", gameStatistics.lastGameStarted);
				printf("[CONSOLE] Last game ended: %d\n", gameStatistics.lastGameEnded);
				time_t seconds = time(NULL);
				printf("[CONSOLE] Time now: %d\n", (int)seconds);
			}
  		break;
  	}
		if(serverQuitRequested) break;
	}
  logPrintf("*** Exit console thread!\n");
  sleep(1);
	fclose(logFile);
  #ifdef USE_DB
  	mysql_close(&mysql);
  #endif
  exit(-1);
  //return NULL;
//  pthread_exit(NULL);
}

static void *aiThread(void *arg)
{
	logPrintf("AI Thread start...\n");
//	long miniSleep;
  int gameid = ((int) arg);
  // TODO: mem leaks, but commenting this helps to crashing :(
//  free(arg);
	logPrintf("!!! Started AI thread for game: %d\n", gameid);
	sleep(5);
	while(game[gameid].gameStatus==GAME_STATUS_PLAYING||game[gameid].gameStatus==GAME_STATUS_WAITING_PLAYERS)
	{
	  if(game[gameid].gameStatus==GAME_STATUS_PLAYING&&
	     game[gameid].playerType[game[gameid].curPlayer]==PLAYER_TYPE_CPU&&
	     game[gameid].playerStatus[game[gameid].curPlayer]!=PLAYER_STATUS_DEAD)
	  {
	  	logPrintf("!!! AI logic %d.%d\n", gameid, game[gameid].curPlayer);
			//miniSleep = getTime();
			//do{ ; }while(getTime()-miniSleep<1000);
			while(getTime()<(unsigned long)threadGameSleep[gameid])
			{
				// do nothing - just wait while the requested sleep amount has gone away
			}
			
//			sleep(1); 
			while (!processAI(gameid))
			{
				//miniSleep = getTime();
				//do{ ; }while(getTime()-miniSleep<2000);
				sleep(2);
			}
			if(game[gameid].gameStatus==GAME_STATUS_PLAYING) // in case the game has ended already
			{
				game[gameid].playerLastActiveTurn[game[gameid].curPlayer] = game[gameid].curTurn;
				endTurnLogic(gameid);
			}
	  }
	  else if(game[gameid].playerType[game[gameid].curPlayer]==PLAYER_TYPE_NETWORK&&
	          game[gameid].playerStatus[game[gameid].curPlayer]>=PLAYER_STATUS_QUIT) // end turn for inactive players
	  {
			if(game[gameid].gameStatus==GAME_STATUS_PLAYING) // in case the game has ended already
			{
				endTurnLogic(gameid);
				//miniSleep = getTime();
				//do{ ; }while(getTime()-miniSleep<2000);
				sleep(2);
			}
	  }
	}
	logPrintf("!!! Deleted AI thread for game: %d\n", gameid);
	return(NULL);	
}


unsigned long getTime()
{
	seconds = time (NULL);
	return seconds*1000; // TODO: now this is bad...
	//  return SDL_GetTicks();
}
