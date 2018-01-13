
#define MENU_MAX_LEVELS 4

#define MENU_COLOR_NORMAL 0x000000
#define MENU_COLOR_ACTIVE 0xC00000
#define MENU_COLOR_INACTIVE 0x808080
#define MENU_COLOR_ERROR 0xff0000
#define MENU_COLOR_TEXT 0x000000
#define MENU_COLOR_HIGHLIGHT 0xffffff

#define MENU_ACTION_UNDEFINED 0
#define MENU_ACTION_CHANGE_STATE 1
#define MENU_ACTION_CHANGE_MENU 2
#define MENU_ACTION_EDIT_KEYBOARD 3
#define MENU_ACTION_EDIT_SLIDER 4

#define MENU_DATA_UNDEFINED (-1)

#define MENU_EDIT_PLAYER_NAME 100
#define MENU_EDIT_FAVOURITE_COLOR 101
#define MENU_EDIT_SERVER_ADDRESS 102
#define MENU_EDIT_SERVER_PORT 103
#define MENU_EDIT_NUM_PLAYERS 104
#define MENU_EDIT_HOTSEAT_PLAYERS 105
#define MENU_EDIT_MIN_PLAYERS 106
#define MENU_EDIT_MAX_PLAYERS 107

// normal menu
#define MENU_TYPE_NORMAL 1
// text contents, no selections
#define MENU_TYPE_TEXT 2
// game menu
#define MENU_TYPE_GAME 3

#define MENU_ATTRIBUTE_ALIGN 0
#define MENU_ATTRIBUTE_X 1
#define MENU_ATTRIBUTE_Y 2
#define MENU_ATTRIBUTE_FONT 3
#define MENU_ATTRIBUTE_HEIGHT 4
#define MENU_ATTRIBUTE_FONT_HEIGHT 5

#define MENU_MAIN_INDEX 0
#define MENU_LOCAL_INDEX 1
#define MENU_NETWORK_INDEX 2
#define MENU_OPTIONS_INDEX 3
#define MENU_INSTRUCTIONS_INDEX 4
#define MENU_ABOUT_INDEX 5
#define MENU_NETWORK_NEW_GAME_INDEX 6
#define MENU_NETWORK_JOIN_GAME_INDEX 7
#define MENU_NETWORK_LOBBY_INDEX 8
#define MENU_QUIT_INDEX 9
#define MENU_PLAYERS_INDEX 10
#define MENU_NETWORK_LOCAL_INDEX 11
#define MENU_NETWORK_INTERNET_INDEX 12

#define MENU_SLIDER_WIDTH 11
#define MENU_SLIDER_HEIGHT 15

static int MENU_IMAGES[] = {
/*	MENU_LOCAL_INDEX, // which menu index has an image assigned
	IMAGE_WIFI_ICONS_INDEX, // image index
	16, // image width
	16, // image height
	0, // image x
	0, // image y*/
	MENU_NETWORK_INDEX, // which menu index has an image assigned
	IMAGE_WIFI_ICONS_INDEX, // image index
	16, // image width
	16, // image height
	16, // image x
	0, // image y
	MENU_NETWORK_LOCAL_INDEX, // which menu index has an image assigned
	IMAGE_WIFI_ICONS_INDEX, // image index
	16, // image width
	16, // image height
	16, // image x
	0, // image y
	MENU_NETWORK_INTERNET_INDEX, // which menu index has an image assigned
	IMAGE_WIFI_ICONS_INDEX, // image index
	16, // image width
	16, // image height
	32, // image x
	0, // image y
};

#define MENU_MAIN_TYPE MENU_TYPE_NORMAL
#ifdef LOBBY
	#define MENU_MAIN_LENGTH 7
#else
	#define MENU_MAIN_LENGTH 6
#endif

static char* MENU_MAIN_TEXT[] = { 
	"",
	"LOCAL GAME",
	#ifdef LOBBY
		"LOCAL WIRELESS",
	#endif
	"INTERNET GAME",
	"OPTIONS",
	"INSTRUCTIONS",
	"ABOUT"
};

static int MENU_MAIN_ACTION[] = {
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_CHANGE_MENU,
	#ifdef LOBBY
		MENU_ACTION_CHANGE_MENU,
	#endif
	MENU_ACTION_CHANGE_MENU,
	MENU_ACTION_CHANGE_MENU,
	MENU_ACTION_CHANGE_MENU,
	MENU_ACTION_CHANGE_STATE
};

