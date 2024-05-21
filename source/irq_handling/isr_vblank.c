#include "tonc.h"
#include "main.h"

void vblankISR(){
	gameLoopState = WORKING;
	gameState = 0;
	//perform the calculations that absolutely must happen every frame
	criticalUpdates();
}