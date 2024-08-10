#include "main_menu.h"

MainMenuData mDat = {
	.menuElementsWidth = {10,10,9,11,13,11}
};

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
int moveY = 0, moveX = 0; // Player directional input

// Initialize the Menu Pages
MenuPage* menuPage;
MenuPage menuPages[6] = {
	{
		.items = {
			{"Play Game", ME_PAGE_TRANSFER, .data.intVal = (int)MPI_PLAY_GAME, .dataType = MPIDT_INT, .textGFXIndex = 0},
			{"Options", ME_PAGE_TRANSFER, .data.intVal = (int)MPI_OPTIONS, .dataType = MPIDT_INT, .textGFXIndex = 2},
			{"Extras", ME_PAGE_TRANSFER, .data.intVal = (int)MPI_EXTRAS, .dataType = MPIDT_INT, .textGFXIndex = 4}
		},
		.itemCount = 3,
		.pageName = "MAIN MENU",
		.tileX = 10,
		.tileY = 6,
		.tileWidth = 10,
		.tileHeight = 10,
		.pxOffX = 0,
		.backPage = (int)MPI_MAIN_MENU
	},
	{
		.items = {
			{"New Game", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecNewGame, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 6},
			{"Continue", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecContinue, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 8},
			{"Load Game", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecLoadGame, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 10},
			{"Back", ME_PAGE_TRANSFER, .data.intVal = (int)MPI_MAIN_MENU, .dataType = MPIDT_INT, .textGFXIndex = 12}
		},
		.itemCount = 4,
		.pageName = "PLAY GAME",
		.tileX = 10,
		.tileY = 6,
		.tileWidth = 10,
		.tileHeight = 12,
		.pxOffX = 0,
		.backPage = (int)MPI_MAIN_MENU
	},
	{
		.items = {
			{"BGM", ME_SOUND_TESTER, .data.functionPtr = menuExecPlayBGM, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 16},
			{"SFX", ME_SOUND_TESTER, .data.functionPtr = menuExecPlaySFX, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 18},
			{"Back", ME_PAGE_TRANSFER, .data.intVal = (int)MPI_EXTRAS, .dataType = MPIDT_INT, .textGFXIndex = 24}
		},
		.itemCount = 3,
		.pageName = "SOUND TEST",
		.tileX = 5,
		.tileY = 6,
		.tileWidth = 22,
		.tileHeight = 10,
		.pxOffX = 4,
		.backPage = (int)MPI_EXTRAS
	},
	{
		.items = {
			{"Sound Test", ME_PAGE_TRANSFER, .data.intVal = (int)MPI_SOUND_TEST, .dataType = MPIDT_INT, .textGFXIndex = 38},
			{"Credits", ME_PAGE_TRANSFER, .data.intVal = (int)MPI_CREDITS, .dataType = MPIDT_INT, .textGFXIndex = 40},
			{"Back", ME_PAGE_TRANSFER, .data.intVal = (int)MPI_MAIN_MENU, .dataType = MPIDT_INT, .textGFXIndex = 42}
		},
		.itemCount = 3,
		.pageName = "EXTRAS",
		.tileX = 11,
		.tileY = 6,
		.tileWidth = 9,
		.tileHeight = 10,
		.pxOffX = 4,
		.backPage = (int)MPI_MAIN_MENU
	},
	{
		.items = {
			{"- Programming -", ME_CREDITS_DISPLAY, .data.intVal = 0, .dataType = MPIDT_INT, .textGFXIndex = 26},
			{"origamiscienceguy", ME_CREDITS_DISPLAY, .data.intVal = 0, .dataType = MPIDT_INT, .textGFXIndex = 32},
			{"- Graphics -", ME_CREDITS_DISPLAY, .data.intVal = 0, .dataType = MPIDT_INT, .textGFXIndex = 30},
			{"n67094", ME_CREDITS_DISPLAY, .data.intVal = 0, .dataType = MPIDT_INT, .textGFXIndex = 36},
			{"- Audio -", ME_CREDITS_DISPLAY, .data.intVal = 0, .dataType = MPIDT_INT, .textGFXIndex = 28},
			{"potatoTeto", ME_CREDITS_DISPLAY, .data.intVal = 0, .dataType = MPIDT_INT, .textGFXIndex = 34}
		},
		.itemCount = 6,
		.pageName = "CREDITS",
		.tileX = 9,
		.tileY = 4,
		.tileWidth = 13,
		.tileHeight = 16,
		.pxOffX = 4,
		.backPage = (int)MPI_EXTRAS
	},
	{
		.items = {
			{"Master Volume", ME_SLIDER, .data.intArray = dataRange, .dataType = MPIDT_INT_ARRAY, .textGFXIndex = 14},
			{"BGM", ME_SLIDER, .data.intArray = dataRange, .dataType = MPIDT_INT_ARRAY, .textGFXIndex = 16},
			{"SFX", ME_SLIDER, .data.intArray = dataRange, .dataType = MPIDT_INT_ARRAY, .textGFXIndex = 18},
			{"Apply Changes", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecOptionsApplyChanges, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 20},
			{"Abort", ME_PAGE_TRANSFER, .data.intVal = (int)MPI_MAIN_MENU, .dataType = MPIDT_INT, .textGFXIndex = 22}
		},
		.itemCount = 5,
		.pageName = "OPTIONS",
		.tileX = 5,//10,
		.tileY = 6,
		.tileWidth = 22,//11
		.tileHeight = 14,
		.pxOffX = 4,
		.backPage = (int)MPI_MAIN_MENU
	}
};


