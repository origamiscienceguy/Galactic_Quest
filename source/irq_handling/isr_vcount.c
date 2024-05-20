#include "GBA_map.h"
#include "main.h"
#include "audio_engine_internal.h"

void vcountISR(){
	//disable vcount interrupts
	REG_DISPSTAT = DSTAT_VBL_IRQ;
	
	//enable the timer and timer interrupts
	REG_TM0CNT = TM_FREQ_1 | TM_ENABLE;
	
	//enable the psg timer and timer interrupts
	/*REG_TM2D = 0x0; //set the timer to close to next timer1 interrupt 0xef00
	REG_TM2CNT = TM_FREQ_64 | TM_IRQ | TM_ENABLE;*/
	
	//add the music task to the queue
	addTask((u32)processAudio, 7, CAT_AUDIO);
}