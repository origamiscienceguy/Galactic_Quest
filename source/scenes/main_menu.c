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

int dataRange[] = {0, 100};

// Initialize the Menu Pages
MenuPage menuPages[6] = {
	{
		.items = {
			{"Play Game", PAGE_TRANSFER, .data.intVal = (int)MPI_PLAY_GAME, .dataType = INT},
			{"Extras", PAGE_TRANSFER, .data.intVal = (int)MPI_EXTRAS, .dataType = INT},
			{"Options", PAGE_TRANSFER, .data.intVal = (int)MPI_OPTIONS, .dataType = INT}
		},
		.itemCount = 3,
		.pageName = "MAIN MENU"
	},
	{
		.items = {
			{"New Game", SCRIPT_RUNNER, .data.functionPtr = menuExecNewGame, .dataType = FUNC_PTR},
			{"Continue", SCRIPT_RUNNER, .data.functionPtr = menuExecContinue, .dataType = FUNC_PTR},
			{"Load Game", SCRIPT_RUNNER, .data.functionPtr = menuExecLoadGame, .dataType = FUNC_PTR},
			{"Back", PAGE_TRANSFER, .data.intVal = (int)MPI_MAIN_MENU, .dataType = INT}
		},
		.itemCount = 4,
		.pageName = "PLAY GAME"
	},{
		.items = {
			{"Sound Test", PAGE_TRANSFER, .data.intVal = (int)MPI_SOUND_TEST, .dataType = INT},
			{"Credits", PAGE_TRANSFER, .data.intVal = (int)MPI_CREDITS, .dataType = INT},
			{"Back", PAGE_TRANSFER, .data.intVal = (int)MPI_MAIN_MENU, .dataType = INT}
		},
		.itemCount = 3,
		.pageName = "EXTRAS"
	},
	{
		.items = {
			{"Master Volume", SLIDER, .data.intArray = dataRange, .dataType = INT_ARRAY},
			{"BGM", SLIDER, .data.intArray = dataRange, .dataType = INT_ARRAY},
			{"SFX", SLIDER, .data.intArray = dataRange, .dataType = INT_ARRAY},
			{"Apply Changes", SCRIPT_RUNNER, .data.functionPtr = menuExecOptionsApplyChanges, .dataType = FUNC_PTR},
			{"Abort", PAGE_TRANSFER, .data.intVal = (int)MPI_MAIN_MENU, .dataType = INT}
		},
		.itemCount = 5,
		.pageName = "OPTIONS"
	},
	{
		.items = {
			{"Back", PAGE_TRANSFER, .data.intVal = (int)MPI_EXTRAS, .dataType = INT}
		},
		.itemCount = 1,
		.pageName = "CREDITS"
	},
	{
		.items = {
			{"BGM", SOUND_TESTER, .data.functionPtr = menuExecPlayBGM, .dataType = FUNC_PTR},
			{"SFX", SOUND_TESTER, .data.functionPtr = menuExecPlaySFX, .dataType = FUNC_PTR},
			{"Back", PAGE_TRANSFER, .data.intVal = (int)MPI_EXTRAS, .dataType = INT}
		},
		.itemCount = 3,
		.pageName = "SOUND TEST"
	}
};