void mainMenuInitialize(){
	REG_DISPCNT = DCNT_MODE0; //black screen
	REG_BG0CNT = BG_4BPP | BG_SBB(STARRY_IMAGE_TILEMAP) | BG_CBB(STARRY_IMAGE_CHARDATA) | BG_PRIO(3) | BG_REG_64x64; //starry background layer
	REG_BG1CNT = BG_4BPP | BG_SBB(TITLE_CARD_TILEMAP) | BG_CBB(TITLE_CARD_CHARDATA) | BG_PRIO(2) | BG_REG_32x32; //title screen layer
	REG_BG2CNT = BG_4BPP | BG_SBB(MENU_WINDOW_TILEMAP) | BG_CBB(MENU_CHARDATA) | BG_PRIO(1) | BG_REG_32x32; //menu page ui layer
	REG_BLDCNT = BLD_TOP(BLD_BG2 | BLD_BACKDROP | BLD_OBJ) | BLD_WHITE;
	REG_BLDY = BLDY(0);
	mDat.starryBG.xPos = 512 - 16;
	REG_BG0HOFS = mDat.starryBG.xPos;
	mDat.starryBG.yPos = TITLE_CAM_PAN_BOTTOM;
	mDat.titleCardBG.xPos = 512 - 15;
	mDat.titleCardBG.yPos = 512 - 43;
	mDat.menuBG.xPos = 0;
	mDat.menuBG.yPos = 0;

	REG_BG0VOFS = mDat.starryBG.yPos;
	REG_BG1HOFS = mDat.titleCardBG.xPos;
	REG_BG1VOFS = mDat.titleCardBG.yPos;
	REG_BG2HOFS = mDat.titleCardBG.xPos;
	REG_BG2VOFS = mDat.titleCardBG.yPos;
	
	//send the palettes
	memcpy32(&paletteBufferBg[STARRY_IMAGE_PAL_START << 4], main_menu_starfieldPal, sizeof(main_menu_starfieldPal) >> 2);
	memcpy32(&paletteBufferBg[TITLE_CARD_PAL_START << 4], sprTitleLogoPal, sizeof(sprTitleLogoPal) >> 2);
	memcpy32(&paletteBufferBg[MENU_PAL_START << 4], tsMenuUIPal, sizeof(tsMenuUIPal) >> 2);

	paletteData[0].size = 24;
	paletteData[0].position = pal_bg_mem;
	paletteData[0].buffer = paletteBufferBg;
	
	memcpy32(&paletteBufferObj[FLYING_COMET_PAL_START << 4], shootingStarPal, sizeof(shootingStarPal) >> 2);
	memcpy32(&paletteBufferObj[STAR_BLOCKER_PAL_START << 4], starBlockerPal, sizeof(starBlockerPal) >> 2);
	memcpy32(&paletteBufferObj[MENU_BUTTON_PROMPT_PAL << 4], menu_button_promptsPal, sizeof(menu_button_promptsPal) >> 2);
	
	paletteData[1].size = 32;
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
	memcpy32(&characterBuffer5[MENU_SLIDER_PROMPT_GFX_START << 5], menu_slider_promptTiles, sizeof(menu_slider_promptTiles) >> 2);
	memcpy32(&characterBuffer5[MENU_BUTTON_PROMPT_GFX_START << 5], menu_button_promptsTiles, sizeof(menu_button_promptsTiles) >> 2);
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
	
	mDat.actionTarget = 32;
	mDat.actionTimer = 0;
	mDat.state = FLASH_WHITE;
	currentScene.state = NORMAL;

	// Prepare values for the starryBG to start panning upward, even during the fade-in transition
	mDat.starryBG.yScrollTimerCurrent = 0;
	mDat.starryBG.yScrollTimerTarget = 32+16+2+53;
	mDat.starryBG.yScrollStartPos = TITLE_CAM_PAN_BOTTOM; // Start position (scaled)
	mDat.starryBG.yScrollTargetPos = TITLE_CAM_PAN_TOP; // Target position (scaled)
}

