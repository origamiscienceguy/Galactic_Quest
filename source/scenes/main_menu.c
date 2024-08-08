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
	mainMenuData.starryBG.yPos = TITLE_CAM_PAN_BOTTOM;
	mainMenuData.titleCardBG.xPos = 512 - 15;
	mainMenuData.titleCardBG.yPos = 512 - 43;
	mainMenuData.menuBG.xPos = 0;
	mainMenuData.menuBG.yPos = 0;
	REG_BG0VOFS = mainMenuData.starryBG.yPos;
	REG_BG1HOFS = mainMenuData.titleCardBG.xPos;
	REG_BG1VOFS = mainMenuData.titleCardBG.yPos;
	
	//send the palettes
	memcpy32(&paletteBufferBg[STARRY_IMAGE_PAL_START << 4], main_menu_starfieldPal, sizeof(main_menu_starfieldPal) >> 2);
	memcpy32(&paletteBufferBg[TITLE_CARD_PAL_START << 4], sprTitleLogoPal, sizeof(sprTitleLogoPal) >> 2);
	memcpy32(&paletteBufferBg[MENU_PAL_START << 4], tsMenuUIPal, sizeof(tsMenuUIPal) >> 2);

	paletteData[0].size = 24;
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
	memcpy32(&characterBuffer1[MENU_GFX_START << 5], tsMenuUITiles, sizeof(tsMenuUITiles) >> 2);
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
	
	//send the starry BG tilemap
	tilemapData[0].position = &se_mem[STARRY_IMAGE_TILEMAP];
	tilemapData[0].buffer = (void *)main_menu_starfieldMetaTiles;
	tilemapData[0].size = sizeof(main_menu_starfieldMetaTiles) >> 2;
	
	mainMenuData.actionTarget = 32;
	mainMenuData.actionTimer = 0;
	mainMenuData.state = FLASH_WHITE;
	currentScene.state = NORMAL;


	if (TITLE_DEBUG_MODE >= 1)
		skipToMenu();
}

