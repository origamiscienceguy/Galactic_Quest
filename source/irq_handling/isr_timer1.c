#include "GBA_map.h"
#include "audio_engine_internal.h"
extern void timer2ISR();

void timer1ISR(){
	//disable DMA
	REG_DMA1CNT = 0;
	REG_DMA2CNT = 0;
	
	//increase the audio counter
	if(audioProgress == 2){
		audioTimer++;
		audioProgress = 0;
	}
	
	//if we are in an odd frame
	if(audioTimer & 1){
		//set the DMA to play from buffer 2
		REG_DMA1SAD = (u32)soundBuffer2;
		REG_DMA2SAD = (u32)soundBuffer2 + MAX_SAMPLES_IN_ONE_FRAME;
	}
	else{
		//otherwise, play from buffer 1
		REG_DMA1SAD = (u32)soundBuffer1;
		REG_DMA2SAD = (u32)soundBuffer1 + MAX_SAMPLES_IN_ONE_FRAME;
	}
	
	//reenable DMA
	REG_DMA1CNT = DMA_COUNT(4) | DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;
	REG_DMA2CNT = DMA_COUNT(4) | DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;
	
	//if audioError is negative, add the period
	if(audioError < 0){
		audioError += SYNC_PERIOD_FRAMES;
	}
	
	//subtract the frame error
	audioError -= SYNC_ERROR_PER_FRAME;
	
	//set the timer reload value for the next frame's timer
	u32 samplesThisFrame;
	if(audioError < 0){
		samplesThisFrame = MAX_SAMPLES_IN_ONE_FRAME;
	}
	else{
		samplesThisFrame = MAX_SAMPLES_IN_ONE_FRAME - 16;
	}
	REG_TM1D = 0x10000 - samplesThisFrame;
	
	//sync the PSG channels
	/*REG_TM2CNT = 0; //stop the timer
	if(leftoverSamples2 == 0){
		timer2ISR();
	}
	else if(leftoverSamples2 >= samplesLastFrame){
		REG_TM2D = 0x0;
	}
	else{
	REG_TM2D = 0x10000 - (leftoverSamples2 << 3); //set the new timer value
	}
	REG_TM2CNT = REG_TM2CNT = TM_FREQ_64 | TM_IRQ | TM_ENABLE; //restart the timer with the new value
	psgBufferReadIndex = psgBufferPreviousWriteIndex;
	
	samplesLastFrame = samplesThisFrame;
	
	vu16 *vramPtr = ((vu16 *)0x06000000);
	switch(psgBufferWriteIndex){
	case 0:
		vramPtr[0] = 0x7fff;
		vramPtr[1] = 0;
		vramPtr[2] = 0;
		vramPtr[3] = 0;
		break;
	case 1:
		vramPtr[0] = 0;
		vramPtr[1] = 0x7fff;
		vramPtr[2] = 0;
		vramPtr[3] = 0;
		break;
	case 2:
		vramPtr[0] = 0;
		vramPtr[1] = 0;
		vramPtr[2] = 0x7fff;
		vramPtr[3] = 0;
		break;
	case 3:
		vramPtr[0] = 0;
		vramPtr[1] = 0;
		vramPtr[2] = 0;
		vramPtr[3] = 0x7fff;
		break;
	}
	
	vramPtr = ((vu16 *)0x060001E0);
	switch(psgBufferReadIndex){
	case 0:
		vramPtr[0] = 0x7fff;
		vramPtr[1] = 0;
		vramPtr[2] = 0;
		vramPtr[3] = 0;
		break;
	case 1:
		vramPtr[0] = 0;
		vramPtr[1] = 0x7fff;
		vramPtr[2] = 0;
		vramPtr[3] = 0;
		break;
	case 2:
		vramPtr[0] = 0;
		vramPtr[1] = 0;
		vramPtr[2] = 0x7fff;
		vramPtr[3] = 0;
		break;
	case 3:
		vramPtr[0] = 0;
		vramPtr[1] = 0;
		vramPtr[2] = 0;
		vramPtr[3] = 0x7fff;
		break;
	}*/
}