void mainMenuNormal(){

	//temporary debug input
	if(inputs.pressed & KEY_START){
		currentScene.scenePointer = sceneList[GAMEPLAY];
		currentScene.state = INITIALIZE;
		endAsset(currentBGMIndex);
	}
	static u8 currentAsset = 0;
	extern u16 numSounds;
	switch(mDat.state){
	case FLASH_WHITE:
		if(mDat.actionTimer == mDat.actionTarget){
			//Enable bg0, bg1, sprite layer, and sets the hardware to mode 0 (4 tiled bg mode)
			// https://problemkaputt.de/gbatek.htm#lcdiodisplaycontrol
			REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
			drawStarBlocker(9);
			mDat.actionTarget = 16;
			mDat.actionTimer = 0;
			mDat.state = FADE_TO_TITLE;

			IOBuffer1[0] = 16;
		}else{
			mDat.actionTimer++;

			if (mDat.actionTimer == 16)
				currentBGMIndex = playNewSound(_musTitle);
			IOBuffer1[0] = mDat.actionTimer >> 1;
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
		updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.titleCardBG.xPos, mDat.titleCardBG.yPos);
		break;
	case FADE_TO_TITLE:
		if(mDat.actionTimer == mDat.actionTarget){
			mDat.state = TITLE_PAN_UP;
			mDat.actionTarget = 2;
			mDat.actionTimer = 0;
			IOBuffer1[0] = 0;
		}else{
			mDat.actionTimer++;
			IOBuffer1[0] = 16 - mDat.actionTimer;
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
		updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.titleCardBG.xPos, mDat.titleCardBG.yPos);
		break;
	case TITLE_PAN_UP:
		if(mDat.actionTimer == mDat.actionTarget){
			mDat.state = TITLE_FLASH;
			mDat.actionTarget = 53;
			mDat.actionTimer = 0;
			titleRevealFadeDirection = 1;

			// Queue IOBuffer1 for controlling fade on REG_BLDY in the next state (TITLE_FLASH)
			IOBuffer1[0] = 0;
		}else{
			// Increment actionTimer
			mDat.actionTimer++;
		}

		// Update the BG Positions (using IOBuffer0)
		interpolateStarryBG();
		updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.titleCardBG.xPos, mDat.titleCardBG.yPos);
		break;
	case TITLE_FLASH:
		if(mDat.actionTimer == mDat.actionTarget){
			mDat.state = TITLE_REVEAL;
			mDat.actionTarget = 46;
			mDat.actionTimer = 0;

			// Send the Title BG tilemap
			tilemapData[1].position = &se_mem[TITLE_CARD_TILEMAP];
			tilemapData[1].buffer = (void *)sprTitleLogoMap;
			tilemapData[1].size = sizeof(sprTitleLogoMap) >> 2;
		}else{
			mDat.actionTimer++;
			IOBuffer1[0] = (mDat.actionTimer * 2) >> 1;
		}
		
		// 39 frames before the final destination, start displaying the star sprite at its designated position
		if (mDat.actionTimer > mDat.actionTarget - 40){
			int index = mDat.actionTimer - (mDat.actionTarget - 40);
			if (index >= 0 && index <= 31) // Ensure index is within bounds
				drawStarBlocker(starBlockerYPos[index - 1]);
			else
				drawStarBlocker(9);
		}

		if (mDat.actionTimer >= mDat.actionTarget - 16){
			// Queue IOBuffer1 for controlling fade on REG_BLDY in the next state (TITLE_REVEAL)
			
			if (IOBuffer1[0] + 2 * titleRevealFadeDirection <= 16 &&
				IOBuffer1[0] + 2 * titleRevealFadeDirection >= 0)
				IOBuffer1[0] = clamp(IOBuffer1[0] + 2 * titleRevealFadeDirection, 0, 16);

			if (mDat.actionTimer >= mDat.actionTarget - 10)
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
		updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.titleCardBG.xPos, mDat.titleCardBG.yPos);
		break;
	case TITLE_REVEAL:
		if(mDat.actionTimer == mDat.actionTarget){
			mDat.state = TITLE_FLYING_COMET_ANIMATION;
			mDat.actionTarget = 64;
			mDat.actionTimer = 0;
		}else{
			mDat.actionTimer++;

			// Control the fading values, utilizing IOBuffer1
			if (IOBuffer1[0] > 0)
				IOBuffer1[0] = 16 - mDat.actionTimer * 2;
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
		if(mDat.actionTimer == mDat.actionTarget){
			mDat.state = TITLE_BEFORE_HOLD;
			objectBuffer[FLYING_COMET_SPRITE].attr0 = ATTR0_HIDE;
			objectBuffer[STAR_BLOCKER_SPRITE].attr0 = ATTR0_HIDE;
			mDat.actionTimer = 0;
			mDat.actionTarget = 220;
		}else{
			u8 starFrame = mDat.actionTimer >> 2;
			objectBuffer[FLYING_COMET_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(shootingStarYPos[starFrame]);
			objectBuffer[FLYING_COMET_SPRITE].attr1 = ATTR1_SIZE_64 | ATTR1_X(shootingStarXPos[starFrame]);
			objectBuffer[FLYING_COMET_SPRITE].attr2 = ATTR2_ID((starFrame << 5) + FLYING_COMET_GFX_START) | ATTR2_PRIO(3) | ATTR2_PALBANK(0);
			mDat.actionTimer++;
		}
		updateObjBuffer();
		break;
	case TITLE_BEFORE_HOLD:
		if(mDat.actionTimer == mDat.actionTarget){
			mDat.state = TITLE_HOLD;
			mDat.actionTimer = 0;
			mDat.actionTarget = 0;
			memcpy32(&paletteBufferObj[PRESS_START_PAL_START << 4], sprTitlePressStartTextPal, sizeof(sprTitlePressStartTextPal) >> 2);
			paletteData[1].size = 16;
			paletteData[1].position = pal_obj_mem;
			paletteData[1].buffer = (void *)paletteBufferObj;
			
			memcpy32(&characterBuffer4[0], sprTitlePressStartTextTiles, sizeof(sprTitlePressStartTextTiles) >> 2);
			characterData[4].position = tile_mem[PRESS_START_CHARDATA];
			characterData[4].buffer = (void *)characterBuffer4;
			characterData[4].size = sizeof(characterBuffer4) >> 2;
		}else{
			mDat.actionTimer++;
		}
		updateObjBuffer();
		break;
	case TITLE_HOLD:
		mDat.actionTimer++;
		
		if(mDat.actionTimer % 128 < 64){
			drawPressStart();
		}else{
			hidePressStart();
		}
		// Make the starry background scroll up-left
		scrollStarryBG(-1, -1);

		updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.titleCardBG.xPos, mDat.titleCardBG.yPos);
		
		updateObjBuffer();
		break;
	case TITLE_AFTER_PRESS_START:
		if(mDat.actionTimer >= mDat.actionTarget){
			hidePressStart();
			skipToMenu();
		}else{
			mDat.actionTimer++;
		}

		// Rapidly blink the "Press Start" graphic
		if(mDat.actionTimer % 16 >= 8){
			drawPressStart();
		}else{
			hidePressStart();
		}

		updateObjBuffer();
		break;
	case TITLE_FLY_OUT:
		if(mDat.actionTimer >= mDat.actionTarget){
			mDat.state = MAIN_MENU_FLY_IN;
			
			currentBGMIndex = playNewSound(_musMainMenu);
			mDat.menuBG.xPos = 512 - 2;
			mDat.menuBG.yPos = 0;

			
			//Clear the menu tilemaps
			memset32(tilemapBuffer1, 0, 512);
			memset32(tilemapBuffer2, 0, 512);
			
			// Queue the tilemap with our drawing functions, using tilemapBuffer1
			tilemapData[1].position = se_mem[MENU_WINDOW_TILEMAP];
			tilemapData[1].buffer = (void *)tilemapBuffer1;
			tilemapData[1].size = 512;

			// Queue the tilemap with our drawing functions, using tilemapBuffer1
			tilemapData[2].position = se_mem[MENU_PAGE_TILEMAP];
			tilemapData[2].buffer = (void *)tilemapBuffer2;
			tilemapData[2].size = 512;

			loadGFX(MENU_CHARDATA, MENU_TEXT_GFX_START, (void *)menu_actionTiles, MENU_TEXT_TILE_WIDTH * 0, MENU_TEXT_TILE_WIDTH * 6, 0);
			loadGFX(MENU_CHARDATA, MENU_TEXT_FOCUSED_GFX_START, (void *)menu_action_focusedTiles, MENU_TEXT_TILE_WIDTH * 0, MENU_TEXT_TILE_WIDTH * 6, 1);
		}else{
			// Make the starry background scroll up, *very* quickly. Use quadratic interpolation

			// Calculate the interpolation factor t, with proper scaling
			int actionTimerScaled = mDat.actionTimer * FIXED_POINT_SCALE;
			int actionTargetScaled = mDat.actionTarget * FIXED_POINT_SCALE;
			int t = (actionTimerScaled * FIXED_POINT_SCALE) / actionTargetScaled;
			int t2 = t * 8;

			// Apply ease-in-out function
			int easedT = easeOutQuint(t);
			int easedT2 = easeInOut(t2, 4);

			// Calculate the interpolated position and update yPos
			mDat.starryBG.yPos = lerp(yStart, yTarget, easedT) / FIXED_POINT_SCALE;
			mDat.titleCardBG.yPos = lerp(titleCardYStart, titleCardYTarget, easedT2) / FIXED_POINT_SCALE;
			mDat.actionTimer++;
		}
		
		updateObjBuffer();
		
		if (mDat.actionTimer > 40){
			// Hide the title card after 40 frames in this state
			memset32(tilemapBuffer1, 0, sizeof(sprTitleLogoMap) >> 2);
			tilemapData[1].size = sizeof(sprTitleLogoMap) >> 2;
			tilemapData[1].buffer = tilemapBuffer1;
			tilemapData[1].position = &se_mem[TITLE_CARD_TILEMAP];
		}

		// Queue BG Scroll registers for the Starry BG and Menu Positions
		updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.titleCardBG.xPos, mDat.titleCardBG.yPos);
		break;
	case MAIN_MENU_FLY_IN:
		// Set the Menu State to "MAIN_MENU_HOLD and re-init its timers"
		mDat.actionTarget = 32;
		mDat.actionTimer = 0;
		mDat.state = MAIN_MENU_HOLD;
		
		initMainMenu();
		drawMainMenu();

		// Make the starry background scroll left
		scrollStarryBG(-1, 0);

		// Queue BG Scroll registers for the Starry BG and Menu Positions
		updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.menuBG.xPos, mDat.menuBG.yPos);
		break;
	case MAIN_MENU_HOLD:
		updateMainMenu();

		drawMainMenu();

		// Make the starry background scroll up-left
		scrollStarryBG(-1, 0);

		// Queue BG Scroll registers for the Starry BG and Menu Positions
		updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.menuBG.xPos, mDat.menuBG.yPos);
		
		break;
	case MAIN_MENU_FLY_OUT:
		if(mDat.actionTimer >= mDat.actionTarget){
			// Start the match
			mainMenuEnd();
		}else{
			mDat.actionTimer++;
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
		updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.menuBG.xPos, mDat.menuBG.yPos);
		break;
	default:
		break;
	}

	if (mDat.state < TITLE_AFTER_PRESS_START){
		if((inputs.pressed & KEY_A) || (inputs.pressed & KEY_START)){
			REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
			//clear the titlecard tilemap
			memset32(tilemapBuffer0, 0, sizeof(tilemapBuffer0) >> 2);
			tilemapData[1].size = sizeof(tilemapBuffer0) >> 2;
			tilemapData[1].buffer = tilemapBuffer0;
			tilemapData[1].position = &se_mem[TITLE_CARD_TILEMAP];

			mDat.state = TITLE_AFTER_PRESS_START;
			mDat.actionTimer = 1;
			mDat.actionTarget = 30;
			
			// Reset the title card's coordinates
			mDat.titleCardBG.xPos = 512 - 15;
			mDat.titleCardBG.yPos = 512 - 43;

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

			yStart = mDat.starryBG.yPos * FIXED_POINT_SCALE; // Start position (scaled)
			yTarget = -4504 * FIXED_POINT_SCALE; // Target position (scaled)
			titleCardYStart = mDat.titleCardBG.yPos * FIXED_POINT_SCALE;
			titleCardYTarget = (mDat.titleCardBG.yPos + 140) * FIXED_POINT_SCALE;

			updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.titleCardBG.xPos, mDat.titleCardBG.yPos);
		}
	}
}

