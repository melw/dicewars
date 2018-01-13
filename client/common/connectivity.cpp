// TODO: lobby & network co-operation

boolean initNetworkConnection()
{
	#ifdef LOBBY
		// test
		networkInitTimer = getTime();
		networkInitCounter = 7;
		networkInitialized = true;
		networkConnected = true;
		return true;
	#endif
	#ifdef DEBUG
	if(networkInitCounter==0)
	{
		sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> INIT");
	}
	#endif
  #ifdef WIN32
	  WORD wVersionRequested = MAKEWORD( 1, 1 ); // requires at least winsock 1.1
  	WSADATA wsaData; 
  #endif
	
	if(!networkInitialized)
	{
		networkStatus = NETWORK_STATUS_CONNECTING;
		#ifdef WIN32
	  if ( WSAStartup( wVersionRequested, &wsaData ) != 0 )
	  {
			logPrintf("!!! Could not startup the network!\n");
	  	return false;
	  }
	  else
	  {
	  	networkInitCounter = 5;
	  	networkInitialized = true;
	  }
	  #elif defined NDS
	  if(wifiInitialized&&networkInitCounter<2) // never initialize wifi more than once
	  	networkInitCounter = 2;
	  switch(networkInitCounter)
	  {
	  	case 0:
	  	{
				u32 Wifi_pass = Wifi_Init(WIFIINIT_OPTION_USELED);
				
//		    REG_IPC_FIFO_TX=0x12345678;
//		    REG_IPC_FIFO_TX = Wifi_pass;

				pFIFO_message transfer2 = (pFIFO_message)( ((u32)&transfer[0]) | 0x00400000); 
				
				transfer2[fifo_lastmsg].command = MESSAGE_WIFI_INIT;
				SendArm7Command((u32)&transfer[fifo_lastmsg]);
				fifo_lastmsg=(fifo_lastmsg+1)&15;

				REG_IPC_FIFO_TX = Wifi_pass;
				/*transfer2 = (pFIFO_message)( ((u32)&transfer[0]) | 0x00400000); 
				
				transfer2[fifo_lastmsg].command = Wifi_pass;
				SendArm7Command((u32)&transfer[fifo_lastmsg]);
				fifo_lastmsg=(fifo_lastmsg+1)&15; 	*/
			   	
				*((volatile u16 *)0x0400010E) = 0; // disable timer3
				
				irqSet(IRQ_TIMER3, Timer_50ms); // setup timer IRQ
				irqEnable(IRQ_TIMER3);
			   	
		   	Wifi_SetSyncHandler(arm9_synctoarm7); // tell wifi lib to use our handler to notify arm7
			
				*((volatile u16 *)0x0400010C) = (u16)-6553; // 6553.1 * 256 cycles = ~50ms;
				*((volatile u16 *)0x0400010E) = 0x00C2; // enable, irq, 1/256 clock
				networkInitCounter = 1;
				networkInitTimer = getTime();
				break;
			}
	
			case 1:
			{
				if(Wifi_CheckInit()!=0)
				{
					networkInitCounter = 2;
					wifiInitialized = true;
				}
				else
				{
					if((signed long)(networkInitTimer-getTime())>1000) // wait max. 1 second for arm7 to respond
					{
						#ifdef DEBUG
						sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! ARM7 FAIL %d", (int)(networkInitTimer-getTime()));
						playerDataUpdated = true;
						networkInitCounter = 0;
						#endif
						return false;
					}
				}
				break;
			}
	
			case 2:
			{
				networkInitTimer = getTime();
				Wifi_EnableWifi();
				Wifi_AutoConnect();
				networkInitCounter = 3;
				break;
			}
	
			case 3:
			{
			  int stat = Wifi_AssocStatus();
			  if(stat == ASSOCSTATUS_ASSOCIATED) // ok!
			  {
			  	networkInitCounter = 5;
			  	networkInitialized = true;
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! DSWIFI OK");
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! %s", serverAddress);
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! PORT %d", serverPort);
					playerDataUpdated = true;
					#endif
			  }
			  else if(stat == ASSOCSTATUS_CANNOTCONNECT)
			  {
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! STAT %d", stat);
					playerDataUpdated = true;
					#endif
			    return false;
			  }
				else if((signed long)(networkInitTimer-getTime())>6000) // wait max. 6 seconds before failing
				{
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! TIMEOUT %d", (int)(networkInitTimer-getTime()));
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! STAT %d", stat);
					playerDataUpdated = true;
					#endif
					logPrintf("Failed with stat: %d\n", stat);
					
					networkInitCounter = 4;
					return false;
				}
				break;
			}
		}
		#else
			// OSX & Linux
	  	networkInitCounter = 5;
	  	networkInitialized = true;		
	  #endif
	}
	else if(!networkConnected)
	{
		switch(networkInitCounter)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			{
				networkStatus = NETWORK_STATUS_CONNECTING;
				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> CONNECT");
				#endif			
				logPrintf("Connecting to server: %s:%d\n", serverAddress, serverPort);
			  if ( (client_sock = socket(AF_INET, SOCK_STREAM, 0))<0)
			  {
			    closesocket(client_sock); 
					logPrintf("!!! Createsock error: %s\n", strerror(errno));
					#ifdef DEBUG
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! SOCK FAIL");
					sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], strerror(errno));
					#endif			
			    return false;
			  }
			  else
			  {
			  	logPrintf("Socket create ok.\n");
				  server_address.sin_family=AF_INET;
				  server_address.sin_port = htons(serverPort);
				  server_address.sin_addr.s_addr=inet_addr(serverAddress);

				  if ( (connect(client_sock,(struct sockaddr *) &server_address, sizeof(server_address))) != 0)
				  {
						#ifdef DEBUG
						sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! CONN FAIL");
						#endif			
				    logPrintf("!!! Could not connect to %s:%d\n", serverAddress, serverPort);
				    logPrintf("Error (%d): %s", errno, strerror(errno));
				    closesocket(client_sock); 
				    return false;
				  }
				  else
				  {
				  	logPrintf("Socket connect ok\n");
						ioctl(client_sock,FIONBIO,&nbMode); // set non-blocking			
					}

			  	networkInitCounter = 6;
			  }
			  break;
			}

			case 6:
			{
				// TODO: Linux&OSX MAC 
				#ifdef NDS
					Wifi_GetData(WIFIGETDATA_MACADDRESS,6,(unsigned char *)MACAddress);
				#elif defined WIN32
					IP_ADAPTER_INFO AdapterInfo[16];
				  DWORD dwBufLen = sizeof(AdapterInfo);
				  DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
				  assert(dwStatus == ERROR_SUCCESS);
				  PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
					byte* temp = AdapterInfo->Address;
					for(int i=0;i<6;i++)
						MACAddress[i] = temp[i];
				#endif

				#ifdef DEBUG
				sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], ">>> VER CHECK");
				#endif
				logPrintf("Send version check query\n");
		    memset(sendBuffer, 0, NETWORK_MAX_DATA);
		    sendBuffer[0] = MESSAGE_INIT_CONNECTION;
		    sendBuffer[1] = versionMajor;
		    sendBuffer[2] = versionMinor;
		    sendBuffer[3] = versionRevision;
		    for(int i=0;i<6;i++)
		    	sendBuffer[4+i] = MACAddress[i];
		    for(int i=0;i<MAX_NAME_LENGTH;i++)
		    	sendBuffer[10+i] = playerName[LOCAL][i];
				sendClientData(sendBuffer);

			  // everything's fine, let's start
			  logPrintf("Connect succeeded to %s:%d! Waiting for version check...\n", serverAddress, serverPort);
		  	networkInitCounter = 7;
		  	networkInitTimer = getTime();
			  break;
			}
			
			case 7:
			{
				if(networkStatus==NETWORK_STATUS_CONNECTING&&(signed long)(networkInitTimer-getTime())>5000) // retry connection in 5 seconds
				{
					logPrintf("!!! RETRY TIMEOUT 5 SECONDS\n");
					resetNetworkConnection(); // retry connection
				}
			}
		}
	}
	  
  return true;
}

