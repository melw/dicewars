#include "common.h"
#include "../data/field.h"

void update(int gameid)
{
	game[gameid].gameTime = getTime()-game[gameid].startTime;
	
	switch(gameState)
	{
		#ifdef CLIENT
		case GAME_STATE_INGAME:
		case GAME_STATE_INGAME_START:
		case GAME_STATE_INGAME_WAITING:
		case GAME_STATE_INGAME_POST_ATTACK:
		case GAME_STATE_INGAME_POST_STATUS:
		case GAME_STATE_INGAME_WAITING_TURN:
		case GAME_STATE_INGAME_MENU:
		case GAME_STATE_INGAME_END:
			gameLogic();
			break;

		case GAME_STATE_MENU:
			menuLogic();
			break;

		case GAME_STATE_KEYBOARD:
			kbdLogic();
		#endif
	}
	
	#ifdef CLIENT
	if(networkInitialized&&networkStatus<=NETWORK_STATUS_CONNECTION_LOST&&networkStatus!=NETWORK_STATUS_WRONG_VERSION)
	{
		flushReceiveBuffer();

		if(networkStatus==NETWORK_STATUS_CONNECTED)
		{
			if(pongCounter!=pingCounter&&getTime()>pingTimer+NETWORK_PING_TIMEOUT) // no answer from server
			{
				logPrintf("!!! PING TIMEOUT! %d\n", pongCounter);
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "!!! PING TIMEOUT");
				playerDataUpdated = true;
				#endif
				networkStatus = NETWORK_STATUS_CONNECTION_LOST;
				resetNetworkConnection();
				reconnectNetwork = true;
			}
			
			if(pingTimer+NETWORK_PING_INTERVAL<getTime()) // new ping
			{
				if(++pingCounter>255) pingCounter=0;
				pingTimer=getTime();
				logPrintf("Send PING %d\n", pingCounter);
				byte pingBuffer[NETWORK_MAX_DATA];
				pingBuffer[0] = MESSAGE_PING;
				pingBuffer[1] = pingCounter;
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> PING %d", pingBuffer[1]);
				playerDataUpdated = true;
				#endif
				sendClientData(pingBuffer);
			}
		}
		else if(networkStatus!=NETWORK_STATUS_WRONG_VERSION&&reconnectNetwork) // re-connecting after a lost connection
		{
			if(!initNetworkConnection())
			{
				logPrintf("!!! Re-connection failed - try again!\n");
				resetNetworkConnection();
			}
			else if(networkInitCounter==7)
			{
				networkStatus = NETWORK_STATUS_CONNECTED;
				networkConnected = true;
				reconnectNetwork = false;
				logPrintf("!!! Re-connection succesfull at %d!\n", networkInitCounter);
				clientJoinGame(gameId);	
			}
		}
	}
	#endif
}

void initTimer(int gameid)
{
	game[gameid].startTime = getTime();
	game[gameid].gameTime = 0;
}

boolean areasConnected(int gameid, int area1, int area2)
{
	for(int i=0;i<MAX_AREA_CONNECTIONS;i++)
	{
		if(game[gameid].areaConnections[area1][i] == area2)
			return true;
	}
	return false;
}

void connectionCount(int gameid, int areaNum)
{
	int j;
	int areaNum2;
	
	for(j=0;j<MAX_AREA_CONNECTIONS;j++)
	{
		areaNum2 = game[gameid].areaConnections[areaNum][j];
		if(areaNum2==AREA_NONE)
		{
			return;
		}
		else
		{
			if(game[gameid].playerArea[areaNum2]==game[gameid].curPlayer)
			{
				if(!game[gameid].areaVisited[areaNum2])
				{
					game[gameid].areaVisited[areaNum2] = true;
					game[gameid].areaConnectionCount++;
					connectionCount(gameid, areaNum2);
				}
				else
				{
//					curConn = connectionCount(conn, areaNum2);
				}
			}
		}
	}
}

