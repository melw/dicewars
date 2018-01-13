#define NETWORK_STATUS_UNDEFINED 0
#define NETWORK_STATUS_CONNECTED 1
#define NETWORK_STATUS_CONNECTING 2
#define NETWORK_STATUS_CONNECTION_LOST 3
#define NETWORK_STATUS_CONNECTION_FAILED 4
#define NETWORK_STATUS_WRONG_VERSION 5

fd_set client_set;

int numOpenGames = -1;
char openGames[NETWORK_MAX_GAMES][MAX_NAME_LENGTH];
int openGameIds[NETWORK_MAX_GAMES];
int openGameStatus[NETWORK_MAX_GAMES];
int openGamePlayers[NETWORK_MAX_GAMES];
int openGameMinPlayers[NETWORK_MAX_GAMES];
int openGameMaxPlayers[NETWORK_MAX_GAMES];
int openGameFieldIds[NETWORK_MAX_GAMES];
char openGameNames[NETWORK_MAX_GAMES][MAX_NAME_LENGTH+8];

byte sendBuffer[NETWORK_MAX_DATA];
byte receiveBuffer[NETWORK_MAX_DATA+1];
unsigned long pingTimer;
int pingCounter;
int pongCounter;

// messages leaving from client to server
void sendClientData(byte *sendBuffer);

// receives data from server
void receiveServerData();

// message buffer flushing for client, receiving server message
void flushReceiveBuffer();

void clientJoinGame(int gameId);
void clientQuitGame();
void clientReadyToStart();
void clientListOpenGames();

boolean readyToStart = false;
boolean networkInitConnectionSent = false;
boolean reconnectNetwork = false;
