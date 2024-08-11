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

#define STAR_BLOCKER_CHARDATA 5
#define STAR_BLOCKER_PAL_START (FLYING_COMET_PAL_START + FLYING_COMET_PAL_SIZE)
#define STAR_BLOCKER_PAL_SIZE 1
#define STAR_BLOCKER_SPRITE 1
#define STAR_BLOCKER_GFX_START (FLYING_COMET_GFX_START + FLYING_COMET_GFX_SIZE)
#define STAR_BLOCKER_GFX_SIZE 16

#define PRESS_START_CHARDATA 4
#define PRESS_START_PAL_START 0
#define PRESS_START_PAL_SIZE 1
#define PRESS_START_SPRITE1 0
#define PRESS_START_SPRITE2 1
#define PRESS_START_SPRITE3 2
#define PRESS_START_GFX_START 0
#define PRESS_START_GFX_SIZE 12

#define MENU_CHARDATA 1
#define MENU_WINDOW_TILEMAP 27
#define MENU_PAGE_TILEMAP 28
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

#define MENU_PAGE_TEXT_GFX_START 0
#define MENU_PAGE_TEXT_PAL_START 3
#define MENU_PAGE_TEXT_CHARDATA 4
#define MENU_PAGE_TEXT_SPRITE 0

#define MENU_BUTTON_PROMPT_SPRITE_FIRST 1
#define MENU_BUTTON_PROMPT_SPRITE_LAST (MENU_BUTTON_PROMPT_SPRITE_FIRST + 3)
#define MENU_SLIDER_PROMPT_SPRITE1 5
#define MENU_SLIDER_PROMPT_SPRITE2 6
#define MENU_SLIDER_BAR_SPRITE_FIRST 7
#define MENU_SLIDER_BAR_SPRITE_LAST (MENU_SLIDER_BAR_SPRITE_FIRST + (5 * 3) - 1)

#define MENU_SLIDER_PROMPT_PAL 3
#define MENU_SLIDER_PROMPT_GFX_START 16
#define MENU_SLIDER_PROMPT_GFX_SIZE 2

#define MENU_TOGGLE_PROMPT_PAL 3
#define MENU_TOGGLE_PROMPT_GFX_START (MENU_SLIDER_PROMPT_GFX_START + MENU_SLIDER_PROMPT_GFX_SIZE)
#define MENU_TOGGLE_PROMPT_GFX_SIZE 64
#define MENU_TOGGLE_SPRITE_FIRST (MENU_SLIDER_BAR_SPRITE_LAST + 1)
#define MENU_TOGGLE_SPRITE_LAST (MENU_TOGGLE_SPRITE_FIRST + 5)

#define MENU_SLIDER_BARS_PAL 4
#define MENU_SLIDER_BARS_GFX_START (MENU_TOGGLE_PROMPT_GFX_START + MENU_TOGGLE_PROMPT_GFX_SIZE)
#define MENU_SLIDER_BARS_GFX_SIZE 6

#define MENU_BUTTON_PROMPT_PAL 3
#define MENU_BUTTON_PROMPT_GFX_START (MENU_SLIDER_BARS_GFX_START + MENU_SLIDER_BARS_GFX_SIZE)
#define MENU_BUTTON_PROMPT_GFX_SIZE 32

#define FONT_NUMBERS_PROMPT_PAL 3
#define FONT_NUMBERS_GFX_START (MENU_BUTTON_PROMPT_GFX_START + MENU_BUTTON_PROMPT_GFX_SIZE)
#define FONT_NUMBERS_GFX_SIZE 20
#define FONT_NUMBERS_SPRITE_FIRST (MENU_TOGGLE_SPRITE_LAST)
#define FONT_NUMBERS_SPRITE_LAST (FONT_NUMBERS_SPRITE_FIRST + (3 * 5))

#define FONT_PERCENT_PAL 3
#define FONT_PERCENT_GFX_START (FONT_NUMBERS_GFX_START + FONT_NUMBERS_GFX_SIZE)
#define FONT_PERCENT_GFX_SIZE 4
#define FONT_PERCENT_SPRITE_FIRST (FONT_NUMBERS_SPRITE_LAST)
#define FONT_PERCENT_SPRITE_LAST (FONT_PERCENT_SPRITE_FIRST + 5)


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

