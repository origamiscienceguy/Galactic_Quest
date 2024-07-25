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
#define STARRY_IMAGE_GFX_SIZE 584

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

//local functions
void mainMenuInitialize();
void mainMenuNormal();
void mainMenuEnd();
void processCameraMainMenu();

//external functions
#endif