// angle 0-5
int fieldPosX(int index, int angle)
{
	int x = index%FIELD_X;
	int y = index/FIELD_X;
	int xAdd = 0;
	if(y%2==1) xAdd = hexagonWidth>>1;
	
	x = x*hexagonWidth+hexagonWidth+xAdd-1;
	
	switch(angle)
	{
		case 1:	
		case 2:
			x += (hexagonWidth>>1);
			break;
		case 4:	
		case 5:
			x -= (hexagonWidth>>1);
			break;
	}
	
	return x;
}

int fieldPosY(int index, int angle)
{
	int y = index/FIELD_X;
	y = y*hexagonHeight+hexagonHeight;
	switch(angle)
	{
		case 0:
			y -= ((hexagonHeight>>2)+(hexagonHeight>>1));
			break;
		case 1:
		case 5:	
			y -= ((hexagonHeight>>3)+(hexagonHeight>>2));
			break;
		case 2:	
		case 4:
			y += ((hexagonHeight>>3)+(hexagonHeight>>2));
			break;
		case 3:
			y += ((hexagonHeight>>2)+(hexagonHeight>>1));
			break;
	}	
	return y;
}

void traceBorder(int gameid, int area)
{
	int x = game[gameid].curIndex%FIELD_X;
	int y = game[gameid].curIndex/FIELD_X;
	int i;
	
	switch(game[gameid].curAngle)
	{
		case 0:
			if(y%2==1) x++;
			y--;
			break;
		case 1:
			x++;
			break;
		case 2:
			if(y%2==1) x++;
			y++;
			break;
		case 3:
			if(y%2==0) x--;
			y++;
			break;
		case 4:
			x--;
			break;
		case 5:
			if(y%2==0) x--;
			y--;
			break;
	}

	if(game[gameid].fieldArea[x+y*FIELD_X]==area) // change index & angle
	{
		game[gameid].curIndex = x+y*FIELD_X;
		game[gameid].curAngle--;
		if(game[gameid].curAngle<0) game[gameid].curAngle = 5;
	}
	else // change the angle, add area to connection list if needed
	{
		game[gameid].curAngle++;
		if(game[gameid].curAngle>5) game[gameid].curAngle = 0;
		
		if(x<0||y<0||x>=FIELD_X||y>=FIELD_Y) // out of bounds 
			return;
		for(i=0;i<MAX_AREA_CONNECTIONS;i++)
		{
			if(game[gameid].areaConnections[area][i]==AREA_NONE)
			{
				if(game[gameid].fieldArea[x+y*FIELD_X]>AREA_NONE)
				{
					game[gameid].areaConnections[area][i] = game[gameid].fieldArea[x+y*FIELD_X];
				}
				break;
			}
			else if(game[gameid].areaConnections[area][i]==game[gameid].fieldArea[x+y*FIELD_X]) // area is already added to connections list
			{
				break;
			}
		}
	}
}

void initAreaBorders(int gameid, int area)
{
	int i,startIndex,startAngle;
	startIndex=0;
	for(i=0;i<FIELD_SIZE;i++)
	{
		if(game[gameid].fieldArea[i]==area)
		{
			startIndex = i;
			break;
		}
	}
	for(i=0;i<MAX_AREA_CONNECTIONS;i++)
		game[gameid].areaConnections[area][i] = AREA_NONE;
	
	startAngle = 0;
	int x = fieldPosX(startIndex, startAngle);
	int y = fieldPosY(startIndex, startAngle);
	int startX=x;
	int startY=y;
	game[gameid].borderListX[area][0] = x;
	game[gameid].borderListY[area][0] = y;
	game[gameid].curIndex = startIndex;
	game[gameid].curAngle = startAngle;
	
	i=1;
	do
	{
		traceBorder(gameid, area);
			
		x = fieldPosX(game[gameid].curIndex, game[gameid].curAngle);
		y = fieldPosY(game[gameid].curIndex, game[gameid].curAngle);
		
		game[gameid].borderListX[area][i] = x;
		game[gameid].borderListY[area][i] = y;
		i++;
		if(i>=MAX_AREA_BORDERS) break; // this shouldn't happen ever!
	} while(x!=startX||y!=startY);
}