void initMainMenu(){
	//set the active screen layers
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
//	REG_BG0CNT = BG_4BPP | BG_SBB(STARRY_IMAGE_TILEMAP) | BG_CBB(STARRY_IMAGE_CHARDATA) | BG_PRIO(3) | BG_REG_64x64; //starry background layer
	REG_BG1CNT = BG_4BPP | BG_SBB(MENU_WINDOW_TILEMAP) | BG_CBB(MENU_CHARDATA) | BG_PRIO(2) | BG_REG_32x32; //title screen layer
	REG_BG2CNT = BG_4BPP | BG_SBB(MENU_PAGE_TILEMAP) | BG_CBB(MENU_CHARDATA) | BG_PRIO(1) | BG_REG_32x32; //menu page ui layer

	// Queue the tilemap with our drawing functions, using tilemapBuffer1
	tilemapData[1].position = se_mem[MENU_WINDOW_TILEMAP];
	tilemapData[1].buffer = (void *)tilemapBuffer1;
	tilemapData[1].size = 512;

	// Queue the tilemap with our drawing functions, using tilemapBuffer1
	tilemapData[2].position = se_mem[MENU_PAGE_TILEMAP];
	tilemapData[2].buffer = (void *)tilemapBuffer2;
	tilemapData[2].size = 512;
	
	memcpy32(&characterBuffer4[MENU_PAGE_TEXT_GFX_START << 5], page_name_ui_64x16Tiles, sizeof(page_name_ui_64x16Tiles) >> 2);
	characterData[4].position = tile_mem[MENU_PAGE_TEXT_CHARDATA];
	characterData[4].buffer = (void *)characterBuffer4;
	characterData[4].size = sizeof(characterBuffer4) >> 2;

	//example usage to load the portion of the image starting 6 tile rows down, and 8 tile rows deep.
	//loadGFX(MENU_CHARDATA, MENU_TEXT_GFX_START, menu_actionTiles, MENU_TEXT_TILE_WIDTH * 6, MENU_TEXT_TILE_WIDTH * 8);

	mDat.currMenuPage = 0;
	menuPage = &menuPages[mDat.currMenuPage];

	mDat.menuCursorPos = 0;
	mDat.windowConfirmDirection = MWCD_NEUTRAL;

	mDat.windowCurrTileXPos = 22;
	mDat.windowCurrTileYPos = 10;
	mDat.winSliceWidth = 10;
	mDat.winSliceHeight = 1;
	mDat.windowTargetTileX = 10;
	mDat.windowTargetTileY = 6;
	mDat.windowTargetWidth = 10;
	mDat.windowTargetHeight = 10;
	mDat.windowState = MMWS_INITIAL_ZIPPING;

	mDat.menuBG.xPos = 0;
	mDat.menuBG.yPos = 0;

	mDat.zipSpeed = 3;
	mDat.wrappedAround = true;

	mDat.showPageWindowBG = false;
}

void updateMainMenu(){
	switch(mDat.windowState) {
		default:
		case MMWS_OPENING:
			// Keep expanding the window height until it's the target size it needs to be; re-center as it expands
			if (mDat.winSliceHeight + 2 < mDat.windowTargetHeight) {
				mDat.winSliceHeight+=2;
				//Ensure that this window never goes beyond the screen boundary
				if (mDat.windowCurrTileYPos - 1 >= 0)
					mDat.windowCurrTileYPos--;
			} else {
				mDat.winSliceHeight = mDat.windowTargetHeight;
				//mDat.windowCurrTileYPos = mDat.windowTargetTileY;
				//mDat.winSliceHeight += 2;
				//mDat.windowTileYPos -= 2;
				mDat.updateDraw = true;
				mDat.windowState = MMWS_READY;
			}
			mDat.windowActionTimer++;
			break;
		case MMWS_CLOSING:
			// Keep expanding the window height until it's the target size it needs to be; re-center as it expands
			if (mDat.winSliceHeight + 2 > 1) {
				mDat.winSliceHeight-=2;
				//Ensure that this window never goes beyond the screen boundary
				if (mDat.windowCurrTileYPos + 1 < 19)
					mDat.windowCurrTileYPos++;
			} else {
				mDat.winSliceHeight = 1;//mDat.windowTargetHeight;
				//mDat.winSliceHeight += 2;
				mDat.windowCurrTileYPos -= 2;

				MenuElementData* dat;
				FunctionPtr datFunctPtr;
				int datIntVal;
				int* datIntArr;

				switch(mDat.windowConfirmDirection) {
					default:
					case MWCD_NEUTRAL:
					case MWCD_FORWARD:
						mDat.windowState = MMWS_ZIPPING;
						mDat.zipSpeed = 3;
						
						dat = &menuPage->items[mDat.menuCursorPos].data;
						switch(menuPage->items[mDat.menuCursorPos].dataType) {
							case MPIDT_FUNC_PTR:
								datFunctPtr = dat->functionPtr;
								break;
							case MPIDT_INT:
								datIntVal = dat->intVal;
								break;
							case MPIDT_INT_ARRAY:
								datIntArr = dat->intArray;
								break;
						}

						switch(menuPage->items[mDat.menuCursorPos].menuElement) {
							case ME_SCRIPT_RUNNER:
								break;
							case ME_PAGE_TRANSFER:
								performPageTransfer(datIntVal);
								break;
							case ME_SLIDER:
								break;
							case ME_SHIFT:
								break;
							case ME_TOGGLE:
								break;
							case ME_SOUND_TESTER:
								break;
						}
						break;
					case MWCD_BACKWARD:
						// nvm, maybe if there's extra time i'll implement this
						//mDat.windowState = MMWS_OPENING;
						//mDat.zipSpeed = 0;
						mDat.windowState = MMWS_ZIPPING;
						mDat.zipSpeed = 3;

						// Always perform a Page Transfer for backing out of a menu
						datIntVal = menuPage->backPage;
						performPageTransfer(datIntVal);
						break;
				}

				mDat.windowTargetTileX = menuPage->tileX;
				mDat.windowTargetTileY = menuPage->tileY;
				mDat.windowTargetWidth = menuPage->tileWidth;
				mDat.windowTargetHeight = menuPage->tileHeight;
				mDat.wrappedAround = false;
			}
			break;
		case MMWS_READY:
			menuPage = &menuPages[mDat.currMenuPage];
			directionalInputEnabled();

			if (moveY != 0)
				mDat.updateDraw = true;
			
			// Navigate the menu; wrap around if we hit an edge
			if (menuPage != &menuPages[MPI_CREDITS]) {
				if (moveY != 0) {
					playNewSound(_sfxMenuMove);
					if((mDat.menuCursorPos + moveY) < 0){
						mDat.menuCursorPos = menuPage->itemCount - 1;
					}
					else if ((mDat.menuCursorPos + moveY) > (menuPage->itemCount - 1)){
						mDat.menuCursorPos = 0;
					}
					else{
						mDat.menuCursorPos += moveY;
					}
				}
			
				menuInputConfirmEnabled();
			}
			
			if (menuPage != &menuPages[MPI_MAIN_MENU]) {
				menuInputBackEnabled();
			}
			break;
		case MMWS_INITIAL_ZIPPING:
		case MMWS_ZIPPING:
			// Continue loading the VRAM graphics, if applicable
			loadMenuGraphics(menuPage);

			//mDat.winSliceWidth = 1;
			mDat.windowCurrTileXPos-= mDat.zipSpeed;
			
			if (mDat.windowCurrTileXPos < 0) {
				if (!mDat.wrappedAround) {
					mDat.windowCurrTileXPos = 29;
					mDat.wrappedAround = true;
				} 
			} else {
				if (mDat.wrappedAround) {
					if (mDat.windowCurrTileXPos < mDat.windowTargetTileX) {
						mDat.windowCurrTileXPos = mDat.windowTargetTileX;
						// Set the xPos Offset
						mDat.menuBG.xPos = menuPage->pxOffX;

						mDat.winSliceWidth = mDat.windowTargetWidth;
						mDat.wrappedAround = true;
						mDat.windowActionTimer = 0;
						mDat.windowState = MMWS_OPENING;
					}
				}
			}

			if (mDat.wrappedAround) {

			}

			mDat.windowActionTimer++;
			break;
		case MMWS_TWEAKING_DATA:
			directionalInputEnabled();
			menuInputConfirmEnabled();
			
			menuInputBackEnabled();
			if (moveX != 0) {
				playNewSound(_sfxMenuMove);
				mDat.updateDraw = true;
			}
			break;
	}
}