// first line is either MENU_DATA_UNDEFINED, or background image index
static int MENU_MAIN_DATA[] = {
	IMAGE_MAIN_BG_INDEX,
	MENU_LOCAL_INDEX,
	#ifdef LOBBY
		MENU_NETWORK_LOCAL_INDEX,
	#endif
	MENU_NETWORK_INTERNET_INDEX,
	MENU_OPTIONS_INDEX,
	MENU_INSTRUCTIONS_INDEX,
	MENU_ABOUT_INDEX,
	GAME_STATE_EXIT
};

static int MENU_MAIN_ATTRIBUTES[] = {
	ALIGN_RT, // align
	207, // x pos / start
	#ifdef LOBBY
		-22, // y pos / start
	#else
		-25, // y pos / start
	#endif
	FONT_TRUETYPE,
	#ifdef LOBBY
		26, // line height
	#else
		30, // line height
	#endif
	FONT_TRUETYPE_HEIGHT // font height
};

#define MENU_LOCAL_TYPE MENU_TYPE_NORMAL
#define MENU_LOCAL_LENGTH 5

static char* MENU_LOCAL_TEXT[] = { 
	"START LOCAL GAME",
	"Start the game",
	"Total players: ",
	"Hotseat players: ",
	"Edit player names"
};

static int MENU_LOCAL_ACTION[] = {
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_CHANGE_STATE,
	MENU_ACTION_EDIT_SLIDER,
	MENU_ACTION_EDIT_SLIDER,
	MENU_ACTION_CHANGE_MENU
};

static int MENU_LOCAL_DATA[] = {
	MENU_DATA_UNDEFINED,
	GAME_STATE_INGAME_START,
	MENU_EDIT_NUM_PLAYERS,
	MENU_EDIT_HOTSEAT_PLAYERS,
	MENU_PLAYERS_INDEX
};

static int MENU_LOCAL_ATTRIBUTES[] = {
	ALIGN_LT, // align
	31, // x pos / start - was X_CEN
	35, // y pos / start
	FONT_TRUETYPE,
	FONT_TRUETYPE_HEIGHT+1, // line height
	FONT_TRUETYPE_HEIGHT // font height
};

#define MENU_NETWORK_TYPE MENU_TYPE_NORMAL
#define MENU_NETWORK_LENGTH 3

static char* MENU_NETWORK_TEXT[] = { 
	"NETWORK GAME",
	"LOCAL WIRELESS",
	"INTERNET GAME",
};

static int MENU_NETWORK_ACTION[] = {
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_CHANGE_MENU,
	MENU_ACTION_CHANGE_MENU
};

static int MENU_NETWORK_DATA[] = {
	IMAGE_MAIN_BG_INDEX,
	MENU_NETWORK_LOCAL_INDEX,
	MENU_NETWORK_INTERNET_INDEX,
};

static int MENU_NETWORK_ATTRIBUTES[] = {
	ALIGN_RT, // align
//	228, // x pos / start
	210, // x pos / start
	35, // -25, // y pos / start
	FONT_TRUETYPE,
	FONT_TRUETYPE_HEIGHT+8, // line height
	FONT_TRUETYPE_HEIGHT // font height
};

#define MENU_OPTIONS_TYPE MENU_TYPE_NORMAL
#define MENU_OPTIONS_LENGTH 3
//#define MENU_OPTIONS_LENGTH 5

static char* MENU_OPTIONS_TEXT[] = { 
	"OPTIONS",
	"Player name: ",
	"Favourite color: ",
//	"Host address: ",
//	"Host port: "
};

static int MENU_OPTIONS_ACTION[] = {
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_EDIT_KEYBOARD,
	MENU_ACTION_EDIT_SLIDER,
//	MENU_ACTION_EDIT_KEYBOARD,
//	MENU_ACTION_EDIT_KEYBOARD
};

static int MENU_OPTIONS_DATA[] = {
	MENU_DATA_UNDEFINED,
	MENU_EDIT_PLAYER_NAME,
	MENU_EDIT_FAVOURITE_COLOR,
//	MENU_EDIT_SERVER_ADDRESS,
//	MENU_EDIT_SERVER_PORT
};

static int MENU_OPTIONS_ATTRIBUTES[] = {
	ALIGN_LT, // align
	31, // x pos / start
	29, // y pos / start
	FONT_TRUETYPE,
	FONT_TRUETYPE_HEIGHT+1, // line height
	FONT_TRUETYPE_HEIGHT // font height
};

