// out of hat
#define LOBBY_GAMECODE 70

#ifdef LOBBY
	void receiveUserData(unsigned char *data, int length, LPLOBBY_USER from);
	void receiveRoomData(unsigned char *data, int length, LPLOBBY_USER from);
	void userCallback(LPLOBBY_USER user, unsigned long reason);
	void lobbyPreInit();
	void lobbyInit();

	LPLOBBY_ROOM openGameRooms[NETWORK_MAX_GAMES];
	LPLOBBY_USER openGameOwners[NETWORK_MAX_GAMES];
#else
	#define lobbyPreInit()
	#define lobbyInit()
#endif
