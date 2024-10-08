#ifndef introh
#define introh

//includes
#include "scenes.h"
#include "audio_engine_external.h"

//constants
#define GBAJAM_WAIT_FRAMES 228
#define AUDIO_BY_WAIT_FRAMES 250
#define PIXEL_PLAY_STUDIOS_WAIT_FRAMES 370
#define BLACK_SCREEN_HOLD_WAIT_FRAMES 10
#define FINAL_TRANSITION_WAIT_FRAMES 110

//enums
enum IntroState{
	GBAJAM_ENTER, GBAJAM_HOLD, GBAJAM_LEAVE, AUDIO_ENGINE_ENTER, AUDIO_ENGINE_HOLD, AUDIO_ENGINE_LEAVE, STUDIO_ENTER, STUDIO_HOLD, STUDIO_LEAVE, BLACK_SCREEN_HOLD, BEFORE_FINAL_TRANSITION
};

//structs
typedef struct IntroData{
	enum IntroState state;
	u16 actionTimer;
	u16 actionTarget;
}IntroData;

//globals
extern const unsigned short Intro_GBAJamBitmap[19200];
extern const unsigned short Intro_GBAJamPal[256];
extern const unsigned short Intro_Audio_engineBitmap[19200];
extern const unsigned short Intro_Audio_enginePal[256];
extern const unsigned short Intro_studioBitmap[19200];
extern const unsigned short Intro_studioPal[256];

//local functions
void introInitialize();
void introNormal();
void introEnd();

//external functions
#endif

