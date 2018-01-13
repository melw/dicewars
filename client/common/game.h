
#define LOCAL 0                   // local game id
#define GAME_HISTORY_LENGTH 100

#define PLAYER1_COLOR 0xA975F4
#define PLAYER7_COLOR 0xA9F50F
#define PLAYER3_COLOR 0x008500
#define PLAYER4_COLOR 0xFF7FFE
#define PLAYER5_COLOR 0xFF7F01
#define PLAYER6_COLOR 0xB3FFFE
#define PLAYER2_COLOR 0xFFFF44
#define PLAYER8_COLOR 0xFF5858

#define PLAYER1_COLOR2 0xC995FE
#define PLAYER7_COLOR2 0xD3FF51
#define PLAYER3_COLOR2 0x20A520
#define PLAYER4_COLOR2 0xFF9FFE
#define PLAYER5_COLOR2 0xFF9F21
#define PLAYER6_COLOR2 0xE3FFFE
#define PLAYER2_COLOR2 0xFFFF91
#define PLAYER8_COLOR2 0xFF7878

#define PLAYER1_COLOR3 0x533F9E
#define PLAYER7_COLOR3 0x53AF01
#define PLAYER3_COLOR3 0x004302
#define PLAYER4_COLOR3 0x9F3FCE
#define PLAYER5_COLOR3 0x9F3F01
#define PLAYER6_COLOR3 0x539FAE
#define PLAYER2_COLOR3 0x9F9F01
#define PLAYER8_COLOR3 0x9F2828

#define PLAYER1_COLOR4 0x734FBE
#define PLAYER7_COLOR4 0x73CF01
#define PLAYER3_COLOR4 0x006302
#define PLAYER4_COLOR4 0xBF4FCE
#define PLAYER5_COLOR4 0xCF4F01
#define PLAYER6_COLOR4 0x73BFCE
#define PLAYER2_COLOR4 0xBFBF01
#define PLAYER8_COLOR4 0xBF2828

int playerColor[MAX_PLAYERS] = {
	PLAYER1_COLOR,
	PLAYER2_COLOR,
	PLAYER3_COLOR,
	PLAYER4_COLOR,
	PLAYER5_COLOR,
	PLAYER6_COLOR,
	PLAYER7_COLOR,
	PLAYER8_COLOR,
};

int playerColor2[MAX_PLAYERS] = {
	PLAYER1_COLOR2,
	PLAYER2_COLOR2,
	PLAYER3_COLOR2,
	PLAYER4_COLOR2,
	PLAYER5_COLOR2,
	PLAYER6_COLOR2,
	PLAYER7_COLOR2,
	PLAYER8_COLOR2,
};

int playerColor3[MAX_PLAYERS] = {
	PLAYER1_COLOR3,
	PLAYER2_COLOR3,
	PLAYER3_COLOR3,
	PLAYER4_COLOR3,
	PLAYER5_COLOR3,
	PLAYER6_COLOR3,
	PLAYER7_COLOR3,
	PLAYER8_COLOR3,
};

int playerColor4[MAX_PLAYERS] = {
	PLAYER1_COLOR4,
	PLAYER2_COLOR4,
	PLAYER3_COLOR4,
	PLAYER4_COLOR4,
	PLAYER5_COLOR4,
	PLAYER6_COLOR4,
	PLAYER7_COLOR4,
	PLAYER8_COLOR4,
};

char *colorName[MAX_PLAYERS] = {
	"violet",
	"green",
	"dark green",
	"lilac",
	"orange",
	"turqoise",
	"yellow",
	"red"
};

char *positionName[MAX_PLAYERS+1] = {
	" ",
	"1st",
	"2nd",
	"3rd",
	"4th",
	"5th",
	"6th",
	"7th",
	"8th"
};

int playerScoreFactor[MAX_PLAYERS] = {
	1,
	1,
	3,
	6,
	10,
	15,
	21,
	28,
};


gameData game[1];
gameStatistics gameStatistics;

char *gameHistory[GAME_HISTORY_LENGTH];
int gameHistoryCount = 0;
#ifdef DEBUG
char *debugHistory[GAME_HISTORY_LENGTH];
int debugHistoryCount = 0;
int debugDrawCount = 0;
#endif

// local variables
char playerName[MAX_PLAYERS][MAX_NAME_LENGTH] = { "Player", "Hotseat1", "Hotseat2", "Hotseat3", "Hotseat4", "Hotseat5", "Hotseat6", "Hotseat7" };
int playerId = 0;
int favouriteColor = 0;
boolean playerJoined = false;
int maxPlayers = 7;
int minPlayers = 2;
int hotseatPlayers = 1;

signed char pointerTab[X_RES*Y_RES];
int pointerX;
int pointerY;
int pointerPressX;
int pointerPressY;
boolean pointerMoved;

boolean menuUpperDrawn;
boolean playerDataUpdated;
boolean gameFieldUpdated;
boolean pointerDown;
boolean endTurnHover;
boolean endTurnChecked;
boolean surrenderHover;
boolean surrenderBox;

boolean readyToAttack;

int xPos;
int yPos;
int xAnchor;
int yAnchor;

int prevState;
long pauseTime;

int curArea;
int attackArea;
int numDice;
int playerWon;

int playerDice[2][MAX_DICE];
int playerDiceAmount[2];
int playerTotal[2];
int attackDice[2];
int diceTimer;
int tempAreaDice[NUM_AREAS];
int tempAreaCounter;
int tempPlayer;

int dicePilingCounter;
int dicePilingTotal;
int dicePilingBuffer[255];

void initGame();
void startGame(int gameid, int mode);

void initPressArea();

void paint();
void gameLogic();
void gamePaint();
void drawGameField();
int getCurrentArea(int x, int y);
void endTurn();
void surrender();
void processAttack();
void connectionStatusPaint();
void updatePlayerPositions();

boolean attackProcessed = true;
boolean endTurnPressed = false;
boolean firstTurn = true;
boolean gameStartProcessed = false;
boolean attackAreaShown = false;

int gameId;
