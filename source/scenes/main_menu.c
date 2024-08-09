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
static u8 currentBGMIndex = 0;
static u8 currentSFXIndex = 0;

// Initialize the Menu Pages
MenuPage menuPages[6] = {
	{
		.items = {
			{"Play Game", PAGE_TRANSFER, .data.intVal = (int)MPI_PLAY_GAME, .dataType = INT, .textGFXIndex = 0},
			{"Extras", PAGE_TRANSFER, .data.intVal = (int)MPI_EXTRAS, .dataType = INT, .textGFXIndex = 2},
			{"Options", PAGE_TRANSFER, .data.intVal = (int)MPI_OPTIONS, .dataType = INT, .textGFXIndex = 4}
		},
		.itemCount = 3,
		.pageName = "MAIN MENU"
	},
	{
		.items = {
			{"New Game", SCRIPT_RUNNER, .data.functionPtr = menuExecNewGame, .dataType = FUNC_PTR, .textGFXIndex = 6},
			{"Continue", SCRIPT_RUNNER, .data.functionPtr = menuExecContinue, .dataType = FUNC_PTR, .textGFXIndex = 8},
			{"Load Game", SCRIPT_RUNNER, .data.functionPtr = menuExecLoadGame, .dataType = FUNC_PTR, .textGFXIndex = 10},
			{"Back", PAGE_TRANSFER, .data.intVal = (int)MPI_MAIN_MENU, .dataType = INT, .textGFXIndex = 12}
		},
		.itemCount = 4,
		.pageName = "PLAY GAME"
	},{
		.items = {
			{"Sound Test", PAGE_TRANSFER, .data.intVal = (int)MPI_SOUND_TEST, .dataType = INT, .textGFXIndex = 36},
			{"Credits", PAGE_TRANSFER, .data.intVal = (int)MPI_CREDITS, .dataType = INT, .textGFXIndex = 38},
			{"Back", PAGE_TRANSFER, .data.intVal = (int)MPI_MAIN_MENU, .dataType = INT, .textGFXIndex = 12}
		},
		.itemCount = 3,
		.pageName = "EXTRAS"
	},
	{
		.items = {
			{"Master Volume", SLIDER, .data.intArray = dataRange, .dataType = INT_ARRAY, .textGFXIndex = 14},
			{"BGM", SLIDER, .data.intArray = dataRange, .dataType = INT_ARRAY, .textGFXIndex = 16},
			{"SFX", SLIDER, .data.intArray = dataRange, .dataType = INT_ARRAY, .textGFXIndex = 18},
			{"Apply Changes", SCRIPT_RUNNER, .data.functionPtr = menuExecOptionsApplyChanges, .dataType = FUNC_PTR, .textGFXIndex = 120},
			{"Abort", PAGE_TRANSFER, .data.intVal = (int)MPI_MAIN_MENU, .dataType = INT, .textGFXIndex = 22}
		},
		.itemCount = 5,
		.pageName = "OPTIONS"
	},
	{
		.items = {
			{"Back", PAGE_TRANSFER, .data.intVal = (int)MPI_EXTRAS, .dataType = INT, .textGFXIndex = 12}
		},
		.itemCount = 1,
		.pageName = "CREDITS"
	},
	{
		.items = {
			{"BGM", SOUND_TESTER, .data.functionPtr = menuExecPlayBGM, .dataType = FUNC_PTR, .textGFXIndex = 16},
			{"SFX", SOUND_TESTER, .data.functionPtr = menuExecPlaySFX, .dataType = FUNC_PTR, .textGFXIndex = 18},
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
	
	memcpy32(&paletteBufferObj[FLYING_COMET_PAL_START << 4], shootingStarPal, sizeof(shootingStarPal) >> 2);
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
	
	memcpy32(&characterBuffer4[FLYING_COMET_GFX_START << 5], shootingStarTiles, sizeof(shootingStarTiles) >> 2);
	memcpy32(&characterBuffer5[0], starBlockerTiles, sizeof(starBlockerTiles) >> 2);
	characterData[4].position = tile_mem[FLYING_COMET_CHARDATA];
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

	// Prepare values for the starryBG to start panning upward, even during the fade-in transition
	mainMenuData.starryBG.yScrollTimerCurrent = 0;
	mainMenuData.starryBG.yScrollTimerTarget = 32+16+2+53;
	mainMenuData.starryBG.yScrollStartPos = TITLE_CAM_PAN_BOTTOM; // Start position (scaled)
	mainMenuData.starryBG.yScrollTargetPos = TITLE_CAM_PAN_TOP; // Target position (scaled)
}

void mainMenuNormal(){
	static u8 currentAsset = 0;
	extern u16 numSounds;
	switch(mainMenuData.state){
	case FLASH_WHITE:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			//Enable bg0, bg1, sprite layer, and sets the hardware to mode 0 (4 tiled bg mode)
			// https://problemkaputt.de/gbatek.htm#lcdiodisplaycontrol
			REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
			drawStarBlocker(9);
			mainMenuData.actionTarget = 16;
			mainMenuData.actionTimer = 0;
			mainMenuData.state = FADE_TO_TITLE;

			IOBuffer1[0] = 16;
		}else{
			mainMenuData.actionTimer++;

			if (mainMenuData.actionTimer == 16)
				currentBGMIndex = playNewSound(_musTitle);
			IOBuffer1[0] = mainMenuData.actionTimer >> 1;
		}
		// Update fade values
		IOData[1].position = (void *)&REG_BLDY;
		IOData[1].buffer = IOBuffer1;
		IOData[1].size = 1;

		// Hide the flying comet sprite and star blocker sprite
		objectBuffer[FLYING_COMET_SPRITE].attr0 = ATTR0_HIDE;
		objectBuffer[STAR_BLOCKER_SPRITE].attr0 = ATTR0_HIDE;

		// Update the BG Positions (using IOBuffer0)
		interpolateStarryBG();
		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.titleCardBG.xPos, mainMenuData.titleCardBG.yPos);
		break;
	case FADE_TO_TITLE:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_PAN_UP;
			mainMenuData.actionTarget = 2;
			mainMenuData.actionTimer = 0;
			IOBuffer1[0] = 0;
		}else{
			mainMenuData.actionTimer++;
			IOBuffer1[0] = 16 - mainMenuData.actionTimer;
		}

		// Update fade values
		IOData[1].position = (void *)&REG_BLDY;
		IOData[1].buffer = IOBuffer1;
		IOData[1].size = 1;

		// Hide the flying comet sprite
		objectBuffer[FLYING_COMET_SPRITE].attr0 = ATTR0_HIDE;
		objectBuffer[STAR_BLOCKER_SPRITE].attr0 = ATTR0_HIDE;
		updateObjBuffer();

		// Update the BG Positions (using IOBuffer0)
		interpolateStarryBG();
		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.titleCardBG.xPos, mainMenuData.titleCardBG.yPos);
		break;
	case TITLE_PAN_UP:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_FLASH;
			mainMenuData.actionTarget = 53;
			mainMenuData.actionTimer = 0;
			titleRevealFadeDirection = 1;

			// Queue IOBuffer1 for controlling fade on REG_BLDY in the next state (TITLE_FLASH)
			IOBuffer1[0] = 0;
		}else{
			// Increment actionTimer
			mainMenuData.actionTimer++;
		}

		// Update the BG Positions (using IOBuffer0)
		interpolateStarryBG();
		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.titleCardBG.xPos, mainMenuData.titleCardBG.yPos);
		break;
	case TITLE_FLASH:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_REVEAL;
			mainMenuData.actionTarget = 46;
			mainMenuData.actionTimer = 0;

			// Send the Title BG tilemap
			tilemapData[1].position = &se_mem[TITLE_CARD_TILEMAP];
			tilemapData[1].buffer = (void *)sprTitleLogoMap;
			tilemapData[1].size = sizeof(sprTitleLogoMap) >> 2;
		}else{
			mainMenuData.actionTimer++;
			IOBuffer1[0] = (mainMenuData.actionTimer * 2) >> 1;
		}
		
		// 39 frames before the final destination, start displaying the star sprite at its designated position
		if (mainMenuData.actionTimer > mainMenuData.actionTarget - 40){
			int index = mainMenuData.actionTimer - (mainMenuData.actionTarget - 40);
			if (index >= 0 && index <= 31) // Ensure index is within bounds
				drawStarBlocker(starBlockerYPos[index - 1]);
			else
				drawStarBlocker(9);
		}

		if (mainMenuData.actionTimer >= mainMenuData.actionTarget - 16){
			// Queue IOBuffer1 for controlling fade on REG_BLDY in the next state (TITLE_REVEAL)
			
			if (IOBuffer1[0] + 2 * titleRevealFadeDirection <= 16 &&
				IOBuffer1[0] + 2 * titleRevealFadeDirection >= 0)
				IOBuffer1[0] = clamp(IOBuffer1[0] + 2 * titleRevealFadeDirection, 0, 16);

			if (mainMenuData.actionTimer >= mainMenuData.actionTarget - 10)
				titleRevealFadeDirection = -1;
			
			// Update fade values
			IOData[1].position = (void *)&REG_BLDY;
			IOData[1].buffer = IOBuffer1;
			IOData[1].size = 1;
			
		}

		// Hide the flying comet sprite
		objectBuffer[FLYING_COMET_SPRITE].attr0 = ATTR0_HIDE;
		updateObjBuffer();

		// Update the BG Positions (using IOBuffer0)
		interpolateStarryBG();
		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.titleCardBG.xPos, mainMenuData.titleCardBG.yPos);
		break;
	case TITLE_REVEAL:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_FLYING_COMET_ANIMATION;
			mainMenuData.actionTarget = 64;
			mainMenuData.actionTimer = 0;
		}else{
			mainMenuData.actionTimer++;

			// Control the fading values, utilizing IOBuffer1
			if (IOBuffer1[0] > 0)
				IOBuffer1[0] = 16 - mainMenuData.actionTimer * 2;
			else
				IOBuffer1[0] = 0;
		}

		// Update fade values
		IOData[1].position = (void *)&REG_BLDY;
		IOData[1].buffer = IOBuffer1;
		IOData[1].size = 1;

		// Hide the flying comet sprite
		objectBuffer[FLYING_COMET_SPRITE].attr0 = ATTR0_HIDE;
		updateObjBuffer();
		break;
	case TITLE_FLYING_COMET_ANIMATION:
		if(mainMenuData.actionTimer == mainMenuData.actionTarget){
			mainMenuData.state = TITLE_BEFORE_HOLD;
			objectBuffer[FLYING_COMET_SPRITE].attr0 = ATTR0_HIDE;
			objectBuffer[STAR_BLOCKER_SPRITE].attr0 = ATTR0_HIDE;
			mainMenuData.actionTimer = 0;
			mainMenuData.actionTarget = 220;
		}else{
			u8 starFrame = mainMenuData.actionTimer >> 2;
			objectBuffer[FLYING_COMET_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(shootingStarYPos[starFrame]);
			objectBuffer[FLYING_COMET_SPRITE].attr1 = ATTR1_SIZE_64 | ATTR1_X(shootingStarXPos[starFrame]);
			objectBuffer[FLYING_COMET_SPRITE].attr2 = ATTR2_ID((starFrame << 5) + FLYING_COMET_GFX_START) | ATTR2_PRIO(3) | ATTR2_PALBANK(0);
			mainMenuData.actionTimer++;
		}
		updateObjBuffer();
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
		}else{
			mainMenuData.actionTimer++;
		}
		updateObjBuffer();
		break;
	case TITLE_HOLD:
		mainMenuData.actionTimer++;
		
		if(mainMenuData.actionTimer % 128 < 64){
			drawPressStart();
		}else{
			hidePressStart();
		}
		// Make the starry background scroll up-left
		scrollStarryBG(-1, -1);

		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.titleCardBG.xPos, mainMenuData.titleCardBG.yPos);
		
		updateObjBuffer();
		break;
	case TITLE_AFTER_PRESS_START:
		if(mainMenuData.actionTimer >= mainMenuData.actionTarget){
			hidePressStart();
			skipToMenu();
		}else{
			mainMenuData.actionTimer++;
		}

		// Rapidly blink the "Press Start" graphic
		if(mainMenuData.actionTimer % 16 >= 8){
			drawPressStart();
		}else{
			hidePressStart();
		}

		updateObjBuffer();
		break;
	case TITLE_FLY_OUT:
		if(mainMenuData.actionTimer >= mainMenuData.actionTarget){
			mainMenuData.state = MAIN_MENU_FLY_IN;
			
			currentBGMIndex = playNewSound(_musMainMenu);
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
		
		updateObjBuffer();
		
		if (mainMenuData.actionTimer > 40){
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
		// Set the Menu State to "MAIN_MENU_HOLD and re-init its timers"
		mainMenuData.actionTarget = 32;
		mainMenuData.actionTimer = 0;
		mainMenuData.state = MAIN_MENU_HOLD;
		
		initMainMenu();
		drawMainMenu();

		// Make the starry background scroll left
		scrollStarryBG(-1, 0);

		// Queue BG Scroll registers for the Starry BG and Menu Positions
		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.menuBG.xPos, mainMenuData.menuBG.yPos);
		break;
	case MAIN_MENU_HOLD:
		updateMainMenu();

		drawMainMenu();

		// Make the starry background scroll up-left
		scrollStarryBG(-1, 0);

		// Queue BG Scroll registers for the Starry BG and Menu Positions
		updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.menuBG.xPos, mainMenuData.menuBG.yPos);
		
		break;
	case MAIN_MENU_FLY_OUT:
		if(mainMenuData.actionTimer >= mainMenuData.actionTarget){
			// Start the match
			mainMenuEnd();
		}else{
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

	if (mainMenuData.state < TITLE_AFTER_PRESS_START){
		if((inputs.pressed & KEY_A) || (inputs.pressed & KEY_START)){
			
			REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
			//clear the titlecard tilemap
			memset32(tilemapBuffer0, 0, sizeof(tilemapBuffer0) >> 2);
			tilemapData[1].size = sizeof(tilemapBuffer0) >> 2;
			tilemapData[1].buffer = tilemapBuffer0;
			tilemapData[1].position = &se_mem[TITLE_CARD_TILEMAP];

			mainMenuData.state = TITLE_AFTER_PRESS_START;
			mainMenuData.actionTimer = 1;
			mainMenuData.actionTarget = 30;
			
			// Reset the title card's coordinates
			mainMenuData.titleCardBG.xPos = 512 - 15;
			mainMenuData.titleCardBG.yPos = 512 - 43;

			// Send the Title BG tilemap
			tilemapData[1].position = &se_mem[TITLE_CARD_TILEMAP];
			tilemapData[1].buffer = (void *)sprTitleLogoMap;
			tilemapData[1].size = sizeof(sprTitleLogoMap) >> 2;
			
			// Load the Press Start graphics
			memcpy32(&paletteBufferObj[PRESS_START_PAL_START << 4], sprTitlePressStartTextPal, sizeof(sprTitlePressStartTextPal) >> 2);
			paletteData[1].size = 16;
			paletteData[1].position = pal_obj_mem;
			paletteData[1].buffer = (void *)paletteBufferObj;
			
			memcpy32(&characterBuffer4[0], sprTitlePressStartTextTiles, sizeof(sprTitlePressStartTextTiles) >> 2);
			characterData[4].position = tile_mem[PRESS_START_CHARDATA];
			characterData[4].buffer = (void *)characterBuffer4;
			characterData[4].size = sizeof(characterBuffer4) >> 2;
		
			// Update fade values
			IOBuffer1[0] = 0;
			IOData[1].position = (void *)&REG_BLDY;
			IOData[1].buffer = IOBuffer1;
			IOData[1].size = 1;

			// Hide the flying comet sprite and star blocker sprite
			objectBuffer[FLYING_COMET_SPRITE].attr0 = ATTR0_HIDE;
			objectBuffer[STAR_BLOCKER_SPRITE].attr0 = ATTR0_HIDE;

			endAsset(currentBGMIndex);
			currentSFXIndex = playNewSound(_sfxMenuConfirmC);

			yStart = mainMenuData.starryBG.yPos * FIXED_POINT_SCALE; // Start position (scaled)
			yTarget = -4504 * FIXED_POINT_SCALE; // Target position (scaled)
			titleCardYStart = mainMenuData.titleCardBG.yPos * FIXED_POINT_SCALE;
			titleCardYTarget = (mainMenuData.titleCardBG.yPos + 140) * FIXED_POINT_SCALE;

			updateBGScrollRegisters(mainMenuData.starryBG.xPos, mainMenuData.starryBG.yPos, mainMenuData.titleCardBG.xPos, mainMenuData.titleCardBG.yPos);
		}
	}
}

