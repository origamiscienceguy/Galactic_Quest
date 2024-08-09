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

#define FLYING_COMET_CHARDATA 4
#define FLYING_COMET_PAL_START 0
#define FLYING_COMET_PAL_SIZE 1
#define FLYING_COMET_SPRITE 0
#define FLYING_COMET_GFX_START 0
#define FLYING_COMET_GFX_SIZE 512

#define STAR_BOCKER_CHARDATA 5
#define STAR_BLOCKER_PAL_START (FLYING_COMET_PAL_START + FLYING_COMET_PAL_SIZE)
#define STAR_BLOCKER_PAL_SIZE 1
#define STAR_BLOCKER_SPRITE 1
#define STAR_BLOCKER_GFX_START (FLYING_COMET_GFX_START + FLYING_COMET_GFX_SIZE)
#define STAR_BLOCKER_GFX_SIZE 1

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

#define BUTTON_PROMPT_PAL 3
#define BUTTON_PROMPT_GFX_START (MENU_TEXT_FOCUSED_GFX_START + MENU_TEXT_FOCUSED_GFX_SIZE)
#define BUTTON_PROMPT_GFX_SIZE 16

#define MENU_TITLE_TEXT_PAL 3
#define MENU_TITLE_TEXT_GFX_START (BUTTON_PROMPT_GFX_START + BUTTON_PROMPT_GFX_SIZE)
#define MENU_TITLE_TEXT_GFX_SIZE 32


#define STARRY_BG_MAX_VEL 

#define TILE_SIZE 8
#define TILEMAP_WIDTH 30
#define TILEMAP_HEIGHT 20

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
#define SEC_TOP_LEFT 16
#define SEC_TOP_MIDDLE 17
#define SEC_LEFT 18
#define SEC_CENTER 19

#define TITLE_CAM_PAN_BOTTOM 250
#define TITLE_CAM_PAN_TOP 104
#define FIXED_POINT_SCALE 1000

#define MENU_TEXT_LAYER_ID 1

//enums
enum TitleSceneState{
	FLASH_WHITE, FADE_TO_TITLE, TITLE_PAN_UP, TITLE_FLASH, TITLE_REVEAL, TITLE_FLYING_COMET_ANIMATION, TITLE_BEFORE_HOLD, TITLE_HOLD, TITLE_AFTER_PRESS_START, TITLE_FLY_OUT, MAIN_MENU_FLY_IN, MAIN_MENU_HOLD, MAIN_MENU_FLY_OUT, 
};

enum MainMenuWindowState {
	MMWS_OPENING,
	MMWS_CLOSING,
	MMWS_READY,
	MMWS_ZIPPING,
	MMWS_INITIAL_ZIPPING
};

enum MenuPageIndex{
	MPI_MAIN_MENU, MPI_PLAY_GAME, MPI_EXTRAS, MPI_OPTIONS, MPI_CREDITS, MPI_SOUND_TEST, MPI_MAX
};

typedef enum{
    ME_SCRIPT_RUNNER,
    ME_PAGE_TRANSFER,
    ME_SLIDER,
	ME_SHIFT,
	ME_TOGGLE,
	ME_SOUND_TESTER,
}MenuElement;

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
	enum TitleSceneState state;
	enum MainMenuWindowState windowState;
	BGData starryBG;
	BGData titleCardBG;
	BGData menuBG;
	u16 actionTimer;
	u16 actionTarget;
	s16 xPos;
	s16 yPos;
	int winSliceWidth;
	int winSliceHeight;
	int currMenuPage;
	u8 menuCursorPos;
	int windowTileXPos;
	u8 windowTileYPos;
	u8 windowTargetWidth;
	u8 windowTargetHeight;
	bool wrappedAround;
	bool showPageWindow;
	u8 windowActionTimer;
	u8 zipSpeed;
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
    enum{ MPIDT_FUNC_PTR, MPIDT_INT, MPIDT_INT_ARRAY } dataType; // To keep track of the type stored
	int textGFXIndex;				// The graphic row that this text appears on in the tileset
} MenuPageItem;

// Define the MenuPage struct with an array of MenuPageItem
typedef struct{
    MenuPageItem items[MAX_MENU_ITEMS];  // Fixed-size array of MenuPageItem
    s32 itemCount;                    // Number of items currently in the array
	char* pageName;
	u8 tileX;
	u8 tileY;
	u8 tileWidth;
	u8 tileHeight;
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
extern cu8 starBlockerYPos[31];
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
extern const unsigned short page_name_ui_64x16Tiles[2560];
extern const unsigned short menu_button_promptsTiles[256];
extern const unsigned short menu_button_promptsPal[16];
int yStart, yTarget, titleCardYStart, titleCardYTarget, titleRevealFadeDirection;

//local functions
void mainMenuInitialize();
void mainMenuNormal();
void mainMenuEnd();

void hidePressStart();
void drawPressStart();
void drawStarBlocker(int yPos);

void scrollStarryBG(int addedX, int addedY);
void interpolateStarryBG();
void updateBGScrollRegisters(u16 bg0XPos, u16 bg0YPos, u16 bg1XPos, u16 bg1YPos);
void startMatch();
void skipToMenu();
void updateObjBuffer();

void initMainMenu();
void updateMainMenu();
void drawMainMenu();
void setTile(int x, int y, int tileIndex, bool flipHorizontal, bool flipVertical, int palette, int layer);
void drawNineSliceWindow(int x, int y, int width, int height, int layer);
int wrapX(int x);
bool isInBounds(int y);
void drawLaserRow(int x, int y, int width, int layer, bool wrapAround);
void drawSecondaryNineSliceWindowStyle(int x, int y, int width, int height, int layer);
void drawMenuTextSegment(int nineSliceWidth, int tileXPos, int tileYPos, int menuElementPosition, int palette, bool highlighted);

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

size_t menuSize;

//external functions
#endif