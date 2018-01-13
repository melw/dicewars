//#ifndef NETWORK_H_INCLUDED
//#define NETWORK_H_INCLUDED

// global network defines
#define NETWORK_MODE_LOCAL 0             // use local loopback, single-player / hotseat
#define NETWORK_MODE_LOBBY 1             // use local wireless network, multi-player
#define NETWORK_MODE_TCP 2               // use internet, multi-player
#define NETWORK_MAX_DATA 255             // data from / to server should fit always in 255 bytes
#define NETWORK_MAX_PLAYERS 8            // max players per game
#define NETWORK_MAX_LISTED_GAMES 6       // how many open games do we list at max.
#define NETWORK_MAX_THREADS 2048         // sufficient for 256 games with max (8 players)... need to recycle these! (TODO)
#define NETWORK_BUFFER_LENGTH 255        // how many server -> client messages we're buffering
#ifdef CLIENT
	#define NETWORK_PING_INTERVAL 10000    // ping interval in milliseconds
	#define NETWORK_PING_TIMEOUT 4000      // timeout for ping in milliseconds - should be smaller than NETWORK_PING_INTERVAL
	#define NETWORK_MAX_GAMES 1
#else
	#define NETWORK_PING_INTERVAL 20000    // ping interval in milliseconds
	#define NETWORK_PING_TIMEOUT 7500      // timeout for ping in milliseconds - should be smaller than NETWORK_PING_INTERVAL
	#define NETWORK_MAX_GAMES 255          // need to redefine this if the server gets crowded
#endif

// defaults - these can be overwritten in dicewars.ini file

// cs.helsinki.fi
// char *serverAddress = "128.214.9.104";

// lan
// char *serverAddress = "192.168.0.102";

// dovoto.com
char *serverAddress = "64.22.79.180";

int serverPort = 23456;

// MESSAGES FROM CLIENT TO SERVER
#define MESSAGE_CREATE_GAME 1            // data[0] = min players (1 byte, 2-MAX_PLAYERS)
                                         // data[1] = max players (1 byte, 2-MAX_PLAYERS)
                                         // data[2] = field id (1 byte, 0-MAX_FIELDS)
                                         // data[3] = favourite color (1 byte, 0-7)
                                         // data[4] = player name (MAX_NAME_LENGTH bytes)
#define MESSAGE_LIST_OPEN_GAMES 2        // no data
#define MESSAGE_JOIN_GAME 3              // data[0] = game id (1 byte, 0-NETWORK_MAX_GAMES)
                                         // data[1] = favourite color (1 byte, 0-7)
                                         // data[2] = player name (MAX_NAME_LENGTH bytes)
#define MESSAGE_READY_TO_START 4         // no data
#define MESSAGE_QUIT_GAME 5              // no data
#define MESSAGE_ATTACK 6                 // data[0] = attack-from area (1 byte, 0-NUM_AREAS)
                                         // data[1] = attack-to area (1 byte, 0-NUM_AREAS)
#define MESSAGE_END_TURN 7               // no data
#define MESSAGE_INIT_CONNECTION 8        // data[0] = version major (1 byte)
                                         // data[1] = version minor (1 byte)
                                         // data[2] = version revision (1 byte)
                                         // data[3] = MAC address (6 bytes)
                                         // data[9] = player name (MAX_NAME_LENGTH bytes)
#define MESSAGE_SURRENDER 9              // no data
#define MESSAGE_SERVER_STATUS 22         // no data

// MESSAGES FROM SERVER TO CLIENT
//#define MESSAGE_INIT_CONNECTION        // no data - connection is ok
//#define MESSAGE_LIST_OPEN_GAMES        // data[0] = number of open games (1 byte, 0-NETWORK_MAX_GAMES)
                                         // data[1] = gameid (1 byte, 0-NETWORK_MAX_GAMES)
                                         // data[2] = game status (1 byte)
                                         // data[3] = amount of players entered already (1 byte, 1-MAX_PLAYERS)
                                         // data[4] = min players (1 byte, 2-MAX_PLAYERS)
                                         // data[5] = max players (1 byte, 2-MAX_PLAYERS)
                                         // data[6] = field id (1 byte, 0-MAX_FIELDS)
                                         // data[7] = game owner color (1 byte, 0-7)
                                         // data[8] = game owner name (MAX_NAME_LENGTH bytes)
                                         // repeat data[1...8+MAX_NAME_LENGTH] for data[0] times