void initAreaDice(int gameid, int area)
{
	int i,prev,prev2,first,counter;
	int firstnon = -1;
	int lastnon = FIELD_SIZE-1;
	boolean foundWide=false;
	
	prev=prev2=first=-1;
	counter=0;
	
	for(i=FIELD_SIZE-1;i>=0;i--)
	{
		if(game[gameid].fieldArea[i]==area)
		{
			if(i>=FIELD_X&&firstnon==-1)
				firstnon = i;
			if(i%FIELD_X<lastnon%FIELD_X)
				lastnon = i;
				
			if(prev2==prev+1)
			{
				foundWide=true;
				break;
			}
			if(prev==-1) // first possible dice positioning in the lower right corner
				first = i;
			else if(prev==first&&i<prev-1&&counter<2) // dice rather more on the left than most-right side of pointy area
				first = i;
			prev2=prev;
			prev=i;
			counter++;
		}
	}
	
	if(foundWide)
	{
		game[gameid].diceIndex[area] = prev;
		game[gameid].areaDiceWide[area] = foundWide;
	}
	else // no wide spots in area found, just use the lowest one
	{
		game[gameid].diceIndex[area] = first;
		game[gameid].areaDiceWide[area] = foundWide;
	}
	// no top hexagons allowed
	if(game[gameid].diceIndex[area]<FIELD_X&&firstnon!=-1)
	{
		game[gameid].diceIndex[area] = firstnon;
		if(game[gameid].fieldArea[firstnon+1] == area)
			game[gameid].areaDiceWide[area] = true;
		else
			game[gameid].areaDiceWide[area] = false;
	}
	
	int x = game[gameid].diceIndex[area]%FIELD_X;
	int y = game[gameid].diceIndex[area]/FIELD_X;
	
	// no bottom right corner allowed
	if(x>FIELD_X-4&&y>FIELD_Y-4)
	{
		game[gameid].diceIndex[area] = lastnon;
		if(game[gameid].fieldArea[lastnon+1] == area)
			game[gameid].areaDiceWide[area] = true;
		else
			game[gameid].areaDiceWide[area] = false;
	}

	// no bottom left corner allowed either
	if(x<3&&y<3)
	{
		game[gameid].diceIndex[area] = firstnon;
		if(game[gameid].fieldArea[firstnon+1] == area)
			game[gameid].areaDiceWide[area] = true;
		else
			game[gameid].areaDiceWide[area] = false;
	}
}

boolean fieldConnected(int gameid, int f, int area)
{
	int row = (f/FIELD_X);
	int rowAdd = 0;
	if(row%2==0) rowAdd = -1;

	if(area==FIELD_ANY)
	{
		if((f%FIELD_X>0            && (FIELD_X+f-1)%FIELD_X!=FIELD_X-1 && game[gameid].fieldArea[f-1]>FIELD_NONE               ) ||
		   (f%FIELD_X<FIELD_X      && (f+1)%FIELD_X!=0                 && game[gameid].fieldArea[f+1]>FIELD_NONE               ) ||
		   (f>FIELD_X+1            && (f+rowAdd)%FIELD_X!=FIELD_X-1    && game[gameid].fieldArea[f-FIELD_X+rowAdd]>FIELD_NONE  ) ||
		   (f>FIELD_X              && (f+rowAdd+1)%FIELD_X!=0          && game[gameid].fieldArea[f-FIELD_X+rowAdd+1]>FIELD_NONE) ||
		   (f<FIELD_SIZE-FIELD_X-1 && (f+rowAdd)%FIELD_X!=FIELD_X-1    && game[gameid].fieldArea[f+FIELD_X+rowAdd]>FIELD_NONE  ) ||
		   (f<FIELD_SIZE-FIELD_X   && (f+rowAdd+1)%FIELD_X!=0          && game[gameid].fieldArea[f+FIELD_X+rowAdd+1]>FIELD_NONE))
			return true;
	}
	else
	{
		if((f%FIELD_X>0            && (FIELD_X+f-1)%FIELD_X!=FIELD_X-1 && game[gameid].fieldArea[f-1]==area               ) ||
		   (f%FIELD_X<FIELD_X      && (f+1)%FIELD_X!=0                 && game[gameid].fieldArea[f+1]==area               ) ||
		   (f>FIELD_X+1            && (f+rowAdd)%FIELD_X!=FIELD_X-1    && game[gameid].fieldArea[f-FIELD_X+rowAdd]==area  ) ||
		   (f>FIELD_X              && (f+rowAdd+1)%FIELD_X!=0          && game[gameid].fieldArea[f-FIELD_X+rowAdd+1]==area) ||
		   (f<FIELD_SIZE-FIELD_X-1 && (f+rowAdd)%FIELD_X!=FIELD_X-1    && game[gameid].fieldArea[f+FIELD_X+rowAdd]==area  ) ||
		   (f<FIELD_SIZE-FIELD_X   && (f+rowAdd+1)%FIELD_X!=0          && game[gameid].fieldArea[f+FIELD_X+rowAdd+1]==area))
			return true;
	}
	
	return false;
}

