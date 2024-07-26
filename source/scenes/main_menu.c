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
	REG_BG0CNT = BG_4BPP | BG_SBB(STARRY_IMAGE_TILEMAP) | BG_CBB(STARRY_IMAGE_CHARDATA) | BG_PRIO(3) | BG_REG_64x64; //starry background layer
	REG_BG1CNT = BG_4BPP | BG_SBB(TITLE_CARD_TILEMAP) | BG_CBB(TITLE_CARD_CHARDATA) | BG_PRIO(2) | BG_REG_32x32; //title screen layer
	REG_BLDCNT = BLD_TOP(BLD_BG2 | BLD_BACKDROP) | BLD_WHITE;
	REG_BLDY = BLDY(0);
	REG_BG0HOFS = 40;
	REG_BG0VOFS = 100;
	REG_BG1HOFS = 512 - 15;
	REG_BG1VOFS = 512 - 43;
	
	//send the palettes
	memcpy32(&paletteBufferBg[STARRY_IMAGE_PAL_START << 4], startfield_samplePal, sizeof(startfield_samplePal) >> 2);
	memcpy32(&paletteBufferBg[TITLE_CARD_PAL_START << 4], sprTitleLogoPal, sizeof(sprTitleLogoPal) >> 2);

	paletteData[0].size = 16;
	paletteData[0].position = pal_bg_mem;
	paletteData[0].buffer = paletteBufferBg;
	
	paletteData[1].size = 8;
	paletteData[1].position = pal_obj_mem;
	paletteData[1].buffer = (void *)shootingStarPal;
	
	//send the background tiles
	memcpy32(&characterBuffer0[STARRY_IMAGE_GFX_START << 5], startfield_sampleTiles, sizeof(startfield_sampleTiles) >> 2);
	characterData[0].position = tile_mem[STARRY_IMAGE_CHARDATA];
	characterData[0].buffer = (void *)characterBuffer0;
	characterData[0].size = sizeof(characterBuffer0) >> 2;
	
	memcpy32(&characterBuffer1[TITLE_CARD_GFX_START << 5], sprTitleLogoTiles, sizeof(sprTitleLogoTiles) >> 2);
	characterData[1].position = tile_mem[TITLE_CARD_CHARDATA];
	characterData[1].buffer = (void *)characterBuffer1;
	characterData[1].size = sizeof(characterBuffer1) >> 2;
	
	memcpy32(&characterBuffer4[SHOOTING_STAR_GFX_START << 5], shootingStarTiles, sizeof(shootingStarTiles) >> 2);
	characterData[4].position = tile_mem[SHOOTING_STAR_CHARDATA];
	characterData[4].buffer = (void *)characterBuffer4;
	characterData[4].size = sizeof(shootingStarTiles) >> 2;
	
	//send the tilemaps;
	
	tilemapData[0].position = &se_mem[STARRY_IMAGE_TILEMAP];
	tilemapData[0].buffer = (void *)startfield_sampleMetaTiles;
	tilemapData[0].size = sizeof(startfield_sampleMetaTiles) >> 2;
	
	tilemapData[1].position = &se_mem[TITLE_CARD_TILEMAP];
	tilemapData[1].buffer = (void *)sprTitleLogoMap;
	tilemapData[1].size = sizeof(sprTitleLogoMap) >> 2;
	
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
			mainMenuData.actionTarget = 32;
			mainMenuData.actionTimer = 0;
		}
		else{
			mainMenuData.actionTimer++;
		}
		IOBuffer[0] = 16 - mainMenuData.actionTimer;
		IOData.position = (void *)&REG_BLDY;
		IOData.buffer = IOBuffer;
		IOData.size = 1;
		break;
	case TITLE_COMET_ANIMATION:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_HOLD;
		}
		else{
			mainMenuData.actionTimer++;
			u8 starFrame = mainMenuData.actionTimer >> 2;
			objectBuffer[SHOOTING_STAR_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(shootingStarYPos[starFrame]);
			objectBuffer[SHOOTING_STAR_SPRITE].attr1 = ATTR1_SIZE_64 | ATTR1_X(shootingStarXPos[starFrame]);
			objectBuffer[SHOOTING_STAR_SPRITE].attr2 = ATTR2_ID(starFrame << 5) | ATTR2_PRIO(0) | ATTR2_PALBANK(0);
			
			OAMData.position = (void *)oam_mem;
			OAMData.buffer = objectBuffer;
			OAMData.size = sizeof(objectBuffer) >> 2;
		}
	default:
		break;
	}
	
}

void mainMenuEnd(){

}

void processCameraMainMenu(){

}