void loadMenuGraphics(MenuPage *menuPage) {
    static int itemIndex = 0;       // Track the current item index
    static u32 vramTileOffset = 0;  // Track the current VRAM tile offset
    static u32 queueChannel = 0;    // Track the current queue channel
    int numItems = menuPage->itemCount;

    // Process up to 4 graphics load operations per call
    for (int i = 0; i < 4; i+=2) {
        if (itemIndex >= numItems) {
            // All items have been processed, reset for next call
            itemIndex = 0;
            vramTileOffset = 0;
            queueChannel = 0;
            return;
        }

        int gfxPosY = menuPage->items[itemIndex].textGFXIndex;

        // Load unhighlighted tiles
        loadGFX(MENU_CHARDATA, MENU_TEXT_GFX_START + vramTileOffset, (void *)menu_actionTiles,
                MENU_TEXT_TILE_WIDTH * gfxPosY, MENU_TEXT_TILE_WIDTH * 2, queueChannel);
        queueChannel = (queueChannel + 1) % 4;

        // Load highlighted tiles
        loadGFX(MENU_CHARDATA, MENU_TEXT_FOCUSED_GFX_START + vramTileOffset, (void *)menu_action_focusedTiles,
                MENU_TEXT_TILE_WIDTH * gfxPosY, MENU_TEXT_TILE_WIDTH * 2, queueChannel);
        vramTileOffset += MENU_TEXT_TILE_WIDTH * 2;
        queueChannel = (queueChannel + 1) % 4;

        itemIndex++;
    }
}