int getFreeFieldBorder(int gameid, int f)
{
	int i;
	int numBorders=0;
	
	int row = (f/FIELD_X);
	int rowAdd = 0;
	if(row%2==0) rowAdd = -1;
	
	if(f%FIELD_X>0            && (FIELD_X+f-1)%FIELD_X<FIELD_X-1  && game[gameid].fieldArea[f-1]               <=FIELD_NONE) game[gameid].borderIndex[numBorders++] = f-1;
	if(f%FIELD_X<FIELD_X      && (f+1)%FIELD_X!=0                 && game[gameid].fieldArea[f+1]               <=FIELD_NONE) game[gameid].borderIndex[numBorders++] = f+1;
	if(f>FIELD_X+1            && (f+rowAdd)%FIELD_X!=FIELD_X-1    && game[gameid].fieldArea[f-FIELD_X+rowAdd]  <=FIELD_NONE) game[gameid].borderIndex[numBorders++] = f-FIELD_X+rowAdd;
	if(f>FIELD_X              && (f+rowAdd+1)%FIELD_X!=0          && game[gameid].fieldArea[f-FIELD_X+rowAdd+1]<=FIELD_NONE) game[gameid].borderIndex[numBorders++] = f-FIELD_X+rowAdd+1;
	if(f<FIELD_SIZE-FIELD_X-1 && (f+rowAdd)%FIELD_X!=FIELD_X-1    && game[gameid].fieldArea[f+FIELD_X+rowAdd]  <=FIELD_NONE) game[gameid].borderIndex[numBorders++] = f+FIELD_X+rowAdd;
	if(f<FIELD_SIZE-FIELD_X   && (f+rowAdd+1)%FIELD_X!=0          && game[gameid].fieldArea[f+FIELD_X+rowAdd+1]<=FIELD_NONE) game[gameid].borderIndex[numBorders++] = f+FIELD_X+rowAdd+1;
	
	if(numBorders==0)
		return -1; // no free border available

	i = rand()%numBorders;
	return game[gameid].borderIndex[i];
}

int getFreeField(int gameid, int area)
{
	int numAreas=0;
	int i;
		
	for(i=0;i<FIELD_SIZE;i++)
	{
		if((area==FIELD_ANY&&game[gameid].fieldArea[i]>FIELD_NONE)||(area!=FIELD_ANY&&game[gameid].fieldArea[i]==area))
		{
			if(fieldConnected(gameid, i, FIELD_NONE))
			{
				game[gameid].fieldIndex[numAreas] = i;
				numAreas++;
			}
		}
	}
	
	if(numAreas==0)
		return -1; // no free field available

	i = rand()%numAreas;
	
	return game[gameid].fieldIndex[i];
}        

