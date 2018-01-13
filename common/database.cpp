void dbStatisticsReset()
{
  gameStatistics.gamesPlayed = 0;
  gameStatistics.openGames = 0;
  gameStatistics.activeGames = 0;
  #ifdef CLIENT
	  gameStatistics.lastPlayerConnected = getTime()/1000;
	  gameStatistics.lastPlayerConnectedBuffer = getTime()/1000;
	  gameStatistics.lastGameStarted = getTime()/1000;
	  gameStatistics.lastGameEnded = getTime()/1000;
  #else
	  gameStatistics.lastPlayerConnected = time(NULL);
	  gameStatistics.lastPlayerConnectedBuffer = time(NULL);
	  gameStatistics.lastGameStarted = time(NULL);
	  gameStatistics.lastGameEnded = time(NULL);
	  gameStatistics.serverStarted = time(NULL);
  #endif
  gameStatistics.playersOnline = 0;

	#ifdef USE_DB
	logPrintf("::: DB - reset game statistics\n");
	char *query = (char*)malloc(128);
	sprintf(query, "SELECT count(1) FROM game WHERE gameEnded!=0");
	//logPrintf("q: %s\n", query);
	if(mysql_query(&mysql, query))
		logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));
	if(!(res = mysql_store_result(&mysql)))
		logPrintf("!!! SQL RES error: %s\n", mysql_error(&mysql));
	else
	{
		row = mysql_fetch_row(res);
		gameStatistics.gamesPlayed = atoi(row[0]);
		logPrintf("::: DB: games played: %d\n", gameStatistics.gamesPlayed);
	}
	memset(query, 0, 128);
	sprintf(query, "SELECT gameStarted FROM game ORDER BY gameStarted DESC LIMIT 0,1");
	//logPrintf("q: %s\n", query);
	if(mysql_query(&mysql, query))
		logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));
	if(!(res = mysql_store_result(&mysql)))
		logPrintf("!!! SQL RES error: %s\n", mysql_error(&mysql));
	else
	{
		row = mysql_fetch_row(res);
		gameStatistics.lastGameStarted = atoi(row[0]);
		logPrintf("::: DB: last game started: %d\n", gameStatistics.lastGameStarted);
	}
	memset(query, 0, 128);
	sprintf(query, "SELECT gameEnded FROM game ORDER BY gameEnded DESC LIMIT 0,1");
	//logPrintf("q: %s\n", query);
	if(mysql_query(&mysql, query))
		logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));
	if(!(res = mysql_store_result(&mysql)))
		logPrintf("!!! SQL RES error: %s\n", mysql_error(&mysql));
	else
	{
		row = mysql_fetch_row(res);
		gameStatistics.lastGameEnded = atoi(row[0]);
		logPrintf("::: DB: last game ended: %d\n", gameStatistics.lastGameEnded);
	}
	mysql_free_result(res);
	free(query);
	#endif  
}

void dbPlayerLink(int gameid, int playerid, char *MAChexed)
{
	#ifdef USE_DB
	char *query = (char*)malloc(128);
	time_t timeNow = time(NULL);

	logPrintf("::: DB - link player %d.%d (%s), MAC: %s\n", gameid, playerid, game[gameid].playerName[playerid], MAChexed);
	// check if a user with same MAC address exists
	sprintf(query, "SELECT playerId FROM player WHERE MAC='%s' AND name='%s'", MAChexed, game[gameid].playerName[playerid]);
//			logPrintf("q: %s\n", query);
	if(mysql_query(&mysql, query))
		logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));
	if(!(res = mysql_store_result(&mysql)))
		logPrintf("!!! SQL RES error: %s\n", mysql_error(&mysql));
	int nrows = mysql_num_rows(res);
	
	if(nrows==0)
	{
		logPrintf(">>> INSERT NEW PLAYER!\n");
		memset(query, 0, 128);
		sprintf(query, "INSERT INTO player (name, MAC, playerType, lastConnected) values ('%s', '%s', %d, %d)", game[gameid].playerName[playerid], MAChexed, PLAYER_TYPE_NETWORK, (int)timeNow);
//		logPrintf("query: %s\n", query);
		if(mysql_query(&mysql, query))
			logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));
		else
		{
			int insertId = mysql_insert_id(&mysql);
			playerIndexDB[gameid][playerid] = insertId;
		}
	}
	else
	{
		row = mysql_fetch_row(res);
		int playerId = atoi(row[0]);
		playerIndexDB[gameid][playerid] = playerId;

		memset(query, 0, 128);
		sprintf(query, "UPDATE player SET lastConnected=%d WHERE playerId=%d", (int)timeNow, playerId);
		logPrintf("query: %s\n", query);
		if(mysql_query(&mysql, query))
			logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));
	}
	logPrintf("::: DB - player [%d.%d] id in database: %d\n", gameid, playerid, playerIndexDB[gameid][playerid]);
	
	mysql_free_result(res);
	free(query);
	#endif
}

