#include "main.h"

//global variables
Inputs inputs;

int main(){
	globalInitialize();
	//start of the game loop in main.s
	gameLoop();
}

//things that only run once at hard reset should go here.
void globalInitialize(){
	//set the interrupt service routine functions and priority order (first is highest priority)
	u16 priorityList[14] = {IRQ_GAMEPAK, IRQ_HBLANK, IRQ_VCOUNT, IRQ_TIMER2, IRQ_TIMER1, IRQ_SERIAL, IRQ_VBLANK,
						   IRQ_TIMER0, IRQ_TIMER3, IRQ_DMA0, IRQ_DMA1, IRQ_DMA2, IRQ_KEYPAD, IRQ_DMA3};
	void (*isrFunctionPointers[14])() = {cartridgeISR, hblankISR, vcountISR, timer2ISR, timer1ISR, serialISR, vblankISR,
								  timer0ISR, timer3ISR, DMA0ISR, DMA1ISR, DMA2ISR, keypadISR, DMA3ISR};
	//apply the chosen priorities
	setInterruptHandlers(priorityList, (u32 *)isrFunctionPointers);
	//set the master service routine
	*(u32 *)0x03007FFC = (u32)&irqMasterServiceRoutine;
	//enable interrupts
	REG_IME = 0;
	REG_IE = 0x3FFF;
	REG_DISPCNT = DCNT_MODE3 | DCNT_BG2 | DCNT_OBJ;
	REG_DISPSTAT = DSTAT_VBL_IRQ;
	REG_WAITCNT = WS_SRAM_8 | WS_ROM0_N3 | WS_ROM0_S1;
	
	//add some tasks to test the task manager
	addTask((u32)criticalUpdates, 2, CAT_CRITICAL);
	
	//play the test song
	audioInitialize(0);
	
}

//add a task to the task manager. If a task with the same category already exists, replace it.
void addTask(u32 functionPointer, u8 priority, u8 category){
	u8 position = 0;
	u8 taskStatus;
	u8 taskCategory;
	u32 *taskData32 = (u32 *)taskData;
	//search for an available position, or for a matching category
	for(u32 i = 0; i < MAX_TASKS; i++){
		//if a matching category is found, remove it
		taskCategory = taskData[(i*8)+7];
		if(taskCategory == category){
			//load the status of the existing task
			taskStatus = taskData[i*8+2];
			//if the task is not in progress
			if(taskStatus != TASK_IN_PROGRESS){
				//set the task to empty
				taskData[(i*8)+2] = TASK_EMPTY;
				//this is our position
				position = i;
				break;
			}
			//if the task is in progress
			else{
				//set the task as being deleted.
				taskData[(i*8)+2] = TASK_DELETING;
			}
		}
		taskStatus = taskData[(i*8)+6];
		//if an open position is found, mark it as our position. This can be overwritten later.
		if(taskStatus == TASK_EMPTY){
			position = i;
		}
	}
	//set the function pointer
	taskData32[position*2] = functionPointer;
	//set the priority
	taskData[(position * 8) + 4] =  priority;
	//set the category
	taskData[(position * 8) + 7] =  category;
	//set the status to "finished." This task will start on the next frame
	taskData[(position * 8) + 6] = TASK_FINISHED;
}

void removeTask(u8 category){
	u8 taskCategory;
	u8 taskStatus;
	for(u32 i = 0; i < MAX_TASKS; i++){
		taskCategory = taskData[(i*8)+3];
		//if a category match is found
		if(taskCategory == category){
			//load the status
			taskStatus = taskData[i*8+2];
			//if the task is not in progress
			if(taskStatus != TASK_IN_PROGRESS){
				//set the task to empty
				taskData[(i*8)+2] = TASK_EMPTY;
				//set the category to empty
				taskData[(i*8)+3] = CAT_EMPTY;
			}
			//if the task is in progress
			else{
				//set the task as being deleted.
				taskData[(i*8)+2] = TASK_DELETING;
			}
			break;
		}
	}
}

void criticalUpdates(){
	u16 lastInputs; 
	
	//enable interrupts
	REG_IME = 1;
	
	//handle inputs
	lastInputs = inputs.current;
	inputs.current = ~REG_KEYINPUT;
	inputs.pressed = inputs.current & ~lastInputs;
	inputs.held = inputs.current & lastInputs;
	inputs.released = ~inputs.current & lastInputs;
	

	REG_IME = 0;
}

//anything that needs to happen during a soft reset.
void softReset(){
}


















//let me scroll down please