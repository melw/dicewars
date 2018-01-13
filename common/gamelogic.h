//#ifndef GAME_H_INCLUDED
//#define GAME_H_INCLUDED

#define MAX_PLAYERS 8
#define MAX_NAME_LENGTH 16

#define PLAYER_ATTACKER 0
#define PLAYER_DEFENDER 1

#define HEXAGON_WIDTH 16
#define HEXAGON_HEIGHT 9
#define FIELD_X 15 //((X_RES-HEXAGON_WIDTH)/HEXAGON_WIDTH)
#define FIELD_Y 20 //((Y_RES-HEXAGON_HEIGHT)/HEXAGON_HEIGHT)
#define FIELD_SIZE (FIELD_X*FIELD_Y)
#define FIELD_ANY 99
#define FIELD_NONE (-1)
#define FIELD_UNUSED (-2)
#define AREA_NONE (-1)

#define MIN_AREA_SIZE 4
#define MAX_AREA_SIZE 10
#define MAX_AREA_BORDERS (6*MAX_AREA_SIZE)
#define MAX_AREA_CONNECTIONS 10
#define NUM_AREAS 30
#define NUM_FIELDS 100

#define MAX_DICE 8
#define MAX_DICE_RESERVE 56 // two rows of dice on the screen
#define MAX_DICE_TOTAL 90

struct gameData
{
  int gameStatus;
  char playerName[MAX_PLAYERS][MAX_NAME_LENGTH];
	int playerColor[MAX_PLAYERS];
  int playerScore[MAX_PLAYERS];
  int gameScore[MAX_PLAYERS];
  int gameOwner;
  int playerPosition[MAX_PLAYERS];
  int playerAreaCount[MAX_PLAYERS];
  int playerDiceCount[MAX_PLAYERS];
  int playerAreaIndex[MAX_PLAYERS][NUM_AREAS];
  int playerStatus[MAX_PLAYERS];
  int playerType[MAX_PLAYERS];
  byte playerMACAddress[MAX_PLAYERS][6];
  boolean playerSurrended[MAX_PLAYERS];
  int playerLastActiveTurn[MAX_PLAYERS];
  int gameField;
  int numPlayers;
  int minPlayers;
  int maxPlayers;
  int curPlayer;
  int curTurn;
  int position;  
  int pot;
  
  // TODO: partially for precalc only, too lazy to split outside the struct right now...
  int playerArea[NUM_AREAS];
  int diceIndex[NUM_AREAS];
  signed char fieldArea[FIELD_SIZE];
  signed char pointerTab[X_RES*Y_RES];
  boolean areaDiceWide[NUM_AREAS];
  int areaDice[NUM_AREAS];
  int areaConnections[NUM_AREAS][MAX_AREA_CONNECTIONS];
  boolean areaVisited[NUM_AREAS];
  int areaConnectionCount;
	int areaSort[NUM_AREAS];
	int fieldIndex[MAX_AREA_SIZE];
	int borderIndex[6];
	int borderListX[NUM_AREAS][MAX_AREA_BORDERS];
	int borderListY[NUM_AREAS][MAX_AREA_BORDERS];
	int diceReserve[MAX_PLAYERS];

	int curAngle;
	int curIndex;

  long gameTime;
  long startTime;
  long stateTimer;

	boolean AIattackAll;
	boolean AIattackPlayer[MAX_PLAYERS];
};

int gameState;

int hexagonWidth;
int hexagonHeight;

void initGameField(int gameid, int fieldNum);
int getCurrentArea(int x, int y);
boolean processAI(int gameid);

#ifdef CLIENT
int xMax, xMin, yMax, yMin;
#endif

int scoreTab[MAX_PLAYERS-1][MAX_PLAYERS] = 
{
	{
		// 2 players
		0, 
		10000, 
	},{
		// 3 players
		0,
		3334,
		10000,
	},{
		// 4 players
		0,
		1667,
		4000,
		10000,
	},{
		// 5 players
		0,
		1000,
		2223,
		4286,
		10000,
	},{
		// 6 players
		0,
		667,
		1429,
		2500,
		4445,
		10000,
	},{
		// 7 players
		0,
		477,
		1000,
		1667,
		2667,
		4546,
		10000,
	},{
		// 8 players
		0,
		358,
		741,
		1200,
		1819,
		2778,
		4616,
		10000,
	}
};


//#endif // GAME_H_INCLUDED
