#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define X_RES 256
#define Y_RES 192
#define X_CEN 128
#define Y_CEN 96

#define ALIGN_L 0x100
#define ALIGN_T 0x200
#define ALIGN_HC 0x10
#define ALIGN_VC 0x20
#define ALIGN_R 0x40
#define ALIGN_B 0x80
#define ALIGN_HCT    (ALIGN_HC | ALIGN_T)
#define ALIGN_HCVC   (ALIGN_HC | ALIGN_VC)
#define ALIGN_HCB    (ALIGN_HC | ALIGN_B)
#define ALIGN_RB     (ALIGN_R | ALIGN_B)
#define ALIGN_RVC    (ALIGN_R | ALIGN_VC)
#define ALIGN_RT     (ALIGN_R | ALIGN_T)
#define ALIGN_LB     (ALIGN_L | ALIGN_B)
#define ALIGN_LVC    (ALIGN_L | ALIGN_VC)
#define ALIGN_LT     (ALIGN_L | ALIGN_T)

#define boolean int
#define true 1
#define false 0
#define SOCKET int
#define byte unsigned char
#ifdef WIN32
	#define pthread_self() GetCurrentThreadId()
	#define pthread_t DWORD
	#define pthread_exit(x) TerminateThread(GetCurrentThread(), 0)
	#define ioctl(x,y,z) ioctlsocket(x,y,z)
#elif defined NDS
	#ifdef LOBBY
		#define pthread_self() LOBBY_GetUserByID(USERID_MYSELF)
		#define pthread_t LPLOBBY_USER
	#else
		#define pthread_self() 0 // TODO
		#define pthread_t int // TODO
	#endif
	#define pthread_exit(x) void(0) // TODO
#else
	#define closesocket(x) close(x)
#endif

#define KEY_POINTER 1
// SDL
#ifndef NDS
	#define KEY_UP 273
	#define KEY_DOWN 274
	#define KEY_RIGHT 275
	#define KEY_LEFT 276
#endif

#define KEY_SELECT 13
#define KEY_ESCAPE 27
#define KEY_BACK 8

unsigned long getTime();
boolean eventCallback();
void setActiveScreen(int display);

// SDL - two screens

#ifdef OSX
	// flip screens
	#define SCREEN_UPPER 0
	#define SCREEN_LOWER 1
	#define MSG_NOSIGNAL SO_NOSIGPIPE
#else
	#define SCREEN_UPPER 1
	#define SCREEN_LOWER 0
#endif

int activeScreen;
#if defined WIN32 | defined LINUX | defined OSX
	#define SCREEN unsigned long
#else
	#define SCREEN uint16
#endif

SCREEN *screenBuffer[2];

#define GAME_STATE_TEST 0 // test-case
#define GAME_STATE_LOADING 1
#define GAME_STATE_MENU 2
#define GAME_STATE_INGAME_START 3
#define GAME_STATE_INGAME 4
#define GAME_STATE_INGAME_WAITING 5
#define GAME_STATE_INGAME_END 6
#define GAME_STATE_INGAME_POST_STATUS 7
#define GAME_STATE_INGAME_WAITING_TURN 8
#define GAME_STATE_INGAME_MENU 9
#define GAME_STATE_INGAME_POST_ATTACK 10
#define GAME_STATE_EXIT 11
#define GAME_STATE_KEYBOARD 12

#define GAME_STATUS_NONEXISTING 0
#define GAME_STATUS_WAITING_PLAYERS 1
#define GAME_STATUS_PLAYING 2
#define GAME_STATUS_ENDED 3

#define PLAYER_TYPE_LOCAL 0
#define PLAYER_TYPE_NETWORK 1
#define PLAYER_TYPE_CPU 2

#define PLAYER_STATUS_UNCONFIRMED 0
#define PLAYER_STATUS_READY 1   // in-game status
#define PLAYER_STATUS_WAITING 2 // in-game status
#define PLAYER_STATUS_DEAD 3    // in-game status
#define PLAYER_STATUS_QUIT 4
#define PLAYER_STATUS_DROPPED 5

#endif // COMMON_H_INCLUDED
