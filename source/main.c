#include "main.h"

//global variables
Inputs inputs;
volatile enum GameLoopState gameLoopState;
vu8 gameState;

int main(){
	globalInitialize();
	playNewAsset(_AreaA_DMA_Only);
	//start of the game loop in main.s
	gameLoop();
}

//things that only run once at hard reset should go here.
void globalInitialize(){
	//disable interrupts
	REG_IME = 0;

	//set the interrupt service routine functions and priority order (first is highest priority)
	u16 priorityList[14] = {IRQ_GAMEPAK, IRQ_HBLANK, IRQ_VCOUNT, IRQ_TIMER2, IRQ_TIMER1, IRQ_SERIAL, IRQ_VBLANK,
						   IRQ_TIMER0, IRQ_TIMER3, IRQ_DMA0, IRQ_DMA1, IRQ_DMA2, IRQ_KEYPAD, IRQ_DMA3};
	void (*isrFunctionPointers[14])() = {cartridgeISR, hblankISR, vcountISR, timer2ISR, timer1ISR, serialISR, vblankISR,
								  timer0ISR, timer3ISR, DMA0ISR, DMA1ISR, DMA2ISR, keypadISR, DMA3ISR};
	//apply the chosen priorities
	setInterruptHandlers(priorityList, (u32 *)isrFunctionPointers);
	//set the master service routine
	*(u32 *)0x03007FFC = (u32)&irqMasterServiceRoutine;
	
	//interupt master service routine expects every bit in REG_IE to be set when not in an interrupt state
	REG_IE = 0x3FFF; 
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG2 | DCNT_OBJ;
	//enable Vblank interrupts
	REG_DISPSTAT = DSTAT_VBL_IRQ; 
	REG_WAITCNT = WS_SRAM_8 | WS_ROM0_N3 | WS_ROM0_S1;
	
	//play the test song
	audioInitialize(0);
	
	//enable interrupts
	REG_IME = 1;
	
}

__attribute__ ((noreturn)) void gameLoop(){
gameLoopState = WAITING_FOR_VBLANK;

	while(1){
		while(gameLoopState == WAITING_FOR_VBLANK){
			//bios halt functiion
			Halt();
		}
		if(inputs.pressed & KEY_A){
			playNewAsset(_sfx_test);
		}
		
		gameLoopState = WAITING_FOR_VBLANK;
	}
}

//things that we want to happen every frame, no matter what. This is called from the vblank handler.
void criticalUpdates(){
	u16 lastInputs; 
	
	//handle inputs
	lastInputs = inputs.current;
	inputs.current = ~REG_KEYINPUT;
	inputs.pressed = inputs.current & ~lastInputs;
	inputs.held = inputs.current & lastInputs;
	inputs.released = ~inputs.current & lastInputs;
	
	//increment the background color. (Just for testing the vblank interrupts)
	(*(vu16 *)0x05000000)++;
	
	//process this frame's audio (this will take a majority of a frame.)
	processAudio();
}

//anything that needs to happen during a soft reset.
void softReset(){

}


















//let me scroll down please