void mainMenuNormal(){
	static u8 currentAssetIndex = 0;
	static u8 currentAsset = 0;
	extern u16 numAssets;
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
			mainMenuData.state = TITLE_WAIT_AT_BOTTOM;
			mainMenuData.actionTarget = 2;
			mainMenuData.actionTimer = 0;
			//playNewAsset(BGM_ID_TITLE);
			IOBuffer0[0] = 0;
		}
		else{
			mainMenuData.actionTimer++;
			IOBuffer0[0] = 16 - mainMenuData.actionTimer;
		}
		IOData[0].position = (void *)&REG_BLDY;
		IOData[0].buffer = IOBuffer0;
		IOData[0].size = 1;
		objectBuffer[SHOOTING_STAR_SPRITE].attr0 = ATTR0_HIDE;
		break;
	case TITLE_WAIT_AT_BOTTOM:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_PAN_UP;
			mainMenuData.actionTarget = 40;
			mainMenuData.actionTimer = 0;
		}
		else{
			mainMenuData.actionTimer++;
		}
		objectBuffer[SHOOTING_STAR_SPRITE].attr0 = ATTR0_HIDE;
		break;
	case TITLE_PAN_UP:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_FLASH;
			mainMenuData.actionTarget = 30;
			mainMenuData.actionTimer = 0;
			IOBuffer0[0] = 0;
		}
		else{
			int yStart = TITLE_CAM_PAN_BOTTOM * FIXED_POINT_SCALE; // Start position (scaled)
			int yTarget = 104 * FIXED_POINT_SCALE; // Target position (scaled)

			// Calculate the interpolation factor t, with proper scaling
			int actionTimerScaled = mainMenuData.actionTimer * FIXED_POINT_SCALE;
			int actionTargetScaled = mainMenuData.actionTarget * FIXED_POINT_SCALE;
			int t = (actionTimerScaled * FIXED_POINT_SCALE) / actionTargetScaled;

			// Apply ease-in-out function
			int easedT = easeInOut(t, 4);

			// Calculate the interpolated position and update yPos
			mainMenuData.starryBG.yPos = lerp(yStart, yTarget, easedT) / FIXED_POINT_SCALE;

			// Increment actionTimer
			mainMenuData.actionTimer++;
		}

		// Update the Starry BG Position
		IOBuffer1[0] = mainMenuData.starryBG.xPos;
		IOBuffer1[1] = mainMenuData.starryBG.yPos;
		IOData[1].position = (void *)(&REG_BG0HOFS);
		IOData[1].buffer = IOBuffer1;
		IOData[1].size = 1;

		objectBuffer[SHOOTING_STAR_SPRITE].attr0 = ATTR0_HIDE;
		IOData[0].position = (void *)&REG_BLDY;
		IOData[0].buffer = IOBuffer0;
		IOData[0].size = 1;
		break;
	case TITLE_FLASH:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_REVEAL;
			mainMenuData.actionTarget = 64;
			mainMenuData.actionTimer = 0;
			IOBuffer0[0] = 16;
			// Send the Title BG tilemap
			tilemapData[1].position = &se_mem[TITLE_CARD_TILEMAP];
			tilemapData[1].buffer = (void *)sprTitleLogoMap;
			tilemapData[1].size = sizeof(sprTitleLogoMap) >> 2;
		}
		else{
			mainMenuData.actionTimer++;
			IOBuffer0[0] = (mainMenuData.actionTimer * 2) >> 1;
		}
		IOData[0].position = (void *)&REG_BLDY;
		IOData[0].buffer = IOBuffer0;
		IOData[0].size = 1;
		objectBuffer[SHOOTING_STAR_SPRITE].attr0 = ATTR0_HIDE;
		break;
	case TITLE_REVEAL:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_COMET_ANIMATION;
			mainMenuData.actionTarget = 64;
			mainMenuData.actionTimer = 0;
			IOBuffer0[0] = 0;
		}
		else{
			mainMenuData.actionTimer++;

			if (IOBuffer0[0] > 0)
				IOBuffer0[0] = 16 - mainMenuData.actionTimer * 2;
			else
				IOBuffer0[0] = 0;
		}
		IOData[0].position = (void *)&REG_BLDY;
		IOData[0].buffer = IOBuffer0;
		IOData[0].size = 1;
		objectBuffer[SHOOTING_STAR_SPRITE].attr0 = ATTR0_HIDE;
		break;
	case TITLE_COMET_ANIMATION:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_BEFORE_HOLD;
			objectBuffer[SHOOTING_STAR_SPRITE].attr0 = ATTR0_HIDE;
			objectBuffer[STAR_BLOCKER_SPRITE].attr0 = ATTR0_HIDE;
			mainMenuData.actionTimer = 0;
			mainMenuData.actionTarget = 220;
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
	case TITLE_BEFORE_HOLD:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_HOLD;
			mainMenuData.actionTimer = 0;
			mainMenuData.actionTarget = 0;
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
			mainMenuData.actionTimer++;
		}
		OAMData.position = (void *)oam_mem;
		OAMData.buffer = objectBuffer;
		OAMData.size = sizeof(objectBuffer) >> 2;
		break;
	case TITLE_HOLD:
		if((inputs.pressed & KEY_A) || (inputs.pressed & KEY_START)){
			skipToMenu();
			yStart = mainMenuData.starryBG.yPos * FIXED_POINT_SCALE; // Start position (scaled)
			yTarget = -4504 * FIXED_POINT_SCALE; // Target position (scaled)
			titleCardYStart = mainMenuData.titleCardBG.yPos * FIXED_POINT_SCALE;
			titleCardYTarget = (mainMenuData.titleCardBG.yPos + 140) * FIXED_POINT_SCALE;
		}
		else{
			mainMenuData.actionTimer++;
		}
		
		if(mainMenuData.actionTimer % 128 < 64){
			displayPressStart();
		}
		else{
			hidePressStart();
		}
		
		// Make the starry background scroll up-left
		scrollStarryBG(-1, -1);

		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.titleCardBG.xPos, mainMenuData.titleCardBG.yPos);
		
		OAMData.position = (void *)oam_mem;
		OAMData.buffer = objectBuffer;
		OAMData.size = sizeof(objectBuffer) >> 2;
		
		IOData[0].position = (void *)(&REG_BG0HOFS);
		IOData[0].buffer = IOBuffer0;
		IOData[0].size = 1;
		break;
	case TITLE_FLY_OUT:
		
		if(mainMenuData.actionTimer >= mainMenuData.actionTarget){
			mainMenuData.state = MAIN_MENU_FLY_IN;
			
			//endAsset(currentAssetIndex);
			//currentAssetIndex = playNewAsset(BGM_ID_MAIN_MENU);
			mainMenuData.menuBG.xPos = 512 - 2;
			mainMenuData.menuBG.yPos = 0;
			loadGFX(MENU_CHARDATA, MENU_TEXT_GFX_START, (void *)menu_actionTiles, MENU_TEXT_TILE_WIDTH * 6, MENU_TEXT_TILE_WIDTH * 8, 0);
			loadGFX(MENU_CHARDATA, MENU_TEXT_FOCUSED_GFX_START, (void *)menu_action_focusedTiles, MENU_TEXT_TILE_WIDTH * 6, MENU_TEXT_TILE_WIDTH * 8, 1);
		}else{
			// Make the starry background scroll up, *very* quickly. Use quadratic interpolation

			// Calculate the interpolation factor t, with proper scaling
			int actionTimerScaled = mainMenuData.actionTimer * FIXED_POINT_SCALE;
			int actionTargetScaled = mainMenuData.actionTarget * FIXED_POINT_SCALE;
			int t = (actionTimerScaled * FIXED_POINT_SCALE) / actionTargetScaled;
			int t2 = t * 8;

			// Apply ease-in-out function
			int easedT = easeOutQuint(t);
			int easedT2 = easeInOut(t2, 4);

			// Calculate the interpolated position and update yPos
			mainMenuData.starryBG.yPos = lerp(yStart, yTarget, easedT) / FIXED_POINT_SCALE;
			mainMenuData.titleCardBG.yPos = lerp(titleCardYStart, titleCardYTarget, easedT2) / FIXED_POINT_SCALE;
			mainMenuData.actionTimer++;
		}
		
		if(mainMenuData.actionTimer % 16 >= 8 && mainMenuData.actionTimer < 80){
			displayPressStart();
		}
		else{
			hidePressStart();
		}
		
		// Sprite positioning
		OAMData.position = (void *)oam_mem;
		OAMData.buffer = objectBuffer;
		OAMData.size = sizeof(objectBuffer) >> 2;
		
		if (mainMenuData.actionTimer > 40) {
			// Hide the title card after 40 frames in this state
			memset32(tilemapBuffer1, 0, sizeof(sprTitleLogoMap) >> 2);
			tilemapData[1].size = sizeof(sprTitleLogoMap) >> 2;
			tilemapData[1].buffer = tilemapBuffer1;
			tilemapData[1].position = &se_mem[TITLE_CARD_TILEMAP];
		}

		// Queue BG Scroll registers for the Starry BG and Menu Positions
		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.titleCardBG.xPos, mainMenuData.titleCardBG.yPos);
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
		

		
		// Make the starry background scroll left
		scrollStarryBG(-1, 0);

		// Queue BG Scroll registers for the Starry BG and Menu Positions
		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.menuBG.xPos, mainMenuData.menuBG.yPos);
		break;
	case MAIN_MENU_HOLD:
		if((inputs.pressed & KEY_A) || (inputs.pressed & KEY_START)){

		}

		tilemapData[1].position = &se_mem[MENU_TILEMAP];
		tilemapData[1].buffer = (void *)tilemapBuffer1;
		tilemapData[1].size = 512;

		int winSliceWidth = 10;
		drawNineSliceWindow(10, 6, winSliceWidth, 10, 1);
		
		drawMenuTextSegment(winSliceWidth, 10, 8, 0, 2, false);
		drawMenuTextSegment(winSliceWidth, 10, 10, 1, 2, true);
		drawMenuTextSegment(winSliceWidth, 10, 12, 2, 2, false);
		
		/*	
		tilemapData[2].position = &se_mem[MENU_TILEMAP];
		tilemapData[2].buffer = (void *)tilemapBuffer2;
		tilemapData[2].size = 512;

		drawNineSliceWindow(10, 6, 9, 9, 2);
		*/

		// Make the starry background scroll up-left
		scrollStarryBG(-1, 0);

		// Queue BG Scroll registers for the Starry BG and Menu Positions
		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.menuBG.xPos, mainMenuData.menuBG.yPos);
		break;
	case MAIN_MENU_FLY_OUT:
		if(mainMenuData.actionTimer >= mainMenuData.actionTarget){
			// Start the match
			mainMenuEnd();
		}
		else{
			mainMenuData.actionTimer++;
		}
		
		/*
		tilemapData[1].position = &se_mem[MENU_TILEMAP];
		tilemapData[1].buffer = (void *)tilemapBuffer1;
		tilemapData[1].size = 512;
		
		tilemapData[2].position = &se_mem[MENU_TILEMAP];
		tilemapData[2].buffer = (void *)tilemapBuffer2;
		tilemapData[2].size = 512;*/

		// Make the starry background scroll up-left
		scrollStarryBG(-1, -1);

		// Queue BG Scroll registers for the Starry BG and Menu Positions
		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.menuBG.xPos, mainMenuData.menuBG.yPos);
		break;
	default:
		break;
	}
	
}

