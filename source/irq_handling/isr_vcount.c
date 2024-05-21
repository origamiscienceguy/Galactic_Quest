#include "tonc.h"
#include "main.h"
#include "audio_engine_internal.h"

void vcountISR(){
	//disable vcount interrupts
	REG_DISPSTAT = DSTAT_VBL_IRQ;
	
	//enable the timer and timer interrupts
	REG_TM0CNT = TM_FREQ_1 | TM_ENABLE;
}