void initMainMenu(){
	mainMenuData.currMenuPage = 0;
	mainMenuData.winSliceWidth = 10;
	mainMenuData.menuCursorPos = 0;
}

void updateMainMenu(){
	MenuPage* menuPage = &menuPages[mainMenuData.currMenuPage];

	// Allow Up/Down to navigate the menu; wrap around if we hit the upper/lower limits
	// Combine both up and down input checks into a single operation
	int moveY = 0;
	if((inputs.pressed & KEY_UP) && !(inputs.pressed & KEY_DOWN)){
		moveY = -1;
	}
	else if((inputs.pressed & KEY_DOWN) && !(inputs.pressed & KEY_UP)){
		moveY = 1;
	}

	// Navigate the menu; wrap around if we hit an edge
	if (moveY != 0) {
		playNewSound(_sfxMenuMove);
		if((mainMenuData.menuCursorPos + moveY) < 0){
			mainMenuData.menuCursorPos = menuPage->itemCount - 1;
		}
		else if ((mainMenuData.menuCursorPos + moveY) > (menuPage->itemCount - 1)){
			mainMenuData.menuCursorPos = 0;
		}
		else{
			mainMenuData.menuCursorPos += moveY;
		}
	}
	
	if((inputs.pressed & KEY_A) || (inputs.pressed & KEY_START)){
		
	}
}

