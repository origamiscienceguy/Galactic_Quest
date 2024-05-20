#ifndef interrupth
#define interrupth

#include "GBA_map.h"

//global variables
extern u32 irqPriorityTable[21];

//global function declerations
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