void mainMenuInitialize(){
	REG_DISPCNT = DCNT_MODE0; //black screen
	REG_BG0CNT = BG_4BPP | BG_SBB(STARRY_IMAGE_TILEMAP) | BG_CBB(STARRY_IMAGE_CHARDATA) | BG_PRIO(3) | BG_REG_64x64; //starry background layer
	REG_BG1CNT = BG_4BPP | BG_SBB(TITLE_CARD_TILEMAP) | BG_CBB(TITLE_CARD_CHARDATA) | BG_PRIO(2) | BG_REG_32x32; //title screen layer
	REG_BLDCNT = BLD_TOP(BLD_BG2 | BLD_BACKDROP | BLD_OBJ) | BLD_WHITE;
	REG_BLDY = BLDY(0);
	mainMenuData.starryBG.xPos = 512 - 16;
	REG_BG0HOFS = mainMenuData.starryBG.xPos;
	mainMenuData.starryBG.yPos = 104;
	REG_BG0VOFS = mainMenuData.starryBG.yPos;
	REG_BG1HOFS = 512 - 15;
	REG_BG1VOFS = 512 - 43;
	
	//send the palettes
	memcpy32(&paletteBufferBg[STARRY_IMAGE_PAL_START << 4], main_menu_starfieldPal, sizeof(main_menu_starfieldPal) >> 2);
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
	memcpy32(&characterBuffer0[STARRY_IMAGE_GFX_START << 5], main_menu_starfieldTiles, sizeof(main_menu_starfieldTiles) >> 2);
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
	tilemapData[0].buffer = (void *)main_menu_starfieldMetaTiles;
	tilemapData[0].size = sizeof(main_menu_starfieldMetaTiles) >> 2;
	
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
			IOBuffer0[0] = 16;
		}
		else{
			mainMenuData.actionTimer++;
			IOBuffer0[0] = mainMenuData.actionTimer >> 1;
		}
		IOData[0].position = (void *)&REG_BLDY;
		IOData[0].buffer = IOBuffer0;
		IOData[0].size = 1;
		break;
	case FADE_TO_TITLE:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_COMET_ANIMATION;
			mainMenuData.actionTarget = 64;
			mainMenuData.actionTimer = 0;
			IOBuffer0[0] = 0;
		}
		else{
			mainMenuData.actionTimer++;
			IOBuffer0[0] = 16 - mainMenuData.actionTimer;
		}
		IOData[0].position = (void *)&REG_BLDY;
		IOData[0].buffer = IOBuffer0;
		IOData[0].size = 1;
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
			mainMenuData.state = TITLE_FLY_OUT;
			mainMenuData.actionTimer = 0;
			mainMenuData.actionTarget = 32;
			//hide "press start"
			objectBuffer[PRESS_START_SPRITE1].attr0 = ATTR0_HIDE;
			objectBuffer[PRESS_START_SPRITE2].attr0 = ATTR0_HIDE;
			objectBuffer[PRESS_START_SPRITE3].attr0 = ATTR0_HIDE;
		}
		else{
			mainMenuData.actionTimer++;
		}
		
		if(mainMenuData.actionTimer % 128 < 64){
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
		
		
		if(currentScene.sceneCounter % 8 <= 0){
			mainMenuData.starryBG.xPos++;		
		}
		if(currentScene.sceneCounter % 8 <= 0){
			mainMenuData.starryBG.yPos++;
		}
		IOBuffer0[0] = mainMenuData.starryBG.xPos;
		IOBuffer0[1] = mainMenuData.starryBG.yPos;
		
		OAMData.position = (void *)oam_mem;
		OAMData.buffer = objectBuffer;
		OAMData.size = sizeof(objectBuffer) >> 2;
		
		IOData[0].position = (void *)(&REG_BG0HOFS);
		IOData[0].buffer = IOBuffer0;
		IOData[0].size = 1;
		
		break;
		
	case TITLE_FLY_OUT:
		
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = MAIN_MENU_FLY_IN;
		}
		else{
			mainMenuData.actionTimer++;
		}
		
		if(mainMenuData.actionTimer % 16 >= 8){
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
		
		if(currentScene.sceneCounter % 8 <= 0){
			mainMenuData.starryBG.xPos++;		
		}
		if(currentScene.sceneCounter % 8 <= 0){
			mainMenuData.starryBG.yPos++;
		}
		IOBuffer0[0] = mainMenuData.starryBG.xPos;
		IOBuffer0[1] = mainMenuData.starryBG.yPos;
		IOBuffer0[2] = 512 - 15;
		IOBuffer0[3] = titleFlyOutYLUT[mainMenuData.actionTimer];
		
		OAMData.position = (void *)oam_mem;
		OAMData.buffer = objectBuffer;
		OAMData.size = sizeof(objectBuffer) >> 2;
		
		IOData[0].position = (void *)(&REG_BG0HOFS);
		IOData[0].buffer = IOBuffer0;
		IOData[0].size = 2;
		break;
		
	case MAIN_MENU_FLY_IN:
		// Example array of MenuPageItem

		/*
		MenuPage menuPage = {
			.items = {
				{"Function Item", SCRIPT_RUNNER, .data.functionPtr = exampleFunction, .dataType = FUNC_PTR},
				{"Integer Item", PAGE_TRANSFER, .data.intValue = 123, .dataType = INT},
				{"Integer Array Item", SLIDER, .data.intArray = dataRange, .dataType = INT_ARRAY}
			},
			.itemCount = 3
		};*/

		// Print the items in the MenuPage
		printMenuPage(&menuPages[MPI_MAIN_MENU]);

		// Set the Menu State to "MAIN_MENU_HOLD and re-init its timers"
		mainMenuData.actionTarget = 32;
		mainMenuData.actionTimer = 0;
		mainMenuData.state = MAIN_MENU_HOLD;

		// Make the starry background scroll up-left
		scrollStarryBG();
		break;
	case MAIN_MENU_HOLD:
		drawNineSliceWindow(0, 0, 15, 15);
		
		if((inputs.pressed & KEY_A) || (inputs.pressed & KEY_START)){
			menuExecNewGame();
		}
		
		// Make the starry background scroll up-left
		scrollStarryBG();
		break;
	case MAIN_MENU_FLY_OUT:
		if(mainMenuData.actionTimer >= mainMenuData.actionTarget){
			// Start the match
			mainMenuEnd();
		}
		else{
			mainMenuData.actionTimer++;
		}

		// Make the starry background scroll up-left
		scrollStarryBG();
		break;
	default:
		break;
	}
	
}