void drawMainMenu(){

		// Write to tilemap layer 1 using tilemapBuffer1
		drawNineSliceWindow(10, 6, mainMenuData.winSliceWidth, 10, 1);
		

		
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

		MenuPage* menuPage = &menuPages[mainMenuData.currMenuPage];
		for(int i = 0; i < menuPage->itemCount; ++i){
			MenuPageItem* thisMenuElement = &menuPage->items[i];
			bool cursorOnElement = (mainMenuData.menuCursorPos == i);
			drawMenuTextSegment(mainMenuData.winSliceWidth, 10, 8 + (2 * i), thisMenuElement->textGFXIndex, 2, cursorOnElement);
		}

		tilemapData[1].position = &se_mem[MENU_TILEMAP];
		tilemapData[1].buffer = (void *)tilemapBuffer1;
		tilemapData[1].size = 512;

		// Write to tilemap layer 2 using tilemapBuffer2
		//drawNineSliceWindow(10, 6, 9, 9, 2);

		//tilemapData[2].position = &se_mem[MENU_TILEMAP];
		//tilemapData[2].buffer = (void *)tilemapBuffer2;
		//tilemapData[2].size = 512;
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

void interpolateStarryBG(){
	// Calculate the interpolation factor t, with proper scaling
	int actionTimerScaled = mainMenuData.starryBG.yScrollTimerCurrent * FIXED_POINT_SCALE;
	int actionTargetScaled = mainMenuData.starryBG.yScrollTimerTarget * FIXED_POINT_SCALE;
	int t = (actionTimerScaled * FIXED_POINT_SCALE) / actionTargetScaled;

	// Apply ease-in-out function
	int easedT = easeInOut(t, 4);

	// Calculate the interpolated position and update yPos
	mainMenuData.starryBG.yPos = lerp(mainMenuData.starryBG.yScrollStartPos * FIXED_POINT_SCALE, mainMenuData.starryBG.yScrollTargetPos * FIXED_POINT_SCALE, easedT) / FIXED_POINT_SCALE;
	
	if (mainMenuData.starryBG.yScrollTimerCurrent < mainMenuData.starryBG.yScrollTimerTarget)
		mainMenuData.starryBG.yScrollTimerCurrent++;
	else
		mainMenuData.starryBG.yScrollTimerCurrent = mainMenuData.starryBG.yScrollTimerTarget;
}

void setTile(int x, int y, int drawingTileIndex, bool flipHorizontal, bool flipVertical, int palette, int layer){
    u16* tilemapBuffer;

    switch(layer){
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
    }else{
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
	
	for (int row = 0; row < 2; row++){
		for (int tileXOff = 0; tileXOff < MENU_TEXT_TILE_WIDTH; tileXOff++){
			int vramIndex = tilesetIndex + tileXOff + (menuElementPosition * MENU_TEXT_TILE_WIDTH) + (row * MENU_TEXT_TILE_WIDTH);
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
    }else{
        int temp = t - FIXED_POINT_SCALE;
        return (halfFactor * -temp * temp) / FIXED_POINT_SCALE + FIXED_POINT_SCALE;
    }
}

int easeOutQuint(int t){
    // Convert t to a floating-point value in fixed-point arithmetic
    int tInverse = FIXED_POINT_SCALE - t;
    int tInverseSquared = (tInverse * tInverse) / FIXED_POINT_SCALE;
    int tInverseCubed = (tInverse * tInverseSquared) / FIXED_POINT_SCALE;
    int tInverseQuartic = (tInverse * tInverseCubed) / FIXED_POINT_SCALE;
    int tInverseQuintic = (tInverse * tInverseQuartic) / FIXED_POINT_SCALE;
    
    // Use the formula: 1 - (1 - t)^5
    return FIXED_POINT_SCALE - tInverseQuintic;
}

void drawPressStart(){
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

void drawStarBlocker(int yPos){
	objectBuffer[STAR_BLOCKER_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(yPos);
	objectBuffer[STAR_BLOCKER_SPRITE].attr1 = ATTR1_SIZE_32 | ATTR1_X(185);
	objectBuffer[STAR_BLOCKER_SPRITE].attr2 = ATTR2_ID(STAR_BLOCKER_GFX_START) | ATTR2_PRIO(3) | ATTR2_PALBANK(1);
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
}

void updateObjBuffer(){
	OAMData.position = (void *)oam_mem;
	OAMData.buffer = objectBuffer;
	OAMData.size = sizeof(objectBuffer) >> 2;
}