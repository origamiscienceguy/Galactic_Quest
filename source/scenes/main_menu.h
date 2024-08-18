#ifndef mainmenuh
#define mainmenuh

//includes
#include "scenes.h"
#include "audio_engine_external.h"

//constants

#define SRAM_BLOCK_SIZE sizeof(MapData) // Size of a single save block
#define SRAM_OPTIONS_SIZE sizeof(Options) // Size of the Options data
#define SRAM_BASE 0x0E000000 // Base address of SRAM (example address)

#define SAVE_SLOT_COUNT 3 // Number of save slots

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
#define MENU_WINDOW_TILEMAP (STARRY_IMAGE_TILEMAP - 1)
#define MENU_PAGE_TILEMAP (MENU_WINDOW_TILEMAP - 1)
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
#define TILEMAP_QUADRANT_SIZE 256

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
#define FIXED_POINT_SCALE 1024

#define MENU_TEXT_LAYER_ID 1

//define constants for Sound Test
#define BGM_SINGLE 999 // Marker for single track

#define SOUND_TEST_BGM_COUNT (int)(sizeof(bgmGroups) / sizeof(bgmGroups[0]))
#define SOUND_TEST_SFX_COUNT 15
#define TOTAL_SOUND_COUNT 26 //rip numSounds
#define BGM_COUNT (TOTAL_SOUND_COUNT - SOUND_TEST_SFX_COUNT) // Technically not correct if SFX also has groups, but it works :p
#define SFX_START 11

// Define the grouped BGM assets
const int bgmGroups[][2] = {
    {_musOpening, BGM_SINGLE}, 					// Singular Group
    {_musTitle, BGM_SINGLE},					// Singular Group
    {_musMainMenu, BGM_SINGLE},					// Singular Group
    {_musThemeA_Battle, _musThemeA_Peace},		// Dual Group
    {_musThemeB, BGM_SINGLE},					// Group with itself
    {_musThemeB_Battle, BGM_SINGLE},			// Group with itself
	{_musThemeC_Battle, _musThemeC_Peace},		// Dual Group
    {_musThemeD_Battle, _musThemeD_Peace}		// Dual Group
};

enum BGMList{
	BGM_OPENING,
	BGM_TITLE,
	BGM_MAINMENU,
	BGM_THEMEA,
	BGM_THEMEB,
	BGM_THEMEB_BATTLE,
	BGM_THEMEC,
	BGM_THEMED,
	BGM_MAX
};

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
	MMWS_TWEAKING_DATA,
	MMWS_APPLIED_OPTIONS,
	MMWS_FINALIZING,
	MMWS_ZIPPING_OUT,
	MMWS_DONE
};

enum MenuPageIndex{
	MPI_MAIN_MENU, MPI_PLAY_GAME, MPI_SOUND_TEST, MPI_EXTRAS, MPI_CREDITS, MPI_OPTIONS, MPI_MAX
};

enum MenuWindowConfirmDirection {
	MWCD_NEUTRAL,
	MWCD_FORWARD,
	MWCD_BACKWARD,
};

enum OptionIDs{
	OPTID_MASTER_VOL,
	OPTID_BGM_VOL,
	OPTID_SFX_VOL,
	OPTID_GRID_ENABLED
};

typedef enum MenuElement{
    ME_SCRIPT_RUNNER,
    ME_PAGE_TRANSFER,
    ME_SLIDER,
	ME_SHIFT,
	ME_TOGGLE,
	ME_SOUND_TESTER,
	ME_CREDITS_DISPLAY,
} MenuElement;

typedef enum MenuItemID{
	MID_SOUND_TEST_BGM,
	MID_SOUND_TEST_SFX,
	MID_SOUND_TEST_CANCEL,
	MID_OPT_MAST_VOL,
	MID_OPT_BGM_VOL,
	MID_OPT_SFX_VOL,
	MID_OPT_GRID_ENABLED
} MenuItemID;

enum AUDGROUP_IDS{
    AUDGROUP_MENUSFX,
    AUDGROUP_SOUNDT_SFX,
    AUDGROUP_SOUNDT_BGM,
	AUDGROUP_MAX
};

//structs
typedef struct BGData{
	u16 xPos;
	u16 yPos;
	s16 xVel;
	s16 yVel;
	u16 scrollTimerCurrent;
	u16 scrollTimerTarget;
	u16 scrollStartPos;
	u16 scrollTargetPos;
	bool snappedThisFrame;
}BGData;

