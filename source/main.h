#ifndef mainh
#define mainh

//includes
#include "GBA_map.h"
#include "math.h"
#include "interrupt.h"
#include "audio_engine_external.h"


//constants
#define MAX_TASKS 8 //be sure to update the constant in the assembly file as well

//enums
enum taskCategory{CAT_EMPTY, CAT_CRITICAL, CAT_AUDIO, CAT_AI, CAT_LOGIC, CAT_GRAPHICS};
enum taskStatus{TASK_EMPTY, TASK_READY, TASK_IN_PROGRESS, TASK_FINISHED, TASK_DELETING};

//struct declerations
typedef struct Inputs{
	u16 current;
	u16 pressed;
	u16 held;
	u16 released;
}Inputs;

//global variables
extern u32 globalCounter;
extern u8 taskData[64];
extern Inputs inputs;

//local function declerations
void gameLoop();
void globalInitialize();
void softReset();
void taskManager();
extern __attribute__ ((noreturn)) void gameLoop();
void addTask(u32, u8, u8);
void removeTask(u8);
void criticalUpdates();




#endif