// returns player score
int dbPlayerPosition(int gameid, int playerid, int position)
{
	if(position<1) return 0; // bug somewhere? this should never occur
	game[gameid].playerPosition[playerid] = position;

	// calculate points:

	int points = 0;
	if(position==1)
		points = game[gameid].pot;
	else
	{
		points = (scoreTab[game[gameid].numPlayers-2][game[gameid].numPlayers-position]*game[gameid].pot)/10000;
		printf("%d. points: %d\n", position, points);
		logPrintf("::: DB - player points[%d.%d]: %d+%d out of %d = %d\n", gameid, playerid, game[gameid].playerScore[playerid], points, game[gameid].pot, (game[gameid].playerScore[playerid]+points));
		game[gameid].pot -= points;
		logPrintf("::: DB - pot now: %d\n", game[gameid].pot);
	}

	#ifdef USE_DB
	
	logPrintf("::: DB - add a database position record for player [%d.%d]: %d\n", gameid, playerid, position);
	char *query = (char*)malloc(128);
	sprintf(query, "INSERT INTO results (gameid, playerid, position, score) values (%d, %d, %d, %d)", gameIndexDB[gameid], playerIndexDB[gameid][playerid], position, (game[gameid].playerScore[playerid]+points));
	logPrintf("query: %s\n", query);
	if(mysql_query(&mysql, query))
		logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));

	memset(query, 0, 128);
	sprintf(query, "SELECT score FROM player WHERE playerId=%d", playerIndexDB[gameid][playerid]);
	logPrintf("q: %s\n", query);
	if(mysql_query(&mysql, query))
		logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));
	if(!(res = mysql_store_result(&mysql)))
		logPrintf("!!! SQL RES error: %s\n", mysql_error(&mysql));
	else
	{
		row = mysql_fetch_row(res);
		int score = atoi(row[0]);
		score += (game[gameid].playerScore[playerid]+points);
		if(score<0) // don't allow negative points
			score = 0;
		
		memset(query, 0, 128);
		sprintf(query, "UPDATE player SET score=%d WHERE playerId=%d", score, playerIndexDB[gameid][playerid]);
		logPrintf("q: %s\n", query);
		if(mysql_query(&mysql, query))
			logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));
	}
	mysql_free_result(res);
	free(query);
	#endif
	
	return (game[gameid].playerScore[playerid]+points);
}

void dbGameStart(int gameid)
{
	#ifdef USE_DB
	char *query = (char*)malloc(128);
	logPrintf("::: DB - create a database game record\n");
	time_t timeNow = time(NULL);
	sprintf(query, "INSERT INTO game (gameStarted) values (%d)", (int)timeNow);
//	logPrintf("query: %s\n", query);
	if(mysql_query(&mysql, query))
		logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));
	else
	{
		int insertId = mysql_insert_id(&mysql);
		gameIndexDB[gameid] = insertId;
	}
	logPrintf("::: DB - game %d id in database: %d\n", gameid, gameIndexDB[gameid]);
	free(query);
	#endif

  gameStatistics.lastGameStarted = time(NULL);
  gameStatistics.openGames--;
  gameStatistics.activeGames++;
}

void dbGameEnd(int gameid)
{
	#ifdef USE_DB
	char *query = (char*)malloc(128);
	logPrintf("::: DB - end game\n");
	time_t timeNow = time(NULL);
	sprintf(query, "UPDATE game SET gameEnded=%d WHERE gameid=%d", (int)timeNow, gameIndexDB[gameid]);
//	logPrintf("query: %s\n", query);
	if(mysql_query(&mysql, query))
		logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));

	logPrintf("::: DB - game %d id in database ended: %d\n", gameid, gameIndexDB[gameid]);
	free(query);
	#endif

	gameStatistics.lastGameEnded = time(NULL);
	gameStatistics.activeGames--;
	gameStatistics.gamesPlayed++;
}

void dbGameErase(int gameid)
{
	boolean playerPositioned[MAX_PLAYERS];
	for(int i=0;i<MAX_PLAYERS;i++)
		playerPositioned[i] = false;

	#ifdef USE_DB
	char *query = (char*)malloc(128);
	// clean up all the remaining positions
	logPrintf("::: DB - erase a game and fill up remaining positions\n");
	sprintf(query, "SELECT playerId FROM results WHERE gameid=%d", gameIndexDB[gameid]);
//	logPrintf("q: %s\n", query);
	if(mysql_query(&mysql, query))
		logPrintf("!!! SQL QUERY ERROR: %s, %s\n", query, mysql_error(&mysql));
	if(!(res = mysql_store_result(&mysql)))
		logPrintf("!!! SQL RES error: %s\n", mysql_error(&mysql));
	int nrows = mysql_num_rows(res);
	for(int i=0;i<nrows;i++)
	{
		row = mysql_fetch_row(res);
		int temp = atoi(row[0]);
		for(int j=0;j<game[gameid].numPlayers;j++)
		{
			if(playerIndexDB[gameid][j]==temp)
			{
				playerPositioned[j] = true;
				logPrintf("::: DB - found already positioned player: [%d.%d]\n", gameid, j);
				break;
			}
		}
	}
	
	mysql_free_result(res);
	logPrintf("::: DB - game positioning done, ready to be erased!\n");
	free(query);
	#endif
	
	boolean playersLeft = true;
	
	while(playersLeft) // loop players until all have been positioned
	{
		int leastAreas = 99;
		int leastIndex = -1;
		for(int i=0;i<game[gameid].numPlayers;i++)
		{
			if(!playerPositioned[i])
			{
				if(game[gameid].playerAreaCount[i]<leastAreas)
				{
					leastAreas = game[gameid].playerAreaCount[i];
					leastIndex = i;
				}
			}
		}
		if(leastIndex==-1)
		{
			playersLeft = false;
		}
		else
		{
			playerPositioned[leastIndex] = true;
			dbPlayerPosition(gameid, leastIndex, game[gameid].position--);
		}
	}
}