#define MENU_INSTRUCTIONS_TYPE MENU_TYPE_TEXT
#define MENU_INSTRUCTIONS_LENGTH 13

static char* MENU_INSTRUCTIONS_TEXT[] = { 
	" INSTRUCTIONS",
	"",
	"Each turn you may attack any opponent's",
	"territory from one of your adjacent",
	"territories as long as your territory has",
	"more than one die on it. When attacking,",
	"If you roll more than the defender, you win.",
	"",
	"At the end of each turn, the greatest",
	"number of adjoining territories you have",
	"is calculated, and you are awarded a bonus",
	"of that number of dice, which are placed",
	"randomly on your territory."
};

static int MENU_INSTRUCTIONS_ACTION[] = {
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED
};

static int MENU_INSTRUCTIONS_DATA[] = {
	IMAGE_INSTRUCTIONS_BG_INDEX,
	MENU_DATA_UNDEFINED,	
	MENU_DATA_UNDEFINED,	
	MENU_DATA_UNDEFINED,	
	MENU_DATA_UNDEFINED,	
	MENU_DATA_UNDEFINED,	
	MENU_DATA_UNDEFINED,	
	MENU_DATA_UNDEFINED,	
	MENU_DATA_UNDEFINED,	
	MENU_DATA_UNDEFINED,	
	MENU_DATA_UNDEFINED,	
	MENU_DATA_UNDEFINED,	
	MENU_DATA_UNDEFINED
};

static int MENU_INSTRUCTIONS_ATTRIBUTES[] = {
	ALIGN_LT, // align
	23, // x pos / start
	23, // y pos / start
	FONT_FIXED,
	FONT_FIXED_HEIGHT+1, // line height
	FONT_FIXED_HEIGHT // font height
};

#define MENU_ABOUT_TYPE MENU_TYPE_TEXT
#define MENU_ABOUT_LENGTH 8

static char* MENU_ABOUT_TEXT[] = { 
	"PROGRAMMING",
	"MATTI PALOSUO",
	"GRAPHICS",
	"REKO NOKKANEN",
	"SOUND DESIGN",
	"JARI PITKÄNEN",
	"ORIGINAL GAME DESIGN",
	"TARO ITO"
};

static int MENU_ABOUT_ACTION[] = {
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED
};

static int MENU_ABOUT_DATA[] = {
	IMAGE_ABOUT_BG_INDEX,
	MENU_DATA_UNDEFINED,
	MENU_DATA_UNDEFINED,
	MENU_DATA_UNDEFINED,
	MENU_DATA_UNDEFINED,
	MENU_DATA_UNDEFINED,
	MENU_DATA_UNDEFINED,
	MENU_DATA_UNDEFINED
};

static int MENU_ABOUT_ATTRIBUTES[] = {
	ALIGN_LT, // align
	31, // x pos / start
	29, // y pos / start
	FONT_TRUETYPE,
	FONT_TRUETYPE_HEIGHT+1, // line height
	FONT_TRUETYPE_HEIGHT // font height
};

#define MENU_NETWORK_NEW_GAME_TYPE MENU_TYPE_NORMAL
#define MENU_NETWORK_NEW_GAME_LENGTH 4

static char* MENU_NETWORK_NEW_GAME_TEXT[] = { 
	"START NETWORK GAME",
	"Start the game",
	"Min. Players (2-8): ",
	"Max. Players (2-8): "
};

static int MENU_NETWORK_NEW_GAME_ACTION[] = {
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_CHANGE_MENU,
	MENU_ACTION_EDIT_SLIDER,
	MENU_ACTION_EDIT_SLIDER
};

static int MENU_NETWORK_NEW_GAME_DATA[] = {
	IMAGE_ABOUT_BG_INDEX,
	MENU_NETWORK_LOBBY_INDEX,
	MENU_EDIT_MIN_PLAYERS,
	MENU_EDIT_MAX_PLAYERS
};

static int MENU_NETWORK_NEW_GAME_ATTRIBUTES[] = {
	ALIGN_LT, // align
	31, // x pos / start - was X_CEN
	35, // y pos / start
	FONT_TRUETYPE,
	FONT_TRUETYPE_HEIGHT+1, // line height
	FONT_TRUETYPE_HEIGHT // font height
};

