#include "main_menu.h"

MainMenuData mainMenuData;

Scene mainMenuScene = {
	.initialize = &mainMenuInitialize,
	.intro = 0,
	.normal = &mainMenuNormal,
	.pause = 0,
	.outro = 0,
	.end = &mainMenuEnd,
};

void mainMenuInitialize(){
	REG_DISPCNT = DCNT_MODE0; //black screen
	REG_BG0CNT = BG_4BPP | BG_SBB(STARRY_IMAGE_TILEMAP) | BG_CBB(STARRY_IMAGE_CHARDATA) | BG_PRIO(3); //starry background layer
	REG_BLDCNT = BLD_TOP(BLD_BG2 | BLD_BACKDROP) | BLD_WHITE;
	REG_BLDY = BLDY(0);
	
	//send the palettes
	memcpy32(&paletteBufferBg[STARRY_IMAGE_PAL_START << 4], startfield_samplePal, sizeof(startfield_samplePal) >> 2);

	paletteData[0].size = 16 >> 2;
	paletteData[0].position = pal_bg_mem;
	paletteData[0].buffer = paletteBufferBg;
	
	//send the background tiles
	characterData[0].position = tile_mem[STARRY_IMAGE_CHARDATA];
	characterData[0].buffer = (void *)startfield_sampleTiles;
	characterData[0].size = sizeof(startfield_sampleTiles) >> 2;
	
	//send the tilemap
	tilemapData[0].position = &se_mem[STARRY_IMAGE_TILEMAP];
	tilemapData[0].buffer = (void *)startfield_sampleMetaTiles;
	tilemapData[0].size = sizeof(startfield_sampleMetaTiles) >> 2;
	
	mainMenuData.actionTarget = 32;
	mainMenuData.actionTimer = 0;
	mainMenuData.state = FLASH_WHITE;
	currentScene.state = NORMAL;
}

void mainMenuNormal(){
	switch(mainMenuData.state){
	case FLASH_WHITE:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
			mainMenuData.actionTarget = 16;
			mainMenuData.actionTimer = 0;
			mainMenuData.state = FADE_TO_TITLE;
		}
		else{
			mainMenuData.actionTimer++;
		}
		IOBuffer[0] = mainMenuData.actionTimer >> 1;
		IOData.position = (void *)&REG_BLDY;
		IOData.buffer = IOBuffer;
		IOData.size = 1;
		break;
	case FADE_TO_TITLE:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_COMET_ANIMATION;
		}
		else{
			mainMenuData.actionTimer++;
		}
		IOBuffer[0] = 16 - mainMenuData.actionTimer;
		IOData.position = (void *)&REG_BLDY;
		IOData.buffer = IOBuffer;
		IOData.size = 1;
		break;
	default:
		break;
	}
	
}

void mainMenuEnd(){

}

void processCameraMainMenu(){

}