//#define MESSAGE_READY_TO_START         // data[0] = player id being ready
#define MESSAGE_START_GAME 10            // data[0] = field id (1 byte, 0-NUM_FIELDS)
                                         // data[1] = number of players (1 byte, 2-MAX_PLAYERS)
                                         // data[2] = player types (MAX_PLAYERS bytes)
                                         // data[2+MAX_PLAYERS] = player names (MAX_NAME_LENGTH * data[1] bytes)
#define MESSAGE_FULL_STATUS 11           // data[0] = turn number (1 byte)
                                         // data[1] = player id taking turn (1 byte, 0...MAX_PLAYERS)
                                         // data[2] = current pot (2 bytes, short)
                                         // data[4] = player positions (MAX_PLAYERS bytes)
                                         // data[4+MAX_PLAYERS] = player scores (MAX_PLAYERS bytes)
                                         // data[4+MAX_PLAYERS*2] = player statuses (MAX_PLAYERS bytes)
                                         // data[4+MAX_PLAYERS*3] = player surrended? (MAX_PLAYERS bytes)
                                         // data[4+MAX_PLAYERS*4] = player indexes on areas (NUM_AREAS bytes)
                                         // data[4+MAX_PLAYERS*4+NUM_AREAS] = dice on areas (NUM_AREAS bytes)
                                         // data[4+MAX_PLAYERS*4+NUM_AREAS*2] = dice in reserve (MAX_PLAYERS bytes)
#define MESSAGE_PLAYER_STATUS 12         // data[0] = own player id (1 byte, 0...MAX_PLAYERS)
                                         // data[1] = amount of players (1 byte, 1-MAX_PLAYERS)
                                         // data[2] = player id (1 byte, 0...MAX_PLAYERS)
                                         // data[3] = player status (1 byte)
                                         // data[4] = player surrended (1 byte)
                                         // data[5] = player color (1 byte, 0-7)
                                         // data[6] = player name (MAX_NAME_LENGTH bytes)
                                         // repeat data[2...6+MAX_NAME_LENGTH] for data[1] times
//#define MESSAGE_ATTACK                 // data[0] = attack-from area (1 byte)
                                         // data[1] = attack-to area (1 byte)
                                         // data[2] = dice on attack-from area (1 byte, 1-8)
                                         // data[3] = dice on attack-to area (1 byte, 1-8)
                                         // data[4] = who won? (1 byte, 0 = attacker, 1 = defender)
                                         // data[5] = dice total for the attacker (1 byte)
                                         // data[6] = dice total for the defender (1 byte)
                                         // data[7] = dice numbers for attacker (8 bytes, padded to 0 if less than 8 dice)
                                         // data[15] = dice numbers for defender (8 bytes, padded to 0 if less than 8 dice)
#define MESSAGE_PLAYER_JOINED 13         // data[0] = gameid (1 byte)
                                         // data[1] = playerid (1 byte)
                                         // data[2] = player color (1 byte, 0-7)
                                         // data[3] = player name (MAX_NAME_LENGTH bytes)
#define MESSAGE_PLAYER_QUIT 14           // data[0] = who quit the game?
#define MESSAGE_PLAYER_DROPPED 15        // data[0] = who dropped from the game?
#define MESSAGE_PLAYER_SURRENDED 16      // data[0] = who surrended?
                                         // data[1] = player position
                                         // data[2] = player score
#define MESSAGE_END_GAME 17              // data[0] = current turn
                                         // data[1] = player id, who won (1 byte)
                                         // data[2] = winning player score
// DON'T CHANGE THIS: (old clients need to know if they're old :)
#define MESSAGE_ERROR 18                 // data[0] = error id (1 byte)
                                         // data[1] = associated error data (0...n bytes, depending on the error type)
#define MESSAGE_PLAYER_DIED 19           // data[0] = who died?
                                         // data[1] = player position
                                         // data[2] = player score
                                         // data[3] = current pot (2 bytes, short)
