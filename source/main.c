#include "main.h"

//global variables
Inputs inputs;
volatile enum GameLoopState gameLoopState;
vu8 gameState;
SceneStatus currentScene;
VideoData tilemapData[4] EWRAM_DATA;
VideoData characterData[6] EWRAM_DATA;
VideoData paletteData[2] EWRAM_DATA;
VideoData OAMData EWRAM_DATA;
VideoData IOData[8] EWRAM_DATA;
Options options;

u16 tilemapBuffer0[0x1000] EWRAM_DATA;
u16 tilemapBuffer1[0x1000] EWRAM_DATA;
u16 tilemapBuffer2[0x1000] EWRAM_DATA;
u16 tilemapBuffer3[0x1000] EWRAM_DATA;
u8 characterBuffer0[0x4000] EWRAM_DATA;
u8 characterBuffer1[0x4000] EWRAM_DATA;
u8 characterBuffer2[0x4000] EWRAM_DATA;
u8 characterBuffer3[0x4000] EWRAM_DATA;
u8 characterBuffer4[0x4000] EWRAM_DATA;
u8 characterBuffer5[0x4000] EWRAM_DATA;
u16 paletteBufferBg[0x100] EWRAM_DATA;
u16 paletteBufferObj[0x100] EWRAM_DATA;
OBJ_ATTR objectBuffer[128] EWRAM_DATA;
u16 IOBuffer0[0x200] EWRAM_DATA;
u16 IOBuffer1[0x200] EWRAM_DATA;
u16 IOBuffer2[0x200] EWRAM_DATA;
u16 IOBuffer3[0x200] EWRAM_DATA;
u16 IOBuffer4[0x200] EWRAM_DATA;
u16 IOBuffer5[0x200] EWRAM_DATA;
u16 IOBuffer6[0x200] EWRAM_DATA;
u16 IOBuffer7[0x200] EWRAM_DATA;

// Definition of the volumeTable array
const int volumeTable[MAX_VOLUME + 1] = {
    0, 26, 51, 77, 102, 128, 154, 179, 204, 230, 256
};

// Definition of sound channels
SoundChannel currentBGMIndex[2] = {
	{.assetIndex = 0xFF, .defaultVolume = 255, .soundIndex = 0xFF},
	{.assetIndex = 0xFF, .defaultVolume = 255, .soundIndex = 0xFF}
};
SoundChannel currentSFXIndex[5] = {
	{.assetIndex = 0xFF, .defaultVolume = 255, .soundIndex = 0xFF},
	{.assetIndex = 0xFF, .defaultVolume = 255, .soundIndex = 0xFF},
	{.assetIndex = 0xFF, .defaultVolume = 255, .soundIndex = 0xFF},
	{.assetIndex = 0xFF, .defaultVolume = 255, .soundIndex = 0xFF},
	{.assetIndex = 0xFF, .defaultVolume = 255, .soundIndex = 0xFF}
};

// Define the grouped BGM assets
const int bgmGroups[][2] = {
    {_musOpening, BGM_SINGLE}, 					// Singular Group
    {_musTitle, BGM_SINGLE},					// Singular Group
    {_musMainMenu, BGM_SINGLE},					// Singular Group
    {_musThemeA_Peace, _musThemeA_Battle},		// Dual Group
    {_musThemeB, BGM_SINGLE},					// Singular Group
    {_musThemeB_Battle, BGM_SINGLE},			// Singular Group
	{_musThemeC_Peace, _musThemeC_Battle},		// Dual Group
    {_musThemeD_Peace, _musThemeD_Battle}		// Dual Group
};

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
	currentScene.scenePointer = sceneList[INTRO_SCENE];
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

	
	//handle inputs
	lastInputs = inputs.current;
	inputs.current = ~REG_KEYINPUT;
	inputs.pressed = inputs.current & ~lastInputs;
	inputs.held = inputs.current & lastInputs;
	inputs.released = ~inputs.current & lastInputs;
	
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
	u16 size = 0;
	void *dest = 0;
	void *src = 0;
	
	//update the tilemaps
	for(u32 layer = 0; layer < 4; layer++){
		size = tilemapData[layer].size;
		dest = tilemapData[layer].position;
		src = tilemapData[layer].buffer;
		
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
	for(u32 layer = 0; layer < 6; layer++){
		size = characterData[layer].size;
		dest = characterData[layer].position;
		src = characterData[layer].buffer;
		
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
	for(u32 layer = 0; layer < 2; layer++){
		size = paletteData[layer].size;
		dest = paletteData[layer].position;
		src = paletteData[layer].buffer;
		
		if(size == 0){
			continue;
		}
		else{
			memcpy32(dest, src, size);
		}
		//mark this transfer as completed
		paletteData[layer].size = 0;
	}
	*(vu16 *)0x5000000 = 0;
	
	//update any video IO registers
	for(u32 layer = 0; layer < 8; layer++){
		size = IOData[layer].size;
		dest = IOData[layer].position;
		src = IOData[layer].buffer;
		
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
	size = OAMData.size;
	dest = OAMData.position;
	src = OAMData.buffer;
	
	if(size != 0){
		memcpy32(dest, src, size);
	}
	//mark this transfer as completed
	OAMData.size = 0;
}
















//let me scroll down please