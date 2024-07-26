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
#define SHOOTING_STAR_SPRITE 1
#define SHOOTING_STAR_GFX_START 0
#define SHOOTING_STAR_GFX_SIZE 32

#define STAR_BOCKER CHARDATA 4
#define STAR_BLOCKER_GFX_START (SHOOTING_STAR_GFX_START + SHOOTING_STAR_GFX_SIZE)
#define STAR_BLOCKER_GFX_SIZE 1

//enums
enum MainMenuState{
	FLASH_WHITE, FADE_TO_TITLE, TITLE_COMET_ANIMATION, TITLE_HOLD, TITLE_TO_MAIN, MAIN_HOLD
};

//structs
typedef struct MainMenuData{
	enum MainMenuState state;
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

//local functions
void mainMenuInitialize();
void mainMenuNormal();
void mainMenuEnd();
void processCameraMainMenu();

//external functions
#endif

