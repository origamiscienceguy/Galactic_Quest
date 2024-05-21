#include "tonc.h"
#include "main.h"

void vblankISR(){
	gameLoopState = WORKING;
	//perform the calculations that absolutely must happen every frame
	criticalUpdates();
}