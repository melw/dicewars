#ifdef USE_DB
	MYSQL mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;
#endif

int playerIndexDB[NETWORK_MAX_GAMES][MAX_PLAYERS];
int gameIndexDB[NETWORK_MAX_GAMES];

void dbPlayerLink(int gameid, int playerid, char *MAChexed);
int dbPlayerPosition(int gameid, int playerid, int position);

void dbGameStart(int gameid);
void dbGameEnd(int gameid);
void dbGameErase(int gameid);