void drawMainMenu(){
		//Clear the menu tilemap every frame
		memset32(tilemapBuffer1, 0, 512);
		memset32(tilemapBuffer2, 0, 512);
		
		switch(mDat.windowState) {
			default:
			case MMWS_OPENING:
			case MMWS_CLOSING:
			case MMWS_ZIPPING:
			case MMWS_INITIAL_ZIPPING:
				// Draw the Menu Page Window
				int secondaryNineSliceYOff = 2;
				if (mDat.currMenuPage == (int)MPI_CREDITS)
					secondaryNineSliceYOff--;
				if (mDat.showPageWindowBG)
					drawSecondaryNineSliceWindowStyle(10, secondaryNineSliceYOff, 10, 2, 2);

				if (mDat.windowState != MMWS_INITIAL_ZIPPING) {
					
					drawNineSliceWindow(mDat.windowCurrTileXPos, mDat.windowCurrTileYPos, mDat.winSliceWidth, mDat.winSliceHeight, 1);
				} else
					drawLaserRow(mDat.windowCurrTileXPos, mDat.windowCurrTileYPos, mDat.winSliceWidth, 1, false);
				
				drawMenuPageText(10, mDat.menuPageTextYPos + secondaryNineSliceYOff, 0);

				if (mDat.showPageWindowBG) {
					// Draw the Menu Page UI Text now				
					objectBuffer[MENU_PAGE_TEXT_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SHAPE(1) | ATTR0_Y(secondaryNineSliceYOff * 8);
					objectBuffer[MENU_PAGE_TEXT_SPRITE].attr1 = ATTR1_SIZE(3) | ATTR1_X(89);
					objectBuffer[MENU_PAGE_TEXT_SPRITE].attr2 = ATTR2_ID(MENU_PAGE_TEXT_GFX_START + (32 * mDat.currMenuPage)) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_PAGE_TEXT_PAL_START);
				}

				// Queue the tilemap with our drawing functions, using tilemapBuffer1
				tilemapData[1].position = se_mem[MENU_WINDOW_TILEMAP];
				tilemapData[1].buffer = (void *)tilemapBuffer1;
				tilemapData[1].size = 512;

				// Queue the tilemap with our drawing functions, using tilemapBuffer1
				tilemapData[2].position = se_mem[MENU_PAGE_TILEMAP];
				tilemapData[2].buffer = (void *)tilemapBuffer2;
				tilemapData[2].size = 512;
				
				updateObjBuffer();
				break;
			case MMWS_READY:
			case MMWS_TWEAKING_DATA:
				if (!mDat.showPageWindowBG)
					mDat.showPageWindowBG = true;
				
				if (mDat.updateDraw) {
					// Draw the Menu Page Window
					int secondaryNineSliceYOff = 2;
					if (mDat.currMenuPage == (int)MPI_CREDITS)
						secondaryNineSliceYOff--;
					if (mDat.showPageWindowBG)
						drawSecondaryNineSliceWindowStyle(10, secondaryNineSliceYOff, 10, 2, 2);

					if (mDat.windowState != MMWS_INITIAL_ZIPPING) {
						
						drawNineSliceWindow(mDat.windowCurrTileXPos, mDat.windowCurrTileYPos, mDat.winSliceWidth, mDat.winSliceHeight, 1);
					} else
						drawLaserRow(mDat.windowCurrTileXPos, mDat.windowCurrTileYPos, mDat.winSliceWidth, 1, false);
					
					menuPage = &menuPages[mDat.currMenuPage];

					//drawMenuButtons(2,3);
					for(int i = 0; i < menuPage->itemCount; ++i){
						MenuPageItem* thisMenuElement = &menuPage->items[i];
						bool cursorOnElement = (mDat.menuCursorPos == i && (menuPage != &menuPages[MPI_CREDITS]));
						bool isTweakingData = (mDat.windowState == MMWS_TWEAKING_DATA);
						drawMenuTextSegment(mDat.winSliceWidth, mDat.windowCurrTileXPos, mDat.windowCurrTileYPos + 2 + (2 * i), i, 2, cursorOnElement && !isTweakingData, mDat.menuElementsWidth[mDat.currMenuPage]);
					}
					
					if (mDat.showPageWindowBG) {
						// Draw the Menu Page UI Text now				
						objectBuffer[MENU_PAGE_TEXT_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SHAPE(1) | ATTR0_Y(secondaryNineSliceYOff * 8);
						objectBuffer[MENU_PAGE_TEXT_SPRITE].attr1 = ATTR1_SIZE(3) | ATTR1_X(89);
						objectBuffer[MENU_PAGE_TEXT_SPRITE].attr2 = ATTR2_ID(MENU_PAGE_TEXT_GFX_START + (32 * mDat.currMenuPage)) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_PAGE_TEXT_PAL_START);
					}
					
					if (mDat.windowState == MMWS_TWEAKING_DATA) {
						drawSliderPrompt(123, 48 + (mDat.menuCursorPos * 16), MENU_SLIDER_PROMPT_SPRITE1, false);
						drawSliderPrompt(170, 48 + (mDat.menuCursorPos * 16), MENU_SLIDER_PROMPT_SPRITE2, true);
					}

					// Queue the tilemap with our drawing functions, using tilemapBuffer1
					tilemapData[1].position = se_mem[MENU_WINDOW_TILEMAP];
					tilemapData[1].buffer = (void *)tilemapBuffer1;
					tilemapData[1].size = 512;

					// Queue the tilemap with our drawing functions, using tilemapBuffer1
					tilemapData[2].position = se_mem[MENU_PAGE_TILEMAP];
					tilemapData[2].buffer = (void *)tilemapBuffer2;
					tilemapData[2].size = 512;
					mDat.updateDraw = false;
					updateObjBuffer();
				}
				drawMenuButtons(2,3);
				break;
		}
}

void mainMenuEnd(){
	currentScene.scenePointer = sceneList[GAMEPLAY];
	currentScene.state = INITIALIZE;
}

void drawMenuPageText(int xPos, int yPos, int imgIndex) {
	// Each character data block holds 512 tiles, and is selected per background.
	// Sprites get character data blocks 4 and 5.
	// The shooting star takes up the entirety of 4, so I put the rest of the sprites in 5. (that way I can just load them at initialization)
	// The library I used (tonc) addresses VRAM as a 2-d array, with [charblock][tile ID]
	
	//objectBuffer[1].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(yPos);
	//objectBuffer[1].attr1 = ATTR1_SIZE_64 | ATTR1_X(xPos);
	//objectBuffer[1].attr2 = ATTR2_ID(MENU_GFX_START + MIDDLE_UPPER) | ATTR2_PRIO(1) | ATTR2_PALBANK(2);
	
	//(imgIndex << 5) + MENU_PAGE_TEXT_SPRITE
}

void scrollStarryBG(int addedX, int addedY){
	if(currentScene.sceneCounter % 8 <= 0){
		mDat.starryBG.xPos -= addedX;		
	}
	if(currentScene.sceneCounter % 8 <= 0){
		mDat.starryBG.yPos -= addedY;
	}
}

void interpolateStarryBG(){
	// Calculate the interpolation factor t, with proper scaling
	int actionTimerScaled = mDat.starryBG.yScrollTimerCurrent * FIXED_POINT_SCALE;
	int actionTargetScaled = mDat.starryBG.yScrollTimerTarget * FIXED_POINT_SCALE;
	int t = (actionTimerScaled * FIXED_POINT_SCALE) / actionTargetScaled;

	// Apply ease-in-out function
	int easedT = easeInOut(t, 4);

	// Calculate the interpolated position and update yPos
	mDat.starryBG.yPos = lerp(mDat.starryBG.yScrollStartPos * FIXED_POINT_SCALE, mDat.starryBG.yScrollTargetPos * FIXED_POINT_SCALE, easedT) / FIXED_POINT_SCALE;
	
	if (mDat.starryBG.yScrollTimerCurrent < mDat.starryBG.yScrollTimerTarget)
		mDat.starryBG.yScrollTimerCurrent++;
	else
		mDat.starryBG.yScrollTimerCurrent = mDat.starryBG.yScrollTimerTarget;
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
	
	/*
			// Hide the title card after 40 frames in this state
			memset32(tilemapBuffer1, 0, sizeof(sprTitleLogoMap) >> 2);
			tilemapData[1].size = sizeof(sprTitleLogoMap) >> 2;
			tilemapData[1].buffer = tilemapBuffer1;
			tilemapData[1].position = &se_mem[TITLE_CARD_TILEMAP];
	*/
}