void mainMenuEnd(){
	currentScene.scenePointer = sceneList[GAMEPLAY];
	currentScene.state = INITIALIZE;
}

void scrollStarryBG(){
	if(currentScene.sceneCounter % 8 <= 0){
		mainMenuData.starryBG.xPos++;		
	}
	if(currentScene.sceneCounter % 8 <= 0){
		mainMenuData.starryBG.yPos++;
	}
	IOBuffer0[0] = mainMenuData.starryBG.xPos;
	IOBuffer0[1] = mainMenuData.starryBG.yPos;
	
	IOData[0].position = (void *)(&REG_BG0HOFS);
	IOData[0].buffer = IOBuffer0;
	IOData[0].size = 1;
}

void drawTile(int x, int y, int tileIndex, bool flipHorizontal, bool flipVertical) {
    /// TODO: Implement the actual tile drawing logic here
	// Tileset is Galactic_Quest\resources\graphics\tsMenuUI.bmp
    printf("Drawing tile %d at (%d, %d) with horizontal flip %d and vertical flip %d\n",
           tileIndex, x, y, flipHorizontal, flipVertical);
}

int snapToGrid(int value) {
    return (value / 8) * 8;
}

/// @brief Draws the nine slice window; Width and Height params are in terms of 8x8 tiles
/// @param width 
/// @param height 
void drawNineSliceWindow(int x, int y, int width, int height) {
    // Snap (x, y) to nearest 8x8 grid values
    x = snapToGrid(x);
    y = snapToGrid(y);

    // Dimensions of the nine slices
    const int TILE_SIZE = 8;

    // Draw the top row
    if (width >= 2) {
        drawTile(x, y, TL_1, false, false); // Top-left corner Part 1
        drawTile(x + TILE_SIZE, y, TL_2, false, false); // Top-left corner Part 2
        drawTile(x + 2 * TILE_SIZE, y, TL_3, false, false); // Top-left corner Part 3
        for (int i = 3; i < width - 3; ++i) {
            drawTile(x + i * TILE_SIZE, y, TOP_MIDDLE, false, false); // Top middle
        }
        if (width >= 3) {
            drawTile(x + (width - 3) * TILE_SIZE, y, TR_1, false, false); // Top-right corner Part 1
        }
        if (width >= 2) {
            drawTile(x + (width - 2) * TILE_SIZE, y, TR_2, false, false); // Top-right corner Part 2
        }
        if (width >= 3) {
            drawTile(x + (width - 1) * TILE_SIZE, y, TR_3, false, false); // Top-right corner Part 3
        }
    }

    // Calculate the bottomY position
    int bottomY = y + (height - 1) * TILE_SIZE;

    // Draw center rows
    for (int j = 2; j < height - 1; ++j) {
        drawTile(x, y + j * TILE_SIZE, LM, false, false); // Left-middle tile
        for (int i = 1; i < width - 1; ++i) {
            drawTile(x + i * TILE_SIZE, y + j * TILE_SIZE, CENTER, false, false); // Center
        }
        drawTile(x + (width - 1) * TILE_SIZE, y + j * TILE_SIZE, RM, false, false); // Right-middle tile
    }

    // Draw the bottom-middle row (mirrored)
    if (height > 4) {
        drawTile(x, bottomY - TILE_SIZE, LM_UPPER, false, true); // Left-middle-lower tile (flipped vertically)
        for (int i = 1; i < width - 1; ++i) {
            drawTile(x + i * TILE_SIZE, bottomY - TILE_SIZE, MIDDLE_UPPER, false, true); // Middle lower (flipped vertically)
        }
        drawTile(x + (width - 1) * TILE_SIZE, bottomY - TILE_SIZE, RM_UPPER, false, true); // Right-middle-lower tile (flipped vertically)
    }

    // Draw the bottom row
    if (width > 2) {
        drawTile(x, bottomY, TL_1, false, true); // Bottom-left corner Part 1 (flipped vertically)
    }
    if (width > 3) {
        drawTile(x + TILE_SIZE, bottomY, TL_2, false, true); // Bottom-left corner Part 2 (flipped vertically)
    }
    if (width > 4) {
        drawTile(x + 2 * TILE_SIZE, bottomY, TL_3, false, true); // Bottom-left corner Part 3 (flipped vertically)
    }
    for (int i = 3; i < width - 3; ++i) {
        drawTile(x + i * TILE_SIZE, bottomY, TOP_MIDDLE, false, true); // Bottom middle (flipped vertically)
    }
    if (width >= 3) {
        drawTile(x + (width - 3) * TILE_SIZE, bottomY, TR_1, false, true); // Bottom-right corner Part 1 (flipped vertically)
    }
    if (width >= 2) {
        drawTile(x + (width - 2) * TILE_SIZE, bottomY, TR_2, false, true); // Bottom-right corner Part 2 (flipped vertically)
    }
    if (width >= 1) {
        drawTile(x + (width - 1) * TILE_SIZE, bottomY, TR_3, false, true); // Bottom-right corner Part 3 (flipped vertically)
    }

    // Draw the top row (additional check to ensure no overlapping)
    if (width >= 2) {
        drawTile(x + TILE_SIZE, y, TL_2, false, false); // Top-left corner Part 2
    }
    if (width >= 3) {
        drawTile(x + 2 * TILE_SIZE, y, TL_3, false, false); // Top-left corner Part 3
    }
    for (int i = 3; i < width - 3; ++i) {
        drawTile(x + i * TILE_SIZE, y, TOP_MIDDLE, false, false); // Top middle
    }
    if (width >= 3) {
        drawTile(x + (width - 3) * TILE_SIZE, y, TR_1, false, false); // Top-right corner Part 1
    }
    if (width >= 2) {
        drawTile(x + (width - 2) * TILE_SIZE, y, TR_2, false, false); // Top-right corner Part 2
    }
    if (width >= 1) {
        drawTile(x + (width - 1) * TILE_SIZE, y, TR_3, false, false); // Top-right corner Part 3
    }
}