#define BGM_SINGLE -1 // Marker for single track

// Define the grouped BGM assets
const int bgmGroups[][2] = {
    {_musThemeA_Battle, _musThemeA_Peace},
    {_musThemeB, BGM_SINGLE}, // Group with itself
    {_musThemeB_Battle, BGM_SINGLE}, // Group with itself
    {_musThemeC_Battle, _musThemeC_Peace},
    {_musThemeD_Battle, _musThemeD_Peace},
    {_musMainMenu, BGM_SINGLE}, // Singular Group
    {_musOpening, BGM_SINGLE}, // Singular Group
    {_musTitle, BGM_SINGLE} // Singular Group
};

#define SOUND_TEST_BGM_COUNT (sizeof(bgmGroups) / sizeof(bgmGroups[0]))
#define SOUND_TEST_SFX_COUNT 14
#define SFX_START 11

//enums
enum TitleSceneState{
	FLASH_WHITE, FADE_TO_TITLE, TITLE_PAN_UP, TITLE_FLASH, TITLE_REVEAL, TITLE_FLYING_COMET_ANIMATION, TITLE_BEFORE_HOLD, TITLE_HOLD, TITLE_AFTER_PRESS_START, TITLE_FLY_OUT, MAIN_MENU_FLY_IN, MAIN_MENU_HOLD, MAIN_MENU_FLY_OUT, 
};

enum MainMenuWindowState {
	MMWS_OPENING,
	MMWS_CLOSING,
	MMWS_READY,
	MMWS_ZIPPING,
	MMWS_INITIAL_ZIPPING,
	MMWS_TWEAKING_DATA
};

enum MenuPageIndex{
	MPI_MAIN_MENU, MPI_PLAY_GAME, MPI_SOUND_TEST, MPI_EXTRAS, MPI_CREDITS, MPI_OPTIONS, MPI_MAX
};

enum MenuWindowConfirmDirection {
	MWCD_NEUTRAL,
	MWCD_FORWARD,
	MWCD_BACKWARD,
};

typedef enum{
    ME_SCRIPT_RUNNER,
    ME_PAGE_TRANSFER,
    ME_SLIDER,
	ME_SHIFT,
	ME_TOGGLE,
	ME_SOUND_TESTER,
	ME_CREDITS_DISPLAY,
}MenuElement;

typedef enum{
	MID_SOUND_TEST_BGM,
	MID_SOUND_TEST_SFX,
}MenuItemID;

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
	bool updateDraw;
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
	u8 menuPageTextYPos;
	u8 secondaryNineSliceYOff;
	u8 menuCursorPos;
	u8 windowConfirmDirection;
	int windowCurrTileXPos;
	int windowCurrTileYPos;
	u8 windowTargetTileX;
	u8 windowTargetTileY;
	u8 windowTargetWidth;
	u8 windowTargetHeight;
	bool wrappedAround;
	bool showPageWindowBG;
	u8 windowActionTimer;
	u8 windowActionTarget;
	u8 zipSpeed;
	int menuElementsWidth[MPI_MAX];
	u32 blendMode, eva, evb, ey;
	int evaLerpStart, evaLerpEnd, evbLerpStart, evbLerpEnd;
}MainMenuData;

typedef int (*FunctionPtr)(void);

// Define a union to store different types of data
typedef union{
    FunctionPtr functionPtr;
    int intVal;
	bool boolVal;
    int* intArray;  // Pointer to an array of integers
} MenuElementData;

// Define the struct with the union
typedef struct{
    char* itemName;
    MenuElement menuElement;
    MenuElementData data;
    enum{ MPIDT_FUNC_PTR, MPIDT_INT, MPIDT_BOOL, MPIDT_INT_ARRAY } dataType; // To keep track of the type stored
	int textGFXIndex;				// The graphic row that this text appears on in the tileset
	MenuItemID id;
} MenuPageItem;

