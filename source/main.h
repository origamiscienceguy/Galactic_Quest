#ifndef mainh
#define mainh

//includes
#include "tonc.h"
#include "scenes.h"
#include "interrupt.h"
#include "audio_engine_external.h"


//constants

//enums
enum GameLoopState{WORKING, WAITING_FOR_VBLANK};

//structs
typedef struct Inputs{
	u16 current;
	u16 pressed;
	u16 held;
	u16 released;
}Inputs;

//globals
extern u32 globalCounter;
extern u8 taskData[64];
extern Inputs inputs;
extern volatile enum GameLoopState gameLoopState;
extern vu8 gameState;

//local functions
void gameLoop();
void globalInitialize();
void softReset();
__attribute__ ((noreturn)) void gameLoop();
void criticalUpdates();
void sceneManager();
void updateGraphics();

//external functions

#endif


