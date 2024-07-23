#ifndef introh
#define introh

//includes
#include "scenes.h"
#include "audio_engine_external.h"

//constants

//enums
enum IntroState{
	GBAJAM_ENTER, GBAJAM_HOLD, GBAJAM_LEAVE, AUDIO_ENGINE_ENTER, AUDIO_ENGINE_HOLD, AUDIO_ENGINE_LEAVE, STUDIO_ENTER, STUDIO_HOLD, STUDIO_LEAVE 
};

//structs
typedef struct IntroData{
	enum IntroState state;
	u8 actionTimer;
	u8 actionTarget;
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