typedef struct MainMenuData{
	enum TitleSceneState state;
	enum MainMenuWindowState windowState;
	bool updateBGTileDraw, updateSpriteDraw, updateUITileDraw;
	bool windowFinalizing, hideMenuCursor;
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
	int blendMode, eva, evb, ey;
	int evaLerpStart, evaLerpEnd, evbLerpStart, evbLerpEnd;
} MainMenuData;

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
extern cu16 starryBGPanYPos[100];
extern cu8 titleCardBGPanYPos[9];

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

int starryBGYPosInit, starryBGYPosTarget, titleCardYStart, titleCardYTarget, titleRevealFadeDirection;

// Define the individual audio groups with variable sizes
static const int menuSFXGroup[] = { _sfxMenuCancel, _sfxMenuConfirmA, _sfxMenuConfirmB, _sfxMenuConfirmC, _sfxMenuMove };
static const int soundtestSFXGroup[] = { _sfxCursorMove, _sfxMenuCancel, _sfxMenuConfirmA, _sfxMenuConfirmB, _sfxMenuConfirmC, _sfxMenuMove, _sfxScreenPan, _sfxShipDmgL, _sfxShipDmgM, _sfxShipDmgS, _sfxShipExplodeL, _sfxShipExplodeM, _sfxShipExplodeS, _sfxShipIdle, _sfxShipMove };
static const int soundtestBGMGroup[] = { _musMainMenu, _musOpening };

// Array of pointers to these groups
static const int* audioGroups[AUDGROUP_MAX] = {
    [AUDGROUP_MENUSFX] = menuSFXGroup,
    [AUDGROUP_SOUNDT_SFX] = soundtestSFXGroup,
    [AUDGROUP_SOUNDT_BGM] = soundtestBGMGroup
};

// Array to store the size of each group
static const int audioGroupSizes[AUDGROUP_MAX] = {
    [AUDGROUP_MENUSFX] = sizeof(menuSFXGroup) / sizeof(menuSFXGroup[0]),
    [AUDGROUP_SOUNDT_SFX] = sizeof(soundtestSFXGroup) / sizeof(soundtestSFXGroup[0]),
    [AUDGROUP_SOUNDT_BGM] = sizeof(soundtestBGMGroup) / sizeof(soundtestBGMGroup[0])
};

// Local Functions
void mainMenuInitialize();
void mainMenuNormal();
void mainMenuEnd();

// Init Functions
void initMainMenu();
void initMenuPages(MenuPage menuPages[]);
void mainMenuInitBlend();
void resetMainMenuWindowVariables();

// Update Functions
void scrollStarryBG(int addedX, int addedY);
void interpolateStarryBG(bool scrollYAxis);
void updateBGScrollRegisters(u16 bg0XPos, u16 bg0YPos, u16 bg1XPos, u16 bg1YPos);
void matchBegin();
void updateOptions();
void skipToMenu();
void updateObjBuffer();
void mainMenuUpdateBlend(u32 eva, u32 evb);
void updateMainMenu();
void menuInputConfirmEnabled();
void menuInputCancelEnabled();
void performPageTransfer(int datIntVal);
void endCurrentBGM();

// Menu Functions
int menuExecNewGame();
int menuExecContinue();
int menuExecLoadGame();
int menuExecOptionsApplyChanges();

// Drawing Functions
void setTile(int x, int y, int tileIndex, bool flipHorizontal, bool flipVertical, int palette, int layer);
void hideSprite(int sprIndex);
void hideSpriteRange(int firstSprite, int lastSprite);
void drawPressStart();
void drawStarBlocker(int yPos);
void drawDigit(int sprIndex, int singleDigit, int xPos, int yPos);
int drawNumber(int startIndex, int numberToDraw, int xPos, int yPos, bool rightAlign);
int drawPercent(int sprIndex, int xPos, int yPos);
void drawMenuButtonPrompts(bool hideAll);
void drawSliderPrompt(int xPos, int yPos, int sprIndex, bool flipSpriteHorizontally);
void drawSliderBar(int sprIndex, int xPos, int yPos, int imgIndex, int barValue);
void drawToggle(int sprIndex, int xPos, int yPos, bool isEnabled);
void drawLaserRow(int x, int y, int width, int layer, bool wrapAround);
void drawSecondaryNineSliceWindowStyle(int x, int y, int width, int height, int layer);
void drawMenuTextSegment(int tileXPos, int tileYPos, int menuElementPosition, int palette, bool highlighted, int numTextTileColumns);
void directionalInputEnabled();
void drawMainMenu();
void drawMenuPageUIText();
void drawNineSliceWindow(int x, int y, int width, int height, int layer);

