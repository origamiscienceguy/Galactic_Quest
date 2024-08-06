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

// Tile indices for nine slice
#define TOP_LEFT     0
#define TOP          1
#define TOP_RIGHT    2
#define LEFT         3
#define CENTER       4
#define RIGHT        5
#define BOTTOM_LEFT  6
#define BOTTOM       7
#define BOTTOM_RIGHT 8

// Tile size
#define TILE_SIZE 8

// 


//enums
enum MainMenuState{
	FLASH_WHITE, FADE_TO_TITLE, TITLE_COMET_ANIMATION, TITLE_HOLD, TITLE_FLY_OUT, MAIN_MENU_FLY_IN, MAIN_MENU_HOLD, MAIN_MENU_FLY_OUT, 
};

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
void processStarryBG();
void scrollStarryBG();
void drawNineSliceWindow(int width, int height);
void startMatch();

//external functions
#endif

