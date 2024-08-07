#ifndef mainmenuh
#define mainmenuh

//includes
#include "scenes.h"
#include "audio_engine_external.h"

//constants
#define STARRY_IMAGE_CHARDATA 0
#define STARRY_IMAGE_TILEMAP 28
#define STARRY_IMAGE_PAL_START 0
#define STARRY_IMAGE_PAL_SIZE 1
#define STARRY_IMAGE_GFX_START 0
#define STARRY_IMAGE_GFX_SIZE 73

#define TITLE_CARD_CHARDATA 1
#define TITLE_CARD_TILEMAP 27
#define TITLE_CARD_PAL_START (STARRY_IMAGE_PAL_START + STARRY_IMAGE_PAL_SIZE)
#define TITLE_CARD_PAL_SIZE 1
#define TITLE_CARD_GFX_START 0
#define TITLE_CARD_GFX_SIZE 95

#define SHOOTING_STAR_CHARDATA 4
#define SHOOTING_STAR_PAL_START 0
#define SHOOTING_STAR_PAL_SIZE 1
#define SHOOTING_STAR_SPRITE 0
#define SHOOTING_STAR_GFX_START 0
#define SHOOTING_STAR_GFX_SIZE 512

#define STAR_BOCKER_CHARDATA 5
#define STAR_BLOCKER_PAL_START (SHOOTING_STAR_PAL_START + SHOOTING_STAR_PAL_SIZE)
#define STAR_BLOCKER_PAL_SIZE 1
#define STAR_BLOCKER_SPRITE 1
#define STAR_BLOCKER_GFX_START (SHOOTING_STAR_GFX_START + SHOOTING_STAR_GFX_SIZE)
#define STAR_BLOCKER_GFX_SIZE 1

#define PRESS_START_CHARDATA 4
#define PRESS_START_PAL_START 0
#define PRESS_START_PAL_SIZE 1
#define PRESS_START_SPRITE1 0
#define PRESS_START_SPRITE2 1
#define PRESS_START_SPRITE3 2
#define PRESS_START_GFX_START 0
#define PRESS_START_GFX_SIZE 12

#define STARRY_BG_MAX_VEL 

#define TILE_SIZE 8
//define the maximum number of menu items on a single page
#define MAX_MENU_ITEMS 10

//define tile indices for nine-slice
#define BLANK 0
#define TL_1 1
#define TL_2 2
#define TL_3 3
#define TOP_MIDDLE 4
#define TR_1 5
#define TR_2 6
#define TR_3 7
#define LM_UPPER 8
#define MIDDLE_UPPER 9
#define CENTER 10
#define LM 11
#define RM_UPPER 12
#define RM 13
#define LASER_TOP 14
#define LASER_BOTTOM 15

//define constants for the secondary nine-slice tile indices
#define SEC_TOP_LEFT 20
#define SEC_TOP_MIDDLE 21
#define SEC_LEFT 22
#define SEC_CENTER 23

//enums
enum MainMenuState{
	FLASH_WHITE, FADE_TO_TITLE, TITLE_COMET_ANIMATION, TITLE_HOLD, TITLE_FLY_OUT, MAIN_MENU_FLY_IN, MAIN_MENU_HOLD, MAIN_MENU_FLY_OUT, 
};

enum MenuPageIndex{
	MPI_MAIN_MENU, MPI_PLAY_GAME, MPI_EXTRAS, MPI_OPTIONS, MPI_CREDITS, MPI_SOUND_TEST, MPI_MAX
};

typedef enum{
    SCRIPT_RUNNER,
    PAGE_TRANSFER,
    SLIDER,
	SHIFT,
	TOGGLE,
	SOUND_TESTER,
} MenuElement;

//structs
typedef struct StarryBGData{
	u16 xPos;
	u16 yPos;
	s16 xVel;
	s16 yVel;
}StarryBGData;

typedef struct MainMenuData{
	enum MainMenuState state;
	StarryBGData starryBG;
	u16 actionTimer;
	u16 actionTarget;
	s16 xPos;
	s16 yPos;
}MainMenuData;

typedef int (*FunctionPtr)(void);

// Define a union to store different types of data
typedef union{
    FunctionPtr functionPtr;
    int intVal;
    int* intArray;  // Pointer to an array of integers
} MenuElementData;

// Define the struct with the union
typedef struct{
    char* itemName;
    MenuElement menuElement;
    MenuElementData data;
    enum{ FUNC_PTR, INT, INT_ARRAY } dataType; // To keep track of the type stored
} MenuPageItem;

// Define the MenuPage struct with an array of MenuPageItem
typedef struct{
    MenuPageItem items[MAX_MENU_ITEMS];  // Fixed-size array of MenuPageItem
    size_t itemCount;                    // Number of items currently in the array
	char* pageName;
} MenuPage;

//globals
extern const unsigned short startfield_sampleTiles[1168];
extern const unsigned short startfield_sampleMetaTiles[4096];
extern const unsigned short startfield_samplePal[16];
extern const unsigned short sprTitleLogoTiles[1520];
extern const unsigned short sprTitleLogoMap[256];
extern const unsigned short sprTitleLogoPal[16];
extern const unsigned short shootingStarTiles[8192];
extern const unsigned short shootingStarPal[16];
extern cu8 shootingStarXPos[16];
extern cu8 shootingStarYPos[16];
extern const unsigned short starBlockerTiles[256];
extern const unsigned short starBlockerPal[16];
extern const unsigned short sprTitlePressStartTextTiles[192];
extern const unsigned short sprTitlePressStartTextPal[16];
extern const unsigned short main_menu_starfieldTiles[1632];
extern const unsigned short main_menu_starfieldMetaTiles[4096];
extern const unsigned short main_menu_starfieldPal[16];
extern cu16 titleFlyOutYLUT[11];

//local functions
void mainMenuInitialize();
void mainMenuNormal();
void mainMenuEnd();
void scrollStarryBG();
void drawNineSliceWindow(int x, int y, int width, int height);
void drawSecondaryNineSliceWindowStyle(int x, int y, int width, int height);
void drawTile(int x, int y, int tileIndex, bool flipHorizontal, bool flipVertical);
int snapToGrid(int value);
void startMatch();
int menuExecNewGame();
int menuExecContinue();
int menuExecLoadGame();
int menuExecOptionsApplyChanges();
int menuExecPlayBGM();
int menuExecPlaySFX();

void printMenuPageItem(const MenuPageItem* item);
void printMenuPage(const MenuPage* menuPage);

size_t menuSize;

//external functions
#endif