#define MENU_NETWORK_JOIN_GAME_TYPE MENU_TYPE_GAME
#define MENU_NETWORK_JOIN_GAME_LENGTH 1

static char* MENU_NETWORK_JOIN_GAME_TEXT[] = { 
	"SELECT GAME"
};

static int MENU_NETWORK_JOIN_GAME_ACTION[] = {
	MENU_ACTION_UNDEFINED
};

static int MENU_NETWORK_JOIN_GAME_DATA[] = {
	IMAGE_ABOUT_BG_INDEX
};

static int MENU_NETWORK_JOIN_GAME_ATTRIBUTES[] = {
	ALIGN_LT, // align
	31, // x pos / start
	29, // y pos / start
	FONT_TRUETYPE,
	FONT_TRUETYPE_HEIGHT+1, // line height
	FONT_TRUETYPE_HEIGHT // font height
};

#define MENU_NETWORK_LOBBY_TYPE MENU_TYPE_GAME
#define MENU_NETWORK_LOBBY_LENGTH 1

static char* MENU_NETWORK_LOBBY_TEXT[] = { 
	"WAITING FOR PLAYERS"
};

static int MENU_NETWORK_LOBBY_ACTION[] = {
	MENU_ACTION_UNDEFINED
};

static int MENU_NETWORK_LOBBY_DATA[] = {
	IMAGE_ABOUT_BG_INDEX
};

static int MENU_NETWORK_LOBBY_ATTRIBUTES[] = {
	ALIGN_LT, // align
	31, // x pos / start
	29, // y pos / start
	FONT_TRUETYPE,
	FONT_TRUETYPE_HEIGHT+1, // line height
	FONT_TRUETYPE_HEIGHT // font height
};

#define MENU_QUIT_TYPE MENU_TYPE_NORMAL
#define MENU_QUIT_LENGTH 6

// First line is always the menu title
static char* MENU_QUIT_TEXT[] = { 
	"",
	"LOCAL GAME",
	"NETWORK GAME",
	"OPTIONS",
	"INSTRUCTIONS",
	"ABOUT"
};

static int MENU_QUIT_ACTION[] = {
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_UNDEFINED
};

// first line is either MENU_DATA_UNDEFINED, or background image index
static int MENU_QUIT_DATA[] = {
	IMAGE_MAIN_BG_INDEX,
	MENU_DATA_UNDEFINED,
	MENU_DATA_UNDEFINED,
	MENU_DATA_UNDEFINED,
	MENU_DATA_UNDEFINED,
	MENU_DATA_UNDEFINED,
	MENU_DATA_UNDEFINED
};

static int MENU_QUIT_ATTRIBUTES[] = {
	ALIGN_RT, // align
	228, // x pos / start
	-25, // y pos / start
	FONT_TRUETYPE,
	30, // line height
	FONT_TRUETYPE_HEIGHT // font height
};

#define MENU_PLAYERS_TYPE MENU_TYPE_NORMAL
#define MENU_PLAYERS_LENGTH (MAX_PLAYERS+1)

static char* MENU_PLAYERS_TEXT[] = { 
	"Name entry",
	"Player 1: ",
	"Player 2: ",
	"Player 3: ",
	"Player 4: ",
	"Player 5: ",
	"Player 6: ",
	"Player 7: ",
	"Player 8: "
};

static int MENU_PLAYERS_ACTION[] = {
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_EDIT_KEYBOARD,
	MENU_ACTION_EDIT_KEYBOARD,
	MENU_ACTION_EDIT_KEYBOARD,
	MENU_ACTION_EDIT_KEYBOARD,
	MENU_ACTION_EDIT_KEYBOARD,
	MENU_ACTION_EDIT_KEYBOARD,
	MENU_ACTION_EDIT_KEYBOARD,
	MENU_ACTION_EDIT_KEYBOARD
};

static int MENU_PLAYERS_DATA[] = {
	MENU_DATA_UNDEFINED,
	MENU_EDIT_PLAYER_NAME,
	MENU_EDIT_PLAYER_NAME,
	MENU_EDIT_PLAYER_NAME,
	MENU_EDIT_PLAYER_NAME,
	MENU_EDIT_PLAYER_NAME,
	MENU_EDIT_PLAYER_NAME,
	MENU_EDIT_PLAYER_NAME,
	MENU_EDIT_PLAYER_NAME
};

