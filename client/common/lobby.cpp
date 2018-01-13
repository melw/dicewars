#ifdef LOBBY

void receiveUserData(unsigned char *data, int length, LPLOBBY_USER from)
{
	logPrintf("user msg %s: %d\n", LOBBY_GetUserName(from), data[0]);
//	#ifdef DEBUG
//	sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< L:UMSG (%s): %d", LOBBY_GetUserName(from), data[0]);	
//	#endif
	if(roomOwner!=NULL) // room?
	{
		if(roomOwner==pthread_self()) // pass data to server (that's us)
		{
			clientMessage(from, data[0], data+1);
		}
		else // pass data to client
		{
			serverMessage(from, data[0], data+1);
		}
	}
}

void receiveRoomData(unsigned char *data, int length, LPLOBBY_USER from)
{
	logPrintf("room msg %s: %d\n", LOBBY_GetUserName(from), data[0]);
//	#ifdef DEBUG
//	sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< L:RMSG (%s): %d", LOBBY_GetUserName(from), data[0]);	
//	#endif
	if(roomOwner!=NULL) // room?
	{
		if(roomOwner==LOBBY_GetUserByID(USERID_MYSELF)) // bypass data to server (that's us)
		{
			// uhm - there should be no broadcast messages for the server...
		}
		else // bypass data to client
		{
		  pthread_t curThread = pthread_self();
			serverMessage(curThread, data[0], data+1);
		}
	}
}

void userCallback(LPLOBBY_USER user, unsigned long reason)
{
	switch(reason)
	{
		case USERINFO_REASON_REGOGNIZE:
			logPrintf("user %s recognized\n", LOBBY_GetUserName(user));
			#ifdef DEBUG
			sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< L:USER REG (%s)", LOBBY_GetUserName(user));	
			#endif
		break;
		case USERINFO_REASON_TIMEOUT:
			logPrintf("user %s timeout\n", LOBBY_GetUserName(user));
			#ifdef DEBUG
			sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< L:USER TIMEOUT (%s)", LOBBY_GetUserName(user));	
			#endif
		break;
		case USERINFO_REASON_RETURN:
			logPrintf("user %s returned\n", LOBBY_GetUserName(user));
			#ifdef DEBUG
			sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< L:USER RET (%s)", LOBBY_GetUserName(user));	
			#endif
		break;
		case USERINFO_REASON_ROOMCREATED:
			logPrintf("user %s created a room\n", LOBBY_GetUserName(user));
//			roomOwner = user;
			#ifdef DEBUG
			sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< L:NEW ROOM (%s)", LOBBY_GetUserName(user));	
			#endif
		break;
		case USERINFO_REASON_ROOMCHANGE:
			logPrintf("user %s changed a room\n", LOBBY_GetUserName(user));
			#ifdef DEBUG
			sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "<<< L:CHG ROOM (%s)", LOBBY_GetUserName(user));	
			#endif
		break;
	}
}

void lobbyPreInit()
{
	IPC_Init();
	IPC_SetChannelCallback(0,&LWIFI_IPC_Callback);
}

void lobbyInit()
{
	#ifdef DEBUG
	sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "!!! L:INIT");	
	#endif
	LOBBY_Init() ;
	LOBBY_SetStreamHandler(0x0001,&receiveUserData);
	LOBBY_SetStreamHandler(0x0002,&receiveRoomData);
	LOBBY_SetUserInfoCallback(&userCallback);
}
#endif