void mainMenuEnd(){
	currentScene.scenePointer = sceneList[GAMEPLAY];
	currentScene.state = INITIALIZE;
}

void scrollStarryBG(int addedX, int addedY){
	if(currentScene.sceneCounter % 8 <= 0){
		mainMenuData.starryBG.xPos -= addedX;		
	}
	if(currentScene.sceneCounter % 8 <= 0){
		mainMenuData.starryBG.yPos -= addedY;
	}
}

void setTile(int x, int y, int drawingTileIndex, bool flipHorizontal, bool flipVertical, int palette, int layer){
    u16* tilemapBuffer;

    switch(layer) {
        default:
            tilemapBuffer = tilemapBuffer0;
            break;
        case 1:
            tilemapBuffer = tilemapBuffer1;
            break;
        case 2:
            tilemapBuffer = tilemapBuffer2;
            break;
        case 3:
            tilemapBuffer = tilemapBuffer3;
            break;
    }

    // Calculate the index and set the tile value
    int tilemapIndex = x + (y << 5);
    tilemapBuffer[tilemapIndex] = SE_BUILD(drawingTileIndex, palette, flipHorizontal, flipVertical);
}

/// @brief Draws a nine slice window for the centric main menu; Width and Height params are in terms of 8x8 tiles
/// @param width 
/// @param height 
void drawNineSliceWindow(int x, int y, int width, int height, int layer){
    int tilesetIndex = MENU_GFX_START;
    int palette = 2;

    if (height > 1){
        // Draw the top-middle row
        if (height >= 2){
            setTile(x, y + 1, tilesetIndex + LM_UPPER, false, false, palette, layer); // Left-middle-upper tile
            for (int i = 1; i < width - 1; ++i){
                setTile(x + i, y + 1, tilesetIndex + MIDDLE_UPPER, false, false, palette, layer); // Middle upper
            }
            setTile(x + (width - 1), y + 1, tilesetIndex + RM_UPPER, false, false, palette, layer); // Right-middle-upper tile
        }

        // Calculate the bottomY position
        int bottomY = y + (height - 1);

        // Draw center rows
        for (int j = 2; j < height - 1; ++j){
            setTile(x, y + j, tilesetIndex + LM, false, false, palette, layer); // Left-middle tile
            for (int i = 1; i < width - 1; ++i){
                setTile(x + i, y + j, tilesetIndex + CENTER, false, false, palette, layer); // Center
            }
            setTile(x + (width - 1), y + j, tilesetIndex + RM, false, false, palette, layer); // Right-middle tile
        }

        // Draw the bottom-middle row (mirrored)
        if (height > 4){
            setTile(x, bottomY - 1, tilesetIndex + LM_UPPER, false, true, palette, layer); // Left-middle-lower tile (flipped vertically)
            for (int i = 1; i < width - 1; ++i){
                setTile(x + i, bottomY - 1, tilesetIndex + MIDDLE_UPPER, false, true, palette, layer); // Middle lower (flipped vertically)
            }
            setTile(x + (width - 1), bottomY - 1, tilesetIndex + RM_UPPER, false, true, palette, layer); // Right-middle-lower tile (flipped vertically)
        }

        // Draw the bottom row
        if (width > 2)
            setTile(x + 1, bottomY, tilesetIndex + TL_2, false, true, palette, layer); // Bottom-left corner Part 2 (flipped vertically)
        if (width > 3)
            setTile(x + 2, bottomY, tilesetIndex + TL_3, false, true, palette, layer); // Bottom-left corner Part 3 (flipped vertically)
        for (int i = 3; i < width - 3; ++i){
            setTile(x + i, bottomY, tilesetIndex + TOP_MIDDLE, false, true, palette, layer); // Bottom middle (flipped vertically)
        }
        if (width >= 3)
            setTile(x + (width - 3), bottomY, tilesetIndex + TR_1, false, true, palette, layer); // Bottom-right corner Part 1 (flipped vertically)
        if (width >= 2)
            setTile(x + (width - 2), bottomY, tilesetIndex + TR_2, false, true, palette, layer); // Bottom-right corner Part 2 (flipped vertically)

        // Draw the top row
        for (int i = 3; i < width - 3; ++i){
            setTile(x + i, y, tilesetIndex + TOP_MIDDLE, false, false, palette, layer); // Top middle
        }

        if (width >= 3)
            setTile(x + (width - 3), y, tilesetIndex + TR_1, false, false, palette, layer); // Top-right corner Part 1
        if (width >= 2)
            setTile(x + 1, y, tilesetIndex + TL_2, false, false, palette, layer); // Top-left corner Part 2
        if (width >= 3)
            setTile(x + 2, y, tilesetIndex + TL_3, false, false, palette, layer); // Top-left corner Part 3

        if (width >= 2)
            setTile(x + (width - 2), y, tilesetIndex + TR_2, false, false, palette, layer); // Top-right corner Part 2

        setTile(x + (width - 1), bottomY, tilesetIndex + TR_3, false, true, palette, layer); // Bottom-right corner Part 3 (flipped vertically)
        setTile(x, bottomY, tilesetIndex + TL_1, false, true, palette, layer); // Bottom-left corner Part 1 (flipped vertically)

        setTile(x + (width - 1), y, tilesetIndex + TR_3, false, false, palette, layer); // Top-right corner Part 3
        setTile(x, y, tilesetIndex + TL_1, false, false, palette, layer); // Top-left corner Part 1
    } else {
        for (int i = 0; i < width; ++i){
            setTile(x + i, y, tilesetIndex + LASER_TOP, false, false, palette, layer);
            setTile(x + i, y + 1, tilesetIndex + LASER_BOTTOM, false, false, palette, layer);
        }
    }
}

