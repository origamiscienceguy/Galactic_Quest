#include "main.h"

//global variables
Inputs inputs;
volatile enum GameLoopState gameLoopState;
vu8 gameState;
SceneStatus currentScene;
VideoData tilemapData[4] EWRAM_DATA;
VideoData characterData[4] EWRAM_DATA;
VideoData paletteData[16] EWRAM_DATA;
VideoData OAMData[16] EWRAM_DATA;
VideoData IOData[16] EWRAM_DATA;

int main(){
	globalInitialize();
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
	currentScene.scenePointer = sceneList[GAMEPLAY];
	currentScene.state = INITIALIZE;
	
	while(1){
		while(gameLoopState == WAITING_FOR_VBLANK){
			//bios halt function
			Halt();
		}
		sceneManager();
		
		gameLoopState = WAITING_FOR_VBLANK;
	}
}

//things that we want to happen every frame, no matter what. This is called from the vblank handler.
void criticalUpdates(){
	u16 lastInputs; 
	static u8 currentAsset = 0;
	static u8 currentAssetIndex = 0;
	extern cu16 numAssets;
	
	//handle inputs
	lastInputs = inputs.current;
	inputs.current = ~REG_KEYINPUT;
	inputs.pressed = inputs.current & ~lastInputs;
	inputs.held = inputs.current & lastInputs;
	inputs.released = ~inputs.current & lastInputs;
	
	if(inputs.pressed & KEY_SELECT){
		endAsset(currentAssetIndex);
		if((currentAsset + 1) == numAssets){
			currentAsset = 0;
		}
		else{
			currentAsset++;
		}
		currentAssetIndex = playNewAsset(currentAsset);
	}
	
	//process this frame's audio (this will take a majority of a frame.)
	processAudio();
}

//anything that needs to happen during a soft reset.
void softReset(){

}

void sceneManager(){
	currentScene.sceneCounter++;
	switch(currentScene.state){
	case INITIALIZE:
		(*currentScene.scenePointer->initialize)();
		break;
	case INTRO:
		(*currentScene.scenePointer->intro)();
		break;
	case NORMAL:
		(*currentScene.scenePointer->normal)();
		break;
	case PAUSE:
		(*currentScene.scenePointer->pause)();
		break;
	case OUTRO:
		(*currentScene.scenePointer->outro)();
		break;
	case END:
		(*currentScene.scenePointer->end)();
		break;
	}
	
};

void updateGraphics(){
	//update the tilemaps
	for(u32 layer = 0; layer < 4; layer++){
		u16 size = tilemapData[layer].size;
		void *dest = tilemapData[layer].position;
		void *src = tilemapData[layer].buffer;
		
		if(size == 0){
			continue;
		}
		else{
			memcpy32(dest, src, size);
		}
		//mark this transfer as completed
		tilemapData[layer].size = 0;
	}
	
	//update the character data
	for(u32 layer = 0; layer < 4; layer++){
		u16 size = characterData[layer].size;
		void *dest = characterData[layer].position;
		void *src = characterData[layer].buffer;
		
		if(size == 0){
			continue;
		}
		else{
			memcpy32(dest, src, size);
		}
		//mark this transfer as completed
		characterData[layer].size = 0;
	}
	
	//update the palette data
	for(u32 layer = 0; layer < 16; layer++){
		u16 size = paletteData[layer].size;
		void *dest = paletteData[layer].position;
		void *src = paletteData[layer].buffer;
		
		if(size == 0){
			continue;
		}
		else{
			memcpy32(dest, src, size);
		}
		//mark this transfer as completed
		paletteData[layer].size = 0;
	}
	
	//update any video IO registers
	for(u32 layer = 0; layer < 16; layer++){
		u16 size = IOData[layer].size;
		void *dest = IOData[layer].position;
		void *src = IOData[layer].buffer;
		
		if(size == 0){
			continue;
		}
		else{
			memcpy32(dest, src, size);
		}
		//mark this transfer as completed
		IOData[layer].size = 0;
	}
	
	//update any OAM entries
	for(u32 layer = 0; layer < 16; layer++){
		u16 size = OAMData[layer].size;
		void *dest = OAMData[layer].position;
		void *src = OAMData[layer].buffer;
		
		if(size == 0){
			continue;
		}
		else{
			memcpy32(dest, src, size);
		}
		//mark this transfer as completed
		OAMData[layer].size = 0;
	}
}
















//let me scroll down please