/// @brief Draws a nine slice window for the centric main menu; Width and Height params are in terms of 8x8 tiles
/// @param width 
/// @param height 
// Function to handle the correct wrapping and drawing for top-right and bottom-right tiles
void drawNineSliceWindow(int x, int y, int width, int height, int layer) {
    int tilesetIndex = MENU_GFX_START;
    int palette = 2;

    if (height > 1) {
        // Draw the top-middle row
        if (height >= 2) {
            setTile(wrapX(x), y + 1, tilesetIndex + LM_UPPER, false, false, palette, layer); // Left-middle-upper tile
            for (int i = 1; i < width - 1; ++i) {
                setTile(wrapX(x + i), y + 1, tilesetIndex + MIDDLE_UPPER, false, false, palette, layer); // Middle upper
            }
            setTile(wrapX(x + (width - 1)), y + 1, tilesetIndex + RM_UPPER, false, false, palette, layer); // Right-middle-upper tile
        }

        // Calculate the bottomY position
        int bottomY = y + (height - 1);

        // Draw center rows
        for (int j = 2; j < height - 1; ++j) {
            setTile(wrapX(x), y + j, tilesetIndex + LM, false, false, palette, layer); // Left-middle tile
            for (int i = 1; i < width - 1; ++i) {
                setTile(wrapX(x + i), y + j, tilesetIndex + CENTER, false, false, palette, layer); // Center
            }
            setTile(wrapX(x + (width - 1)), y + j, tilesetIndex + RM, false, false, palette, layer); // Right-middle tile
        }

        // Draw the bottom-middle row (mirrored)
        if (height > 4) {
            setTile(wrapX(x), bottomY - 1, tilesetIndex + LM_UPPER, false, true, palette, layer); // Left-middle-lower tile (flipped vertically)
            for (int i = 1; i < width - 1; ++i) {
                setTile(wrapX(x + i), bottomY - 1, tilesetIndex + MIDDLE_UPPER, false, true, palette, layer); // Middle lower (flipped vertically)
            }
            setTile(wrapX(x + (width - 1)), bottomY - 1, tilesetIndex + RM_UPPER, false, true, palette, layer); // Right-middle-lower tile (flipped vertically)
        }

        // Draw the bottom row
        if (width > 2) {
            setTile(wrapX(x + 1), bottomY, tilesetIndex + TL_2, false, true, palette, layer); // Bottom-left corner Part 2 (flipped vertically)
        }
        if (width > 3) {
            setTile(wrapX(x + 2), bottomY, tilesetIndex + TL_3, false, true, palette, layer); // Bottom-left corner Part 3 (flipped vertically)
        }
        for (int i = 3; i < width - 3; ++i) {
            setTile(wrapX(x + i), bottomY, tilesetIndex + TOP_MIDDLE, false, true, palette, layer); // Bottom middle (flipped vertically)
        }
        if (width >= 3) {
            setTile(wrapX(x + (width - 3)), bottomY, tilesetIndex + TR_1, false, true, palette, layer); // Bottom-right corner Part 1 (flipped vertically)
        }
        if (width >= 2) {
            setTile(wrapX(x + (width - 2)), bottomY, tilesetIndex + TR_2, false, true, palette, layer); // Bottom-right corner Part 2 (flipped vertically)
        }

        // Draw the top row
        for (int i = 3; i < width - 3; ++i) {
            setTile(wrapX(x + i), y, tilesetIndex + TOP_MIDDLE, false, false, palette, layer); // Top middle
        }

        if (width >= 3) {
            setTile(wrapX(x + (width - 3)), y, tilesetIndex + TR_1, false, false, palette, layer); // Top-right corner Part 1
        }
        if (width >= 2) {
            setTile(wrapX(x + 1), y, tilesetIndex + TL_2, false, false, palette, layer); // Top-left corner Part 2
        }
        if (width >= 3) {
            setTile(wrapX(x + 2), y, tilesetIndex + TL_3, false, false, palette, layer); // Top-left corner Part 3
        }

        if (width >= 2) {
            setTile(wrapX(x + (width - 2)), y, tilesetIndex + TR_2, false, false, palette, layer); // Top-right corner Part 2
        }

        // Correct top-right and bottom-right corners with wrapping
        setTile(wrapX(x + (width - 1)), bottomY, tilesetIndex + TR_3, false, true, palette, layer); // Bottom-right corner Part 3 (flipped vertically)
        setTile(wrapX(x + (width - 1)), y, tilesetIndex + TR_3, false, false, palette, layer); // Top-right corner Part 3

        setTile(wrapX(x), bottomY, tilesetIndex + TL_1, false, true, palette, layer); // Bottom-left corner Part 1 (flipped vertically)
        setTile(wrapX(x), y, tilesetIndex + TL_1, false, false, palette, layer); // Top-left corner Part 1
    } else {
		drawLaserRow(x, y, width, layer, true);
    }
}

// Helper function to get wrapped x-coordinate
int wrapX(int x) {
    return (x + 30) % 30; // Ensure x is within [0, 29]
}

// Helper function to ensure y is within bounds
bool isInBounds(int y) {
    return y >= 0 && y < 30;
}

// Draw the LASER_TOP and LASER_BOTTOM tiles with wrapping
void drawLaserRow(int x, int y, int width, int layer, bool wrapAround) {
	int tilesetIndex = MENU_GFX_START;
    int palette = 2;
	for (int i = 0; i < width; ++i) {
        int drawX = x + i;

        // Handle wrapping or boundary checking based on the boolean flag
        if (wrapAround) {
            drawX = wrapX(drawX);
        } else if (!isInBounds(drawX)) {
            continue; // Skip drawing if out of bounds and not wrapping
        }

        // Check bounds for y position (should be within the range 0-29)
        if (isInBounds(y) && isInBounds(y + 1)) {
            setTile(drawX, y, tilesetIndex + LASER_TOP, false, false, palette, layer);
            setTile(drawX, y + 1, tilesetIndex + LASER_BOTTOM, false, false, palette, layer);
        }
    }
}

/// @brief Draws a nine slice window for the main menu's Menu Page window; Width and Height params are in terms of 8x8 tiles
/// @param width 
/// @param height 
void drawSecondaryNineSliceWindowStyle(int x, int y, int width, int height, int layer) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int tileX = (x + j) % TILEMAP_WIDTH;
            int tileY = (y + i) % TILEMAP_HEIGHT;
            int tileIndex = MENU_GFX_START + SEC_CENTER; // Default to center tile
            bool flipHorizontal = false;
            bool flipVertical = false;

            // Determine which tile to draw based on position in the nine-slice grid
            if (i == 0 && j == 0) {
                tileIndex = MENU_GFX_START + SEC_TOP_LEFT;
            } else if (i == 0 && j == width - 1) {
                tileIndex = MENU_GFX_START + SEC_TOP_LEFT;
                flipHorizontal = true; // Flip for top-right corner
            } else if (i == 0) {
                tileIndex = MENU_GFX_START + SEC_TOP_MIDDLE;
            } else if (i == height - 1 && j == 0) {
                tileIndex = MENU_GFX_START + SEC_TOP_LEFT;
                flipVertical = true; // Flip for bottom-left corner
            } else if (i == height - 1 && j == width - 1) {
                tileIndex = MENU_GFX_START + SEC_TOP_LEFT;
                flipHorizontal = true; // Flip for bottom-right corner
                flipVertical = true;
            } else if (i == height - 1) {
                tileIndex = MENU_GFX_START + SEC_TOP_MIDDLE;
                flipVertical = true; // Flip for bottom edge
            } else if (j == 0) {
                tileIndex = MENU_GFX_START + SEC_LEFT;
            } else if (j == width - 1) {
                tileIndex = MENU_GFX_START + SEC_LEFT;
                flipHorizontal = true; // Flip for right edge
            }

            setTile(tileX, tileY, tileIndex, flipHorizontal, flipVertical, 2, layer);
        }
    }
}

