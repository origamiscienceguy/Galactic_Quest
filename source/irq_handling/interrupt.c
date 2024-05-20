#include "GBA_map.h"
#include "interrupt.h"

void setInterruptHandlers(u16 *priorityList, u32 *functionList){
	//disable interrupts while this is going
	REG_IME = 0;
	//the irqPriorityTable is formated like this:
	//16 bits mask0, 16 bits mask1, functionptr0, functionptr1, 16 bits mask2, etc.
	u32 i;
	u32 j;
	u16 maskLower = 0;
	u16 maskUpper = 0;
	//first set all the function pointers
	for(i = 0, j = 1; i < 14; i+=2){
		irqPriorityTable[j] = functionList[i];
		irqPriorityTable[j+1] = functionList[i+1];
		j += 3;
	}
	//then set the priority checking bits
	for(i = 0, j = 0; i < 14; i+=2){
		maskLower |= priorityList[i];
		maskUpper |= priorityList[i];
		maskUpper |= priorityList[i+1];
		irqPriorityTable[j] = maskLower | (maskUpper << 16);
		maskLower |= priorityList[i+1];
		j += 3;
	}
	//reenable interrupts
	REG_IME = 1;
}