// Define the MenuPage struct with an array of MenuPageItem
typedef struct{
    MenuPageItem items[MAX_MENU_ITEMS];  // Fixed-size array of MenuPageItem
	int backPage;
    s32 itemCount;                    // Number of items currently in the array
	char* pageName;
	u8 tileX;
	u8 tileY;
	u8 tileWidth;
	u8 tileHeight;
	u8 pxOffX;
	bool showConfirmPrompt;
	bool showBackPrompt;
	bool showSoundTestPrompts;
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
extern const unsigned short page_name_ui_64x16Tiles[3072];
extern const unsigned short menu_button_prompt_32x16Tiles[512];
extern const unsigned short menu_button_prompt_32x16Pal[16];
extern const unsigned short menu_slider_prompt_8x16Tiles[32];
extern const unsigned short menu_toggle_prompt_64x32Tiles[1024];
extern const unsigned short font_numbers_8x16Tiles[320];
extern const unsigned short font_percent_16x16Tiles[64];
extern const unsigned short menu_slider_bars_8x16Tiles[96];
extern const unsigned short menu_slider_bars_8x16Pal[16];

int yStart, yTarget, titleCardYStart, titleCardYTarget, titleRevealFadeDirection;

//local functions
void mainMenuInitialize();
void mainMenuNormal();
void mainMenuEnd();

void hidePressStart();
void hideAllUIWindowSprites();
void drawPressStart();
void drawStarBlocker(int yPos);

void drawDigit(int sprIndex, int singleDigit, int xPos, int yPos);
int drawNumber(int startIndex, int numberToDraw, int xPos, int yPos, bool rightAlign);
int drawPercent(int sprIndex, int xPos, int yPos);

void scrollStarryBG(int addedX, int addedY);
void interpolateStarryBG();
void updateBGScrollRegisters(u16 bg0XPos, u16 bg0YPos, u16 bg1XPos, u16 bg1YPos);
void startMatch();
void skipToMenu();
void updateObjBuffer();

void initMainMenu();
void initMenuPages(MenuPage menuPages[]);

void updateMainMenu();
void loadMenuGraphics(MenuPage *menuPage);
void drawMainMenu();
void drawMenuPageUIText();
void hidePageUITextSprite();
void setTile(int x, int y, int tileIndex, bool flipHorizontal, bool flipVertical, int palette, int layer);
void drawNineSliceWindow(int x, int y, int width, int height, int layer);
int wrapTileX(int x);
bool tileYInBounds(int y);
void drawLaserRow(int x, int y, int width, int layer, bool wrapAround);
void drawSecondaryNineSliceWindowStyle(int x, int y, int width, int height, int layer);
void drawMenuTextSegment(int tileXPos, int tileYPos, int menuElementPosition, int palette, bool highlighted, int numTextTileColumns);
void directionalInputEnabled();
void menuInputConfirmEnabled();
void menuInputCancelEnabled();
void performPageTransfer(int datIntVal);
void drawMenuButtons(bool hideAll);
void drawSliderPrompt(int xPos, int yPos, int sprIndex, bool flipSpriteHorizontally);
void hideSliderPrompt();
void drawSliderBar(int sprIndex, int xPos, int yPos, int imgIndex, int barValue);
void drawToggle(int sprIndex, int xPos, int yPos, bool isEnabled);
void mainMenuInitBlend();
void mainMenuUpdateBlend(u32 eva, u32 evb);

int menuExecNewGame();
int menuExecContinue();
int menuExecLoadGame();
int menuExecOptionsApplyChanges();
int menuExecPlayBGM(u8 soundIndex);
int menuExecPlaySFX(u8 soundIndex);

void loadGFX(u32 VRAMCharBlock, u32 VRAMTileIndex, void *graphicsBasePointer, u32 graphicsTileOffset, u32 numTilesToSend, u32 queueChannel);

int easeInOut(int t, int power);
int easeOutQuint(int t);
int lerp(int a, int b, int t);
int interpolateValues(int timer, int targetTime, int lerpStartVal, int lerpTargetVal);
int calculatePercentage(int numerator, int denominator);

void printMenuPageItem(const MenuPageItem* item);

size_t menuSize;

//external functions
#endif