//#define MESSAGE_SERVER_STATUS 22       // data[0] = server uptime - minutes (2 bytes, short)
                                         // data[2] = games played (2 bytes, short)
                                         // data[4] = open games (1 byte)
                                         // data[5] = active games (1 byte)
                                         // data[6] = last player connected - minutes ago (2 bytes, short)
                                         // data[8] = last game started - minutes ago (2 bytes, short)
                                         // data[10] = last game finished - minutes ago (2 bytes, short)
                                         // data[12] = players online (2 bytes, short)

// FROM BOTH CLIENT AND SERVER
#define MESSAGE_PING 20                  // data[0] = ping id (1 byte, running number from 0 to 255 per game)
                                         // if game[gameid].gameStatus = GAME_STATUS_WAITING_PLAYERS, include running statistics:
                                         // data[1] = games played (2 bytes, short)
                                         // data[3] = open games (1 byte)
                                         // data[4] = active games (1 byte)
                                         // data[5] = last player connected - minutes ago (2 bytes, short)
                                         // data[7] = last game started - minutes ago (2 bytes, short)
                                         // data[9] = last game finished - minutes ago (2 bytes, short)
                                         // data[11] = players online (2 bytes, short)

#define MESSAGE_PONG 21                  // data[0] = ping id (1 byte, copied from ping message)

// ERROR CODES
#define ERROR_CREATE_GAME_FAILED 0       // client tried to create a game but failed - this shouldn't happen though unless MAX_GAMES limit is reached
#define ERROR_JOIN_GAME_FAILED 1         // client tried to join to a game but failed because:
                                         // data[1] = 0, game does not exist (GAME_STATUS_NONEXISTING)
                                         // data[1] = 1, game has already started (GAME_STATUS_PLAYING / GAME_STATUS_ENDED)
                                         // data[1] = 2, game has already max number of players
                                         // data[1] = 3, client has already joined this game!
#define ERROR_INIT_CONNECTION_REQUIRED 2 // no data
#define ERROR_INIT_CONNECTION_FAILED 3   // data[1] = required version major (1 byte)
                                         // data[2] = required version minor (1 byte)
                                         // data[3] = required version revision (1 byte)

pthread_t threadIds[NETWORK_MAX_THREADS];
int threadGameIndex[NETWORK_MAX_THREADS];
boolean threadGameAI[NETWORK_MAX_GAMES];
int threadPlayerIndex[NETWORK_MAX_THREADS];
char threadPlayerName[NETWORK_MAX_THREADS][MAX_NAME_LENGTH];
byte threadSendBuffer[NETWORK_BUFFER_LENGTH][NETWORK_MAX_DATA];
pthread_t threadBufferIds[NETWORK_BUFFER_LENGTH];
unsigned long threadPingTimer[NETWORK_MAX_THREADS];
int threadPingCounter[NETWORK_MAX_THREADS];
int threadPongCounter[NETWORK_MAX_THREADS];
boolean threadConnected[NETWORK_MAX_THREADS];
byte threadMACAddress[NETWORK_MAX_THREADS][6];
long threadGameSleep[NETWORK_MAX_GAMES];
unsigned long threadRequestGameTimer[NETWORK_MAX_GAMES];
boolean threadRequestGameEnd[NETWORK_MAX_GAMES];
unsigned long threadGameActivityTimer[NETWORK_MAX_GAMES];

pthread_t roomOwner;

#ifdef WIN32
  u_long nbMode = 1;
  u_long bMode = 0;
#else
	int nbMode = 1;
	int bMode = 0;
#endif

fd_set server_set;
int curMessage;
int networkMode = NETWORK_MODE_LOCAL;

// message coming from a client to the server
void clientMessage(pthread_t curThread, byte message, byte *data);
void receiveClientData(SOCKET sockfd);

// message leaving from the server to a client
void serverMessage(pthread_t threadId, byte message, byte *data);
void sendServerData(SOCKET sockfd, byte *sendBuffer);

byte receiveMessage();

// DEBUG - for now
void waitForMessage(byte message);

#if defined LOBBY || !defined CLIENT
void rejoinPlayer(pthread_t threadId, byte *data, int gameid, int playerid);
void startNetworkGame(byte *data, int gameid);
#endif
void endTurnLogic(int gameid);
void resetPlayerAreas(int gameid);
void processAttack(int gameid, int playerid, byte *data);
boolean findNextPlayer(int gameid);

//#endif // NETWORK_H_INCLUDED