/// @brief Draws a nine slice window for the main menu's Menu Page window; Width and Height params are in terms of 8x8 tiles
/// @param width 
/// @param height 
void drawSecondaryNineSliceWindowStyle(int x, int y, int width, int height, int layer){
   	int tilesetIndex = MENU_GFX_START;
    int palette = 2;

    int middleWidth = width - 2;
    int middleHeight = height - 2;

    // Draw the top-left tile
    setTile(x, y, tilesetIndex + SEC_TOP_LEFT, false, false, palette, layer);

    // Draw the top-middle tiles
    for (int i = 0; i < middleWidth; i++){
        setTile(x + 1 + i, y, tilesetIndex + SEC_TOP_MIDDLE, false, false, palette, layer);
    }

    // Draw the top-right tile
    setTile(x + 1 + middleWidth, y, tilesetIndex + SEC_TOP_LEFT, true, false, palette, layer);

    // Draw the left tiles
    for (int i = 0; i < middleHeight; i++){
        setTile(x, y + 1 + i, tilesetIndex + SEC_LEFT, false, false, palette, layer);
    }

    // Draw the center tiles
    for (int yOffset = 0; yOffset < middleHeight; yOffset++){
        for (int xOffset = 0; xOffset < middleWidth; xOffset++){
            setTile(x + 1 + xOffset, y + 1 + yOffset, tilesetIndex + SEC_CENTER, false, false, palette, layer);
        }
    }

    // Draw the right tiles
    for (int i = 0; i < middleHeight; i++){
        setTile(x + 1 + middleWidth, y + 1 + i, tilesetIndex + SEC_LEFT, true, false, palette, layer);
    }

    // Draw the bottom-left tile
    setTile(x, y + 1 + middleHeight, tilesetIndex + SEC_TOP_LEFT, false, true, palette, layer);

    // Draw the bottom-middle tiles
    for (int i = 0; i < middleWidth; i++){
        setTile(x + 1 + i, y + 1 + middleHeight, tilesetIndex + SEC_TOP_MIDDLE, false, true, palette, layer);
    }

    // Draw the bottom-right tile
    setTile(x + 1 + middleWidth, y + 1 + middleHeight, tilesetIndex + SEC_TOP_LEFT, true, true, palette, layer);
}

