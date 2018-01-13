byte MACAddress[6];

int networkInitCounter = 0;
unsigned long networkInitTimer;
int networkStatus = NETWORK_STATUS_UNDEFINED;
boolean networkInitialized = false; // is dswifi initialized?
boolean networkConnected = false; // are we connected to the server?
boolean wifiInitialized = false; // only ever call Wifi_Init() once

SOCKET client_sock;
struct sockaddr_in server_address;

boolean initNetworkConnection(); // init wifi & create socket connection
void resetNetworkConnection(); // only reset & close socket connection
void uninitNetworkConnection(); // uninit whole wifi