void drawMenuTextSegment(int nineSliceWidth, int tileXPos, int tileYPos, int menuElementPosition, int palette, bool highlighted, int numTextTileColumns){
	int tilesetIndex;	
	if (!highlighted)
		tilesetIndex = MENU_TEXT_GFX_START;
	else
		tilesetIndex = MENU_TEXT_FOCUSED_GFX_START;
	
	for (int row = 0; row < 2; row++){
		for (int tileXOff = 0; tileXOff < numTextTileColumns; tileXOff++){
			int vramIndex = tilesetIndex + tileXOff + (menuElementPosition * MENU_TEXT_TILE_WIDTH * 2) + (row * MENU_TEXT_TILE_WIDTH);
			setTile(tileXPos + tileXOff, tileYPos + row, vramIndex, false, false, palette, MENU_TEXT_LAYER_ID);
		}
	}
}

void directionalInputEnabled() {
	// Allow Up/Down to navigate the menu; wrap around if we hit the upper/lower limits
	// Combine both up and down input checks into a single operation
	moveX = 0;
	moveY = 0;

	if((inputs.pressed & KEY_UP) && !(inputs.pressed & KEY_DOWN)){
		moveY = -1;
	}
	else if((inputs.pressed & KEY_DOWN) && !(inputs.pressed & KEY_UP)){
		moveY = 1;
	}
	if((inputs.pressed & KEY_RIGHT) && !(inputs.pressed & KEY_LEFT)){
		moveX = -1;
	}
	else if((inputs.pressed & KEY_LEFT) && !(inputs.pressed & KEY_RIGHT)){
		moveX = 1;
	}
}

void menuInputConfirmEnabled() {
	if((inputs.pressed & KEY_A) || (inputs.pressed & KEY_START)){
		switch(mDat.windowState) {
			case MMWS_READY:
				switch(menuPage->items[mDat.menuCursorPos].menuElement) {
					case ME_SCRIPT_RUNNER:
						break;
					case ME_PAGE_TRANSFER:
						playNewSound(_sfxMenuConfirmA);
						mDat.windowConfirmDirection = MWCD_FORWARD;
						mDat.windowState = MMWS_CLOSING;
						mDat.updateDraw = true;
						break;
					case ME_SLIDER:
						playNewSound(_sfxMenuConfirmA);
						mDat.windowState = MMWS_TWEAKING_DATA;
						mDat.updateDraw = true;
						break;
					case ME_SHIFT:
						break;
					case ME_TOGGLE:
						break;
					case ME_SOUND_TESTER:
						break;
				}
				break;
			case MMWS_TWEAKING_DATA:
				switch(menuPage->items[mDat.menuCursorPos].menuElement) {
					case ME_SCRIPT_RUNNER:
						break;
					case ME_PAGE_TRANSFER:
						break;
					case ME_SLIDER:
						playNewSound(_sfxMenuConfirmA);
						hideSliderPrompt();
						mDat.windowState = MMWS_READY;
						mDat.updateDraw = true;
						break;
					case ME_SHIFT:
						break;
					case ME_TOGGLE:
						break;
					case ME_SOUND_TESTER:
						break;
				}
				break;
		}
	}
}

void menuInputBackEnabled() {
	if((inputs.pressed & KEY_B)){
		switch(mDat.windowState) {
			default:
				break;
			case MMWS_READY:
				playNewSound(_sfxMenuCancel);
				mDat.windowConfirmDirection = MWCD_BACKWARD;
				mDat.windowState = MMWS_CLOSING;		
				mDat.updateDraw = true;
				break;
			case MMWS_TWEAKING_DATA:
				playNewSound(_sfxMenuMove);
				hideSliderPrompt();
				mDat.windowState = MMWS_READY;
				mDat.updateDraw = true;
				break;
		}
	}
}

int menuExecNewGame(){
	mDat.state = MAIN_MENU_FLY_OUT;
	mDat.actionTimer = 0;
	mDat.actionTarget = 40; // Specifies the length of the actionTimer
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

void performPageTransfer(int datIntVal) {
	mDat.windowTargetWidth = menuPage->tileWidth;
	mDat.windowTargetHeight = menuPage->tileHeight;

	mDat.menuCursorPos = 0;

	// Get the new Menu Page based on the data we're reading
	mDat.currMenuPage = datIntVal;
	menuPage = &menuPages[mDat.currMenuPage];

	// Start loading the new menu page's graphics into VRAM (this will take more than one frame, so this function will keep being called even during MMWS_ZIPPING state)
	loadMenuGraphics(menuPage);

	// Reset the xPos Offset before we start zipping
	mDat.menuBG.xPos = 0;
}

//example usage to load the portion of the image starting 6 tile rows down, and 8 tile rows deep.
//loadGFX(MENU_CHARDATA, MENU_TEXT_GFX_START, menu_actionTiles, MENU_TEXT_TILE_WIDTH * 6, MENU_TEXT_TILE_WIDTH * 8);

// Limit of 4 queue channels per frame
// Each character data block holds 512 tiles, and is selected per background.
// Sprites get character data blocks 4 and 5.
// The shooting star takes up the entirety of 4, so I put the rest of the sprites in 5. (that way I can just load them at initialization)
// The library I used (tonc) addresses VRAM as a 2-d array, with [charblock][tile ID]
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

void drawMenuButtons(){
	objectBuffer[MENU_BUTTON_PROMPT_SPRITE1].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(18*8);
	objectBuffer[MENU_BUTTON_PROMPT_SPRITE1].attr1 = ATTR1_SIZE_32 | ATTR1_X(26*8);
	objectBuffer[MENU_BUTTON_PROMPT_SPRITE1].attr2 = ATTR2_ID(512 + MENU_BUTTON_PROMPT_GFX_START) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_BUTTON_PROMPT_PAL);
	
	objectBuffer[MENU_BUTTON_PROMPT_SPRITE2].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(18*8);
	objectBuffer[MENU_BUTTON_PROMPT_SPRITE2].attr1 = ATTR1_SIZE_32 | ATTR1_X(0);
	objectBuffer[MENU_BUTTON_PROMPT_SPRITE2].attr2 = ATTR2_ID(512 + MENU_BUTTON_PROMPT_GFX_START + 8) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_BUTTON_PROMPT_PAL);
}

void drawSliderPrompt(int xPos, int yPos, int sprIndex, bool flipSpriteHorizontally){
	objectBuffer[sprIndex].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SHAPE(2) | ATTR0_Y(yPos);
	objectBuffer[sprIndex].attr1 = ATTR1_SIZE(0) | ATTR1_X(xPos) | (flipSpriteHorizontally ? ATTR1_HFLIP : 0x0000);

	objectBuffer[sprIndex].attr2 = ATTR2_ID(512 + MENU_SLIDER_PROMPT_GFX_START) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_BUTTON_PROMPT_PAL);
}

void hideSliderPrompt() {
	objectBuffer[MENU_SLIDER_PROMPT_SPRITE1].attr0 = ATTR0_HIDE;
	objectBuffer[MENU_SLIDER_PROMPT_SPRITE2].attr0 = ATTR0_HIDE;
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
	mDat.state = TITLE_FLY_OUT;
	mDat.actionTimer = 1;
	mDat.actionTarget = 300;
}

void updateObjBuffer(){
	OAMData.position = (void *)oam_mem;
	OAMData.buffer = objectBuffer;
	OAMData.size = sizeof(objectBuffer) >> 2;
}