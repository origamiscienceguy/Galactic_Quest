#ifndef interrupth
#define interrupth

#include "tonc.h"

//constants

//enums

//structs


//globals
//this is defined inside the interrupt master service routine assembly function
extern u32 irqPriorityTable[21];

//local functions

//external functions
extern void irqMasterServiceRoutine();
extern void setInterruptHandlers(u16 *, u32 *);
extern void vblankISR();
extern void hblankISR();
extern void vcountISR();
extern void timer0ISR();
extern void timer1ISR();
extern void timer2ISR();
extern void timer3ISR();
extern void serialISR();
extern void DMA0ISR();
extern void DMA1ISR();
extern void DMA2ISR();
extern void DMA3ISR();
extern void keypadISR();
extern void cartridgeISR();


#endif