void eraseArea(int gameid, int area)
{
	for(int i=0;i<FIELD_SIZE;i++)
	{
		if(game[gameid].fieldArea[i]==area)
			game[gameid].fieldArea[i]=FIELD_UNUSED;
	}
}
/*
void generateGameField(int gameid)
{
	int curArea=0;
	int lastArea=0;
	int i,j,f;
	long sT = getTime();
	
	for(i=0;i<FIELD_SIZE;i++)
	{
		game[gameid].fieldArea[i] = FIELD_NONE;
	}

	printf("FIELD_X: %d\n", FIELD_X);
	printf("FIELD_Y: %d\n", FIELD_Y);

	j=0;
	f = FIELD_X*(FIELD_Y>>1)+(FIELD_X>>1);
//	f = rand()%FIELD_SIZE;
	game[gameid].fieldArea[f] = curArea;
	printf("[START NEW AREA] (%d): %d\n", curArea, f);
	
//	for(int i=0;i<21;i++)
	while(1)
	{
		if(curArea!=lastArea) // start a new area
		{
			f = getFreeField(FIELD_ANY);
			if(f==-1)
			{
				printf("[NEW AREA] no free fields for %d - QUIT!\n", curArea);
				break;
			}
			else
			{
				f = getFreeFieldBorder(f);
				printf("[START NEW AREA] (%d): %d\n", curArea, f);
				game[gameid].fieldArea[f] = curArea;
				j++;
			}
			lastArea=curArea;
		}
		else // continue current area
		{
			f = getFreeField(curArea);
			if(f==-1)
			{
				printf("[CONTINUE AREA] no free fields for %d - let's try to do it again...!\n", curArea);
				eraseArea(curArea);
				printf("[ERASED AREA %d]\n", curArea);
				j=0;
				lastArea--;
			}
			else
			{
				f = getFreeFieldBorder(f);
				game[gameid].fieldArea[f] = curArea;
				j++;
				if(j>=MIN_AREA_SIZE) // check if we're done drawing an area
				{
					if(j>=MAX_AREA_SIZE||rand()%3==2) // done drawing an area
					{
						curArea++;
						if(curArea>=NUM_AREAS)
							break;
						j=0;
					}
				}
			}
		}
	}
	
	long eT = getTime();
	
	FILE *fp;
	fp = fopen("field.dat", "ab");
	fwrite(fieldArea, 1, FIELD_SIZE, fp);
	fclose(fp);
	
	printf("DONE! (took %d ms)\n", (int)(eT-sT));
	fflush(stdout);
}

void loadGameField()
{
	FILE *fp;
	fp = fopen("field.dat", "rb");

	fread(fieldArea, 1, FIELD_SIZE, fp);
	fclose(fp);
}
*/
void initGameField(int gameid, int fieldNum)
{
	int i,j;
	
	// field positioning variables
	#ifdef CLIENT
	xMin=FIELD_X>>1; xMax=FIELD_X>>1;
	yMin=FIELD_Y>>1; yMax=FIELD_Y>>1;
	xAnchor = yAnchor = 0;
	#endif

	hexagonWidth = HEXAGON_WIDTH;
	hexagonHeight = HEXAGON_HEIGHT;
		
	for(i=0;i<FIELD_SIZE;i++)
	{
		game[gameid].fieldArea[i] = FIELD_DATA[i+fieldNum*FIELD_SIZE];
		#ifdef CLIENT
			if(i%FIELD_X<xMin&&game[gameid].fieldArea[i]!=AREA_NONE)
				xMin = i%FIELD_X;
			if(i%FIELD_X>xMax&&game[gameid].fieldArea[i]!=AREA_NONE)
				xMax = i%FIELD_X;
			if(i/FIELD_X<yMin&&game[gameid].fieldArea[i]!=AREA_NONE)
				yMin = i/FIELD_X;
			if(i/FIELD_X>yMax&&game[gameid].fieldArea[i]!=AREA_NONE)
				yMax = i/FIELD_X;
		#endif
	}

	#ifdef CLIENT
		// CENTRALIZE THE GAMEFIELD
		xAnchor = ((-xMin+(FIELD_X-(xMax+1)))>>1)*HEXAGON_WIDTH-4;
		yAnchor = ((-yMin+(FIELD_Y-(yMax+1)))>>1)*HEXAGON_HEIGHT+2;
	#endif

	#ifdef CLIENT
		// local game, copy old values to a temp. buffer
		for(i=0;i<NUM_AREAS;i++)
			tempAreaDice[i] = 0; // game[gameid].areaDice[i];
		tempPlayer = 0; // game[gameid].curPlayer;
	#endif
	
	for(i=0;i<NUM_AREAS;i++)
	{
		for(j=0;j<MAX_AREA_BORDERS;j++) game[gameid].borderListX[i][j] = -1;
		initAreaBorders(gameid, i);
		initAreaDice(gameid, i);
		game[gameid].areaSort[i] = AREA_NONE;
	}

	#ifdef CLIENT
		// dice piling init for the game start
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
	#endif
	
	for(i=FIELD_SIZE-1;i>=0;i--)
	{
		if(game[gameid].fieldArea[i]>FIELD_NONE)
		{
			for(j=NUM_AREAS-1;j>=0;j--)
			{
				if(game[gameid].areaSort[j]==game[gameid].fieldArea[i])
					break;
				if(game[gameid].areaSort[j]==AREA_NONE)
				{
					game[gameid].areaSort[j]=game[gameid].fieldArea[i];
					break;
				}
			}
		}
	}
}