static int MENU_PLAYERS_ATTRIBUTES[] = {
	ALIGN_LT, // align
	31, // x pos / start
	25, // y pos / start
	FONT_TRUETYPE,
	FONT_TRUETYPE_HEIGHT+1, // line height
	FONT_TRUETYPE_HEIGHT // font height
};

#define MENU_NETWORK_LOCAL_TYPE MENU_TYPE_NORMAL
#define MENU_NETWORK_LOCAL_LENGTH 3

static char* MENU_NETWORK_LOCAL_TEXT[] = { 
	"START AD-HOC GAME",
	"Start a new game",
	"Join game",
};

static int MENU_NETWORK_LOCAL_ACTION[] = {
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_CHANGE_MENU,
	MENU_ACTION_CHANGE_MENU
};

static int MENU_NETWORK_LOCAL_DATA[] = {
	MENU_DATA_UNDEFINED,
	MENU_NETWORK_NEW_GAME_INDEX,
	MENU_NETWORK_JOIN_GAME_INDEX,
};

static int MENU_NETWORK_LOCAL_ATTRIBUTES[] = {
	ALIGN_HCT, // align
	X_CEN, // x pos / start
	35, // y pos / start
	FONT_TRUETYPE,
	FONT_TRUETYPE_HEIGHT+1, // menu element height
	FONT_TRUETYPE_HEIGHT // font height
};

#define MENU_NETWORK_INTERNET_TYPE MENU_TYPE_NORMAL
#define MENU_NETWORK_INTERNET_LENGTH 3

static char* MENU_NETWORK_INTERNET_TEXT[] = { 
	"START INTERNET GAME",
	"Start a new game",
	"Join game",
};

static int MENU_NETWORK_INTERNET_ACTION[] = {
	MENU_ACTION_UNDEFINED,
	MENU_ACTION_CHANGE_MENU,
	MENU_ACTION_CHANGE_MENU
};

static int MENU_NETWORK_INTERNET_DATA[] = {
	MENU_DATA_UNDEFINED,
	MENU_NETWORK_NEW_GAME_INDEX,
	MENU_NETWORK_JOIN_GAME_INDEX,
};

static int MENU_NETWORK_INTERNET_ATTRIBUTES[] = {
	ALIGN_HCT, // align
	X_CEN, // x pos / start
	35, // y pos / start
	FONT_TRUETYPE,
	FONT_TRUETYPE_HEIGHT+1, // menu element height
	FONT_TRUETYPE_HEIGHT // font height
};


#define MENU_LOBBY_INSTRUCTIONS_LENGTH 9

char *menuLobbyInstructionsTexts[] = {
	"A game will start when all the players",
	"are ready and minimum number of players",
	"(2) have joined to the game. Players with",
	"gray name are not yet ready, whereas the",
	"names in black are ready to start. The",
	"game will start automatically if maximum",
	"number of players (7) have joined.",
	"Press A or click the lower right arrow",
	"when you're ready to start the game!"
};

#define MENU_NEW_GAME_INSTRUCTIONS_LENGTH 9

char *menuNewGameInstructionsTexts[] = {
	"Please set the number of minimum",
	"and maximum players before starting the",
	"game. If less than maximum amount of",
	"players join the game, the remaining",
	"slots will be played by the CPU",
	"",
	"Select 'start game' or click the lower",
	"right arrow when you're ready to start",
	"the game!"
};

#define MENU_JOIN_GAME_INSTRUCTIONS_LENGTH 7

char *menuJoinGameInstructionsTexts[] = {
	"This screen shows all the games that",
	"are currently looking for players.",
	"If there's no open games, you can",
	"either wait for one, or create a game",
	"by yourself - other players coming",
	"online will see your game and are",
	"likely to join it.",
};

int currentMenu;
int currentMenuIndex;
int currentMenuLevel = 0;
int parentMenu[MENU_MAX_LEVELS];
int parentMenuIndex[MENU_MAX_LEVELS];
boolean menuHoverRight;
boolean menuHoverLeft;
boolean lastHoverRight;
boolean lastHoverLeft;

boolean menuUpdated = true;
boolean networkRequestSent = false;

// semi-private:
int getMenuAction(int menuIndex, int rowIndex);
int getMenuData(int menuIndex, int rowIndex);
int getMenuAttribute(int menuIndex, int rowIndex);
int getMenuImageIndex(int menuIndex, int rowIndex);

// global:
void menuLogic();
void menuKeyPressed(int key);
void menuKeyReleased(int key);
void menuPaint(int menuIndex);

boolean menuSliderDown[16];