int menuExecNewGame(){
	mainMenuData.state = MAIN_MENU_FLY_OUT;
	mainMenuData.actionTimer = 0;
	mainMenuData.actionTarget = 40; // Specifies the length of the actionTimer
	return 0;
}

int menuExecContinue(){
	return 0;
}

int menuExecLoadGame(){
	return 0;
}

int menuExecOptionsApplyChanges(){
	return 0;
}

int menuExecPlayBGM(){
	return 0;
}

int menuExecPlaySFX(){
	return 0;
}


void printMenuPageItem(const MenuPageItem* item){
    //printf("String: %s\n", item->itemName);
    //printf("Enum: %d\n", item->dataType);
    
    switch (item->dataType){
        case FUNC_PTR:
            //printf("Function return value: %d\n", item->data.functionPtr());
            break;
        case INT:
            //printf("Integer value: %d\n", item->data.intVal);
            break;
        case INT_ARRAY:
            //printf("Integer array: ");
            // Assuming the size of the array is known or can be tracked
            // This placeholder assumes 4 elements for demonstration
            for (size_t i = 0; i < 4; ++i){
                //printf("%d ", item->data.intArray[i]);
            }
            //printf("\n");
            break;
    }
}

void printMenuPage(const MenuPage* menuPage){
    for (size_t i = 0; i < menuPage->itemCount; ++i){
        printMenuPageItem(&menuPage->items[i]);
    }
}