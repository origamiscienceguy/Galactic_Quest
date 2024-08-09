#ifndef mainmenuh
#define mainmenuh

//includes
#include "scenes.h"
#include "audio_engine_external.h"

//constants
#define TITLE_DEBUG_MODE 0

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

#define FLYING_COMET_CHARDATA 4
#define FLYING_COMET_PAL_START 0
#define FLYING_COMET_PAL_SIZE 1
#define FLYING_COMET_SPRITE 0
#define FLYING_COMET_GFX_START 0
#define FLYING_COMET_GFX_SIZE 512

#define STAR_BOCKER_CHARDATA 5
#define FLYING_COMET_BLOCKER_PAL_START (FLYING_COMET_PAL_START + FLYING_COMET_PAL_SIZE)
#define FLYING_COMET_BLOCKER_PAL_SIZE 1
#define FLYING_COMET_BLOCKER_SPRITE 1
#define FLYING_COMET_BLOCKER_GFX_START (FLYING_COMET_GFX_START + FLYING_COMET_GFX_SIZE)
#define FLYING_COMET_BLOCKER_GFX_SIZE 1

#define PRESS_START_CHARDATA 4
#define PRESS_START_PAL_START 0
#define PRESS_START_PAL_SIZE 1
#define PRESS_START_SPRITE1 0
#define PRESS_START_SPRITE2 1
#define PRESS_START_SPRITE3 2
#define PRESS_START_GFX_START 0
#define PRESS_START_GFX_SIZE 12

#define MENU_CHARDATA 1
#define MENU_TILEMAP 27
#define MENU_PAL_START 2
#define MENU_GFX_START (TITLE_CARD_GFX_START + TITLE_CARD_GFX_SIZE)
#define MENU_GFX_SIZE 20
#define MENU_TEXT_GFX_START (MENU_GFX_START + MENU_GFX_SIZE)
#define MENU_TEXT_TILE_WIDTH 13
#define MENU_TEXT_GFX_SIZE 156
#define MENU_TEXT_PAL_START 2
#define MENU_TEXT_FOCUSED_GFX_START (MENU_TEXT_GFX_START + MENU_TEXT_GFX_SIZE)
#define MENU_TEXT_FOCUSED_GFX_SIZE 156
#define MENU_TEXT_FOCUSED_PAL_START 2

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

#define TITLE_CAM_PAN_BOTTOM 250
#define TITLE_CAM_PAN_TOP 104
#define FIXED_POINT_SCALE 1000
#define BGM_ID_TITLE 10
#define BGM_ID_MAIN_MENU 0

#define MENU_TEXT_LAYER_ID 1

//enums
enum MainMenuState{
	FLASH_WHITE, FADE_TO_TITLE, TITLE_PAN_UP, TITLE_FLASH, TITLE_REVEAL, TITLE_FLYING_COMET_ANIMATION, TITLE_BEFORE_HOLD, TITLE_HOLD, TITLE_FLY_OUT, MAIN_MENU_FLY_IN, MAIN_MENU_HOLD, MAIN_MENU_FLY_OUT, 
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
typedef struct BGData{
	u16 xPos;
	u16 yPos;
	s16 xVel;
	s16 yVel;
	u16 yScrollTimerCurrent;
	u16 yScrollTimerTarget;
	u16 yScrollStartPos;
	u16 yScrollTargetPos;
}BGData;

typedef struct MainMenuData{
	enum MainMenuState state;
	BGData starryBG;
	BGData titleCardBG;
	BGData menuBG;
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
extern const unsigned short tsMenuUITiles[320];
extern const unsigned short tsMenuUIPal[16];
extern const unsigned short menu_action_focusedTiles[8320];
extern const unsigned short menu_actionTiles[8320];
int yStart, yTarget, titleCardYStart, titleCardYTarget;

//local functions
void mainMenuInitialize();
void mainMenuNormal();
void mainMenuEnd();

void hidePressStart();
void displayPressStart();

void scrollStarryBG(int addedX, int addedY);
void interpolateStarryBG();
void drawNineSliceWindow(int x, int y, int width, int height, int layer);
void drawSecondaryNineSliceWindowStyle(int x, int y, int width, int height, int layer);
void setTile(int x, int y, int tileIndex, bool flipHorizontal, bool flipVertical, int palette, int layer);
void updateBGScrollRegisters(u16 bg0XPos, u16 bg0YPos, u16 bg1XPos, u16 bg1YPos);
void startMatch();
void skipToMenu();
int menuExecNewGame();
int menuExecContinue();
int menuExecLoadGame();
int menuExecOptionsApplyChanges();
int menuExecPlayBGM();
int menuExecPlaySFX();

void loadGFX(u32 VRAMCharBlock, u32 VRAMTileIndex, void *graphicsBasePointer, u32 graphicsTileOffset, u32 numTilesToSend, u32 queueChannel);


int easeInOut(int t, int power);
int easeOutQuint(int t);
int lerp(int a, int b, int t);

void printMenuPageItem(const MenuPageItem* item);
void printMenuPage(const MenuPage* menuPage);

size_t menuSize;

//external functions
#endif