// Helper Functions
void loadGFX(u32 VRAMCharBlock, u32 VRAMTileIndex, void *graphicsBasePointer, u32 graphicsTileOffset, u32 numTilesToSend, u32 queueChannel);
int easeInOut(int t, int power);
int easeOutQuint(int t);
int lerp(int a, int b, int t);
int interpolateValues(int timer, int targetTime, int lerpStartVal, int lerpTargetVal);
int calculatePercentage(int numerator, int denominator);
int wrapTileX(int x);
bool tileYInBounds(int y);
void loadMenuGraphics(MenuPage *menuPage);
bool sfxIsPlaying(int sfxIndex);
int calculateEffectiveVolume(int soundAssetVol, int userVol);
u8 calculateFinalVolume(u8 assetVolume, int userVolume, int masterVolume);
void justLikeUpdateAllVolumesMan();












//enums
typedef enum ShipType{
	NONE = -1, SCOUT, FIGHTER, BOMBER, DESTROYER, CRUISER, BATTLESHIP, CARRIER,
} ShipType;

enum ShipState{
	READY_VISIBLE, READY_HIDDEN, FINISHED_VISIBLE, FINISHED_HIDDEN, WRONG_TEAM_VISIBLE, WRONG_TEAM_HIDDEN, SELECTED, DOCKED, DESTROYED, NOT_PARTICIPATING
};

enum MapState{
	TURN_START, OPEN_MAP, SELECT_A_SHIP, SHIP_SELECTED, SHIP_MOVEMENT_SELECT, SHIP_MOVING, BATTLE, TURN_END, TURN_END_MOVEMENT, TURN_REPLAY, OPEN_TILE_SELECTED,
	BATTLE_SELECT_A_SHIP, AFTER_BATTLE_BEFORE_MOVE, INACTIVE_SHIP_SELECTED, RANGE_CHECK
};

enum TeamState{
	TEAM_ABSENT, TEAM_ACTIVE, TEAM_DEFEATED
};

typedef enum Team{
	RED_TEAM, BLUE_TEAM, GREEN_TEAM, YELLOW_TEAM
} Team;

enum CameraState{
	CAM_STILL, CAM_PANNING, CAM_TRACKING, 
};

enum CursorState{
	CUR_HIDDEN, CUR_STILL, CUR_MOVE_ONCE_1, CUR_MOVE_ONCE_2, CUR_MOVE_ONCE_3, CUR_MOVE_ONCE_WAIT, CUR_MOVE_SLOW_1, CUR_MOVE_SLOW_2, CUR_MOVE_FAST
};

enum CursorDirection{
	CUR_NO_DIRECTION, CUR_RIGHT, CUR_UP, CUR_LEFT, CUR_DOWN, CUR_UP_RIGHT, CUR_UP_LEFT, CUR_DOWN_RIGHT, CUR_DOWN_LEFT
};

enum HighlightState{
	NO_HIGHLIGHT, MOVEMENT_RANGE_HIGHLIGHT, VISIBILITY_HIGHLIGHT, POTENTIAL_ENCOUNTERS_HIGHLIGHT
};

enum WidgetState{
	WIDGET_HIDDEN_LEFT, WIDGET_HIDDEN_RIGHT, WIDGET_STILL_LEFT, WIDGET_STILL_RIGHT, 
	WIDGET_MOVING_LEFT, WIDGET_MOVING_RIGHT, WIDGET_HIDING_LEFT, WIDGET_HIDING_RIGHT,
	WIDGET_EMERGING_LEFT, WIDGET_EMERGING_RIGHT
};

enum GridState{
	NO_GRID, GRID_ON
};

enum SelectAShipMenuState{
	NO_SELECT_A_SHIP_MENU, WAITING_SELECT_A_SHIP_MENU, SELECTING_SELECT_A_SHIP_MENU 
};

enum ActionMenuState{
	NO_ACTION_MENU, WAITING_ACTION_MENU, SELECTING_ACTION_MENU
};

enum ActionMenuOptions{
	MOVE_OPTION, SHOOT_OPTION, END_TURN_OPTION, SEE_RANGE_OPTION, BACK_OPTION
};

//structs
typedef struct ShipData{
	enum ShipType type; //the type of ship this one is
	enum ShipState state; //the state of this ship
	enum Team team; //which team this ship belongs to
	u8 health; //the health of this ship, 100 is max, 0 is dead
	u8 xPos; //the x position of this ship on the map
	u8 yPos; //the y position of this ship on the map
	s8 xVel; //the x velocity of this ship
	s8 yVel; //the y velocity of this ship
	u8 teamLink; //the index of the next ship of the same team, forming a linked list.
	u8 sameTileLink; //the index of the next ship in the same tile as this one, forming a linked list. If this points to itself, there are no others
	u8 padding[6];
}ShipData;