void resetNetworkConnection()
{
	#ifdef LOBBY
		#ifdef DEBUG
		sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "!!! N:RESET");
		#endif
		networkConnected = false;
		return;
	#endif
	logPrintf("!!! Reset network connection.\n");
	ioctl(client_sock,FIONBIO,&bMode);
	if(shutdown(client_sock, 2)!=0)
	{
		logPrintf("!!! Closesock error: %s\n", strerror(errno));
		#ifdef DEBUG
		sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! CLOSESOCK FAIL");
		sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], strerror(errno));
		#endif			
	}
	else
	{
		logPrintf("!!! Closesock ok\n");
		#ifdef DEBUG
		sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "! CLOSESOCK OK");
		#endif
	}
	#ifdef NDS
		for(int i=0;i<6;i++)
			swiWaitForVBlank();
	#endif
	if(closesocket(client_sock)!=0)
	{
		logPrintf("!!! Closesock error: %s\n", strerror(errno));
	}
	networkConnected = false;
	networkInitCounter = 5;

	// assure that no old ping's are timeouted
	pongCounter=pingCounter;
	pingTimer = getTime();
}

void uninitNetworkConnection()
{
	#ifdef LOBBY
		#ifdef DEBUG
		sprintf(debugHistory[(debugHistoryCount++)%GAME_HISTORY_LENGTH], "!!! N:UNINIT");
		#endif
	#endif
	if(networkConnected||networkInitCounter>5)
	{
		resetNetworkConnection();
	}
	#if defined NDS & !defined LOBBY
		if(networkInitialized||networkInitCounter>0)
		{
			Wifi_DisconnectAP();
			Wifi_DisableWifi();
		}
	#endif
	
	networkInitialized = false;
	if(networkInitCounter>2)
		networkInitCounter = 2;
}

