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
	REG_BLDCNT = BLD_TOP(BLD_BG2 | BLD_BACKDROP | BLD_OBJ) | BLD_WHITE;
	REG_BLDY = BLDY(0);
	REG_BG0HOFS = 512-16;
	REG_BG0VOFS = 104;
	REG_BG1HOFS = 512 - 15;
	REG_BG1VOFS = 512 - 43;
	
	//send the palettes
	memcpy32(&paletteBufferBg[STARRY_IMAGE_PAL_START << 4], startfield_samplePal, sizeof(startfield_samplePal) >> 2);
	memcpy32(&paletteBufferBg[TITLE_CARD_PAL_START << 4], sprTitleLogoPal, sizeof(sprTitleLogoPal) >> 2);

	paletteData[0].size = 16;
	paletteData[0].position = pal_bg_mem;
	paletteData[0].buffer = paletteBufferBg;
	
	memcpy32(&paletteBufferObj[SHOOTING_STAR_PAL_START << 4], shootingStarPal, sizeof(shootingStarPal) >> 2);
	memcpy32(&paletteBufferObj[STAR_BLOCKER_PAL_START << 4], starBlockerPal, sizeof(starBlockerPal) >> 2);
	
	paletteData[1].size = 16;
	paletteData[1].position = pal_obj_mem;
	paletteData[1].buffer = (void *)paletteBufferObj;
	
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
	memcpy32(&characterBuffer5[0], starBlockerTiles, sizeof(starBlockerTiles) >> 2);
	characterData[4].position = tile_mem[SHOOTING_STAR_CHARDATA];
	characterData[4].buffer = (void *)characterBuffer4;
	characterData[4].size = sizeof(characterBuffer4) >> 2;
	characterData[5].position = tile_mem[STAR_BOCKER_CHARDATA];
	characterData[5].buffer = (void *)characterBuffer5;
	characterData[5].size = sizeof(characterBuffer5) >> 2;
	
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
			objectBuffer[STAR_BLOCKER_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(9);
			objectBuffer[STAR_BLOCKER_SPRITE].attr1 = ATTR1_SIZE_32 | ATTR1_X(185);
			objectBuffer[STAR_BLOCKER_SPRITE].attr2 = ATTR2_ID(STAR_BLOCKER_GFX_START) | ATTR2_PRIO(3) | ATTR2_PALBANK(1);
			
			oam_mem[STAR_BLOCKER_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(9);
			oam_mem[STAR_BLOCKER_SPRITE].attr1 = ATTR1_SIZE_32 | ATTR1_X(185);
			oam_mem[STAR_BLOCKER_SPRITE].attr2 = ATTR2_ID(STAR_BLOCKER_GFX_START) | ATTR2_PRIO(3) | ATTR2_PALBANK(1);
			
			mainMenuData.actionTarget = 16;
			mainMenuData.actionTimer = 0;
			mainMenuData.state = FADE_TO_TITLE;
			IOBuffer[0] = 16;
		}
		else{
			mainMenuData.actionTimer++;
			IOBuffer[0] = mainMenuData.actionTimer >> 1;
		}
		IOData.position = (void *)&REG_BLDY;
		IOData.buffer = IOBuffer;
		IOData.size = 1;
		break;
	case FADE_TO_TITLE:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_COMET_ANIMATION;
			mainMenuData.actionTarget = 64;
			mainMenuData.actionTimer = 0;
			IOBuffer[0] = 0;
		}
		else{
			mainMenuData.actionTimer++;
			IOBuffer[0] = 16 - mainMenuData.actionTimer;
		}
		IOData.position = (void *)&REG_BLDY;
		IOData.buffer = IOBuffer;
		IOData.size = 1;
		break;
	case TITLE_COMET_ANIMATION:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_HOLD;
			objectBuffer[SHOOTING_STAR_SPRITE].attr0 = ATTR0_HIDE;
			objectBuffer[STAR_BLOCKER_SPRITE].attr0 = ATTR0_HIDE;
			mainMenuData.actionTimer = 0;
			
			memcpy32(&paletteBufferObj[PRESS_START_PAL_START << 4], sprTitlePressStartTextPal, sizeof(sprTitlePressStartTextPal) >> 2);
			paletteData[1].size = 16;
			paletteData[1].position = pal_obj_mem;
			paletteData[1].buffer = (void *)paletteBufferObj;
			
			memcpy32(&characterBuffer4[0], sprTitlePressStartTextTiles, sizeof(sprTitlePressStartTextTiles) >> 2);
			characterData[4].position = tile_mem[PRESS_START_CHARDATA];
			characterData[4].buffer = (void *)characterBuffer4;
			characterData[4].size = sizeof(characterBuffer4) >> 2;
		}
		else{
			u8 starFrame = mainMenuData.actionTimer >> 2;
			objectBuffer[SHOOTING_STAR_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(shootingStarYPos[starFrame]);
			objectBuffer[SHOOTING_STAR_SPRITE].attr1 = ATTR1_SIZE_64 | ATTR1_X(shootingStarXPos[starFrame]);
			objectBuffer[SHOOTING_STAR_SPRITE].attr2 = ATTR2_ID((starFrame << 5) + SHOOTING_STAR_GFX_START) | ATTR2_PRIO(3) | ATTR2_PALBANK(0);
			mainMenuData.actionTimer++;
		}
		OAMData.position = (void *)oam_mem;
		OAMData.buffer = objectBuffer;
		OAMData.size = sizeof(objectBuffer) >> 2;
		break;
	case TITLE_HOLD:
		if((inputs.pressed & KEY_A) || (inputs.pressed & KEY_START)){
			currentScene.scenePointer = sceneList[GAMEPLAY];
			currentScene.state = INITIALIZE;
			//hide "press start"
			objectBuffer[PRESS_START_SPRITE1].attr0 = ATTR0_HIDE;
			objectBuffer[PRESS_START_SPRITE2].attr0 = ATTR0_HIDE;
			objectBuffer[PRESS_START_SPRITE3].attr0 = ATTR0_HIDE;
		}
		if(mainMenuData.actionTimer % 128 >= 64){
			//display "press start"
			objectBuffer[PRESS_START_SPRITE1].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(121);
			objectBuffer[PRESS_START_SPRITE1].attr1 = ATTR1_SIZE_32x8 | ATTR1_X(78);
			objectBuffer[PRESS_START_SPRITE1].attr2 = ATTR2_ID(PRESS_START_GFX_START) | ATTR2_PRIO(0) | ATTR2_PALBANK(PRESS_START_PAL_START);
			objectBuffer[PRESS_START_SPRITE2].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(121);
			objectBuffer[PRESS_START_SPRITE2].attr1 = ATTR1_SIZE_32x8 | ATTR1_X(110);
			objectBuffer[PRESS_START_SPRITE2].attr2 = ATTR2_ID(PRESS_START_GFX_START + 4) | ATTR2_PRIO(0) | ATTR2_PALBANK(PRESS_START_PAL_START);
			objectBuffer[PRESS_START_SPRITE3].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(121);
			objectBuffer[PRESS_START_SPRITE3].attr1 = ATTR1_SIZE_32x8 | ATTR1_X(142);
			objectBuffer[PRESS_START_SPRITE3].attr2 = ATTR2_ID(PRESS_START_GFX_START + 8) | ATTR2_PRIO(0) | ATTR2_PALBANK(PRESS_START_PAL_START);
		}
		else{
			//hide "press start"
			objectBuffer[PRESS_START_SPRITE1].attr0 = ATTR0_HIDE;
			objectBuffer[PRESS_START_SPRITE2].attr0 = ATTR0_HIDE;
			objectBuffer[PRESS_START_SPRITE3].attr0 = ATTR0_HIDE;
		}
		
		mainMenuData.actionTimer++;
		
		OAMData.position = (void *)oam_mem;
		OAMData.buffer = objectBuffer;
		OAMData.size = sizeof(objectBuffer) >> 2;
		break;
	default:
		break;
	}
	
}

void mainMenuEnd(){

}

void processCameraMainMenu(){

}