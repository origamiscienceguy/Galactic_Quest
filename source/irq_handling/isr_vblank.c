#include "GBA_map.h"
extern u8 vblankHit;

void vblankISR(){
	if(vblankHit == 0){
		//if vblankHit is 0, it means that not all tasks completed
		//trigger a DMA3 interrupt, which is the lowest priority interrupt.
		//That interrupt service routine will save the progress of the current task.
		REG_DMA3SAD = 0;
		REG_DMA3DAD = 0;
		REG_DMA3CNT = DMA_COUNT(1) | DMA_DST_FIXED | DMA_SRC_FIXED | DMA_16 | DMA_AT_NOW | DMA_IRQ | DMA_ENABLE;
	}
	else{
		//if vblankHit was already 0, it means all tasks completed. We are in the wait for vblank function
		//set the variable to 1 so that we escape and start the game loop again
		vblankHit = 0;
	}
	//do other vblank stuff if needed
}