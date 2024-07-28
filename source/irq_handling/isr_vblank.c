#include "tonc.h"
#include "main.h"

void vblankISR(){
	if(gameLoopState == WAITING_FOR_VBLANK){
		updateGraphics();
	}
	else{
		*(vu16 *)0x5000200 += 20;
	}
	gameLoopState = WORKING;
	//perform the calculations that absolutely must happen every frame
	criticalUpdates();
}