void drawMenuTextSegment(int nineSliceWidth, int tileXPos, int tileYPos, int menuElementPosition, int palette, bool highlighted){
	int tilesetIndex;	
	if (!highlighted)
		tilesetIndex = MENU_TEXT_GFX_START;
	else
		tilesetIndex = MENU_TEXT_FOCUSED_GFX_START;
	
	for (int row = 0; row < 2; row++) {
		for (int tileXOff = 0; tileXOff < MENU_TEXT_TILE_WIDTH; tileXOff++) {
			int vramIndex = tilesetIndex + tileXOff + (menuElementPosition * MENU_TEXT_TILE_WIDTH * 2) + (row * MENU_TEXT_TILE_WIDTH);
			setTile(tileXPos + tileXOff, tileYPos + row, vramIndex, false, false, palette, MENU_TEXT_LAYER_ID);
		}
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

//example usage to load the portion of the image starting 6 tile rows down, and 8 tile rows deep.
//loadGFX(MENU_CHARDATA, MENU_TEXT_GFX_START, menu_actionTiles, MENU_TEXT_TILE_WIDTH * 6, MENU_TEXT_TILE_WIDTH * 8);

// Limit of 4 queue channels per frame
void loadGFX(u32 VRAMCharBlock, u32 VRAMTileIndex, void *graphicsBasePointer, u32 graphicsTileOffset, u32 numTilesToSend, u32 queueChannel){
	characterData[queueChannel].position = &tile_mem[VRAMCharBlock][VRAMTileIndex];
	characterData[queueChannel].buffer = &((u8 *)graphicsBasePointer)[graphicsTileOffset << 5];
	characterData[queueChannel].size = numTilesToSend << 3;
}

void updateBGScrollRegisters(u16 bg0XPos, u16 bg0YPos, u16 bg1XPos, u16 bg1YPos){
	// Send the buffer to get processed
	IOData[0].position = (void *)(&REG_BG0HOFS);
	IOData[0].buffer = IOBuffer0;
	IOData[0].size = 4;

	IOBuffer0[0] = bg0XPos;
	IOBuffer0[1] = bg0YPos;
	IOBuffer0[2] = bg1XPos;
	IOBuffer0[3] = bg1YPos;
	IOBuffer0[4] = 0;
	IOBuffer0[5] = 0;
	IOBuffer0[6] = 0;
	IOBuffer0[7] = 0;
}

int lerp(int a, int b, int t){
    return a + (t * (b - a) / FIXED_POINT_SCALE);
}

int easeInOut(int t, int factor){
    int halfFactor = factor / 2;
    if (t < FIXED_POINT_SCALE / 2){
        return (halfFactor * t * t) / FIXED_POINT_SCALE;
    } else {
        int temp = t - FIXED_POINT_SCALE;
        return (halfFactor * -temp * temp) / FIXED_POINT_SCALE + FIXED_POINT_SCALE;
    }
}

int easeOutQuint(int t) {
    // Convert t to a floating-point value in fixed-point arithmetic
    int tInverse = FIXED_POINT_SCALE - t;
    int tInverseSquared = (tInverse * tInverse) / FIXED_POINT_SCALE;
    int tInverseCubed = (tInverse * tInverseSquared) / FIXED_POINT_SCALE;
    int tInverseQuartic = (tInverse * tInverseCubed) / FIXED_POINT_SCALE;
    int tInverseQuintic = (tInverse * tInverseQuartic) / FIXED_POINT_SCALE;
    
    // Use the formula: 1 - (1 - t)^5
    return FIXED_POINT_SCALE - tInverseQuintic;
}

void displayPressStart(){
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

void hidePressStart(){
	objectBuffer[PRESS_START_SPRITE1].attr0 = ATTR0_HIDE;
	objectBuffer[PRESS_START_SPRITE2].attr0 = ATTR0_HIDE;
	objectBuffer[PRESS_START_SPRITE3].attr0 = ATTR0_HIDE;
}

void skipToMenu(){
	//mainMenuData.actionTarget = 0;
	//mainMenuData.actionTimer = 0;
	//mainMenuData.state = FLASH_WHITE;
	
	currentScene.state = NORMAL;
	mainMenuData.state = TITLE_FLY_OUT;
	mainMenuData.actionTimer = 1;
	mainMenuData.actionTarget = 300;
	displayPressStart();
}