int getCurrentArea(int gameid, int x, int y)
{
	int xAdd = 0;
	if(y%2==1) xAdd = hexagonWidth>>1;
	
	x += xAdd; //+hexagonWidth;
	x /= hexagonWidth;
	x--;
	
//	y -= hexagonHeight<<1;
	y /= hexagonHeight;
	y--;
	
	if(game[gameid].fieldArea[x+y*FIELD_X]>0)
		return game[gameid].fieldArea[x+y*FIELD_X];
	else
		return AREA_NONE;
}

// returns true if ai has no further actions to make

boolean processAI(int gameid)
{
	// 1. go through all areas, count who is strongest etc.
	// 2. process AI: 
	// - rule 1: choose strongest own area
	// - rule 2: attack to an area that has less dice than the attacker (do not attack weak players if someone else is winning)
	// - rule 3: if no weaker areas are found
	//
	// TODO: something wrong with curAreaDice check... CPU doesn't always attack when it could (which gives it a touch of niceness, though)

	if(game[gameid].gameStatus!=GAME_STATUS_PLAYING) // return immediately if the game has ended
		return true;
	
	int attackAreaE[NUM_AREAS]; // equal attack areas
	int attackAreaW[NUM_AREAS]; // weaker attack areas
	int numAttackAreaE = 0;
	int numAttackAreaW = 0;
//	int curAreaDice = 2; // sort by dice on areas, prefer starting attacks from the strongest area
//	int prevAttackAreaE = 0;
//	int prevAttackAreaW = 0;
	
	int ownAreas = game[gameid].playerAreaCount[game[gameid].curPlayer];
//	int ownDice = game[gameid].playerDiceCount[game[gameid].curPlayer];
	int totalAreas = 0; // TODO: these could be calculated / updated elsewhere...
	int totalDice = 0;
	int mostAreas = 0;
	int mostDice = 0;

	game[gameid].AIattackAll = true;
	// calc. total dice & areas
	for(int i=0;i<game[gameid].numPlayers;i++)
	{
		if(i!=game[gameid].curPlayer)
		{
			totalAreas += game[gameid].playerAreaCount[i];
			totalDice += game[gameid].playerDiceCount[i];
			if(game[gameid].playerAreaCount[i]>mostAreas) mostAreas = game[gameid].playerAreaCount[i];
			if(game[gameid].playerDiceCount[i]>mostDice) mostDice = game[gameid].playerDiceCount[i];
			
			game[gameid].AIattackPlayer[i] = false; // just reset attacking to a player false for now
			
//			if(game[gameid].playerAreaCount[i]>ownAreas||game[gameid].playerDiceCount[i]>ownDice) // we're not the strongest, there's a possibility of not attacking weak ones
//				game[gameid].AIattackAll = false;
		}
	}
	
	if(ownAreas<mostAreas) // we are not leading, see who we want to attack specifically (maybe)
	{
		for(int i=0;i<game[gameid].numPlayers;i++)
		{
			if(i!=game[gameid].curPlayer)
			{
//				if(game[gameid].playerAreaCount[i]>((totalAreas*3)/game[gameid].numPlayers)||game[gameid].playerDiceCount[i]>((totalDice*3)/game[gameid].numPlayers))
				if(game[gameid].playerAreaCount[i]>((totalAreas*5)/(game[gameid].numPlayers*2)))
				{
					game[gameid].AIattackPlayer[i] = true; // this is most likely a winning player, go ahead and attack!
					game[gameid].AIattackAll = false;
				}
			}
		}
	}
	
	#ifdef CLIENT
		playerDataUpdated = true;
	#endif
	
	for(int i=0;i<NUM_AREAS;i++)
	{
		if(game[gameid].playerArea[i]==game[gameid].curPlayer&&game[gameid].areaDice[i]>1) // =curAreaDice) // see if we can attack from this area
		{
//			curAreaDice = game[gameid].areaDice[i];
			for(int j=0;j<MAX_AREA_CONNECTIONS;j++)
			{
				if(game[gameid].areaConnections[i][j]==AREA_NONE) // done with area
				{
					break;
				}
				else if(game[gameid].playerArea[game[gameid].areaConnections[i][j]]!=game[gameid].curPlayer) // not own area
				{
					if(game[gameid].areaDice[game[gameid].areaConnections[i][j]]<game[gameid].areaDice[i]&& // attack area is weak enough!
					  (game[gameid].AIattackAll||game[gameid].AIattackPlayer[game[gameid].playerArea[game[gameid].areaConnections[i][j]]])) // ..and we're allowed to attack that area
					{
	/*					if(i!=prevAttackAreaW)
						{
							prevAttackAreaW = i;
							numAttackAreaW;
						}*/
						attackAreaW[numAttackAreaW*2] = i;
						attackAreaW[numAttackAreaW*2+1] = game[gameid].areaConnections[i][j];
						numAttackAreaW++;
					}
					else if(game[gameid].areaDice[game[gameid].areaConnections[i][j]]==game[gameid].areaDice[i]&& // attack area has equal amount of dice
					       (game[gameid].AIattackAll||game[gameid].AIattackPlayer[game[gameid].playerArea[game[gameid].areaConnections[i][j]]])) // ..and we're allowed to attack that area
					{
/*						if(i!=prevAttackAreaE)
						{
							prevAttackAreaE = i;
							numAttackAreaE;
						}*/
						attackAreaE[numAttackAreaE*2] = i;
						attackAreaE[numAttackAreaE*2+1] = game[gameid].areaConnections[i][j];
						numAttackAreaE++;						
					}
				}
			}
		}
	}

	if(numAttackAreaW>0||numAttackAreaE>0) // AI attack 
	{
		int areaIndex, areaIndex2, index;
		if(numAttackAreaW>0) // choose random weaker area
		{
			index = rand()%numAttackAreaW;
			areaIndex = attackAreaW[index*2];
			areaIndex2 = attackAreaW[index*2+1];
		}
		else // choose random equal area
		{
			index = rand()%numAttackAreaE;
			areaIndex = attackAreaE[index*2];
			areaIndex2 = attackAreaE[index*2+1];
		}
		
		gameState = GAME_STATE_INGAME_WAITING;

		logPrintf("CPU is attacking from: %d to %d!\n", areaIndex, areaIndex2);
		byte data[NETWORK_MAX_DATA];
		memset(data, 0, NETWORK_MAX_DATA);
		
		if(networkMode!=NETWORK_MODE_LOCAL)
		{
			data[0] = areaIndex; data[1] = areaIndex2;
			processAttack(gameid, game[gameid].curPlayer, data);
		}
		#ifdef CLIENT
		else
		{
			data[0] = MESSAGE_ATTACK; data[1] = areaIndex; data[2] = areaIndex2;
			sendClientData(data);
		}
		#endif
	}
	else // no attack actions were made anymore, let's end AI turn
	{	
		logPrintf("GAME: %d, END AI (%d) TURN\n", gameid, game[gameid].curPlayer);
	
		return true;
	}
	
	return false;
}
