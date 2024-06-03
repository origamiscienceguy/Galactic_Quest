#include "tonc.h"
#include "main.h"

void vblankISR(){
	if(gameLoopState == WAITING_FOR_VBLANK){
		updateGraphics();
	}
	gameLoopState = WORKING;
	//perform the calculations that absolutely must happen every frame
	criticalUpdates();
}