typedef struct SelectedShip{
	u8 index; //the index of the currently selected ship
	u8 angle; //the angle the ship graphic is facing, right is 0 and 256, cw
	s16 xPos; //the current pixel position of the center of the selected ship
	s16 yPos; //the current pixel position of the center of the selected ship
	s16 xInitial; //the initial position of the selected ship before movement
	s16 yInitial; //the initial position of the selected ship before movement
	s16 xTarget; //the position the selected ship is moving towards
	s16 yTarget; //the position the selected ship is moving towards
	u8 animationTimer; //the timer of the selected ships animation
	u8 actionTimer; //the timer of the selected ships movement
	u8 actionTarget; //how long this movement should take
}SelectedShip;

typedef struct TeamData{
	enum TeamState state; //the state of this team in this particular battle
	u8 firstShip; //the index of the first ship in every team a new turn will begin with this ship highlighted
	u8 numStartingShips; //the number of ships each team began the battle with
	u8 numAliveShips; //the number of ships each team has that is currently alive
}TeamData;

typedef struct CameraData{
	enum CameraState state; //what the camera is currently doing
	u8 actionTimer; //how long the camera has been performing an action
	u8 actionTarget; //how long the camera is supposed to be performing an action
	s16 xPos; //the current x pixel position of the top left pixel on screen. 16.48 fixed point
	s16 yPos; //the current y pixel position of the top left pixel on screen. 16.48 fixed point
	s16 xStartingPos; //the initial x position of the camera for this movement
	s16 yStartingPos; //the initial y position of the camera for this movement
	s16 xTargetPos; //the x position the camera is currently seeking towards
	s16 yTargetPos; //the y position the camera is currently seeking towards
}CameraData;

typedef struct CursorData{
	s16 xPos; //the pixel position of the cursor in the x axis
	s16 yPos; //the pixel position of the cursor in the y axis
	u8 selectXPos; //the map position that the cursor will select
	u8 selectYPos; //the map position that the cursor will select
	enum CursorState state; //the state of the cursor
	enum CursorDirection direction; //the direction of the cursor
	u8 counter; //how long the button to move the cursor has been held down
}CursorData;

typedef struct HighlightData{
	enum HighlightState state;
	enum GridState gridState;
}HighlightData;

typedef struct MinimapData{
	enum WidgetState widgetState;
	u8 actionTimer;
	u8 actionTarget;
	u8 updateRequest;
}MinimapData;

typedef struct SelectAShipMenu{
	enum SelectAShipMenuState state;
	enum WidgetState widgetState;
	u8 currentSelection;
	u8 currentTopOption;
	u8 actionTarget;
	u8 actionTimer;
	u8 shipCount;
	u8 downHeldCounter;
	u8 upHeldCounter;
}SelectAShipMenu;

typedef struct ActionMenu{
	enum ActionMenuState state;
	enum WidgetState widgetState;
	u8 actionTarget;
	u8 actionTimer;
	u8 currentSelection;
	u8 moveOption;
	u8 checkRangeOption;
	u8 shootOption;
	u8 endTurnOption;
	u8 numOptions;
}ActionMenu;

#define MAX_SHIPS 256
#define NUM_TEAMS 4

typedef struct MapData{
	enum MapState state; //what is the stage of the game are we in
	enum Team teamTurn; //who's turn is it
	u8 turnNum; //how many turns have elapsed
	u8 xSize; //the number of tiles large the map is in the x direction
	u8 ySize; //number of tiles in the y direction
	u8 numShips; //the number of ships on the map.
	u8 actionTimer; //how many frames have elapsed in a state action
	u8 actionTarget; //how many frames a state action should take
	ShipData ships[MAX_SHIPS]; //the array of information about every ship in this battle
	TeamData teams[NUM_TEAMS]; //the array of information about each team in the battle
	CameraData camera; //the struct containing data about the camera
	SelectedShip selectedShip; //the struct containing data about the currently selected ship
	CursorData cursor; //the struct containing data about the cursor
	HighlightData highlight; //the data about the highlight layer
	MinimapData minimap; //the data about the minimap layer
	SelectAShipMenu selectAShip; //the data about the select-a-ship menu
	ActionMenu actionMenu; //the data about the action meny
	u8 saveSlot;
}MapData;

//external functions
extern MapData mapData;






// SRAM Functions
u32 GetSaveSlotAddress(u8 saveSlot);
u32 GetOptionsAddress(void);

void LoadGame(MapData *mapData, u8 saveSlot);
void LoadOptions(Options *options);

void SaveGame(const MapData *mapData, u8 saveSlot);
void SaveOptions(const Options *options);

void InitializeMapData(MapData *mapData);
void InitializeOptions(Options *options);





#endif


