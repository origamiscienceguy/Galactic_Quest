#include "main_menu.h"
#include <string.h>

MainMenuData mDat = {
	.menuElementsWidth = {10,10,11,9,13,11,10,10},
	.blendMode = 0,
	.eva = 0x81,
	.evb = 0,
	.ey = 0
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
static u8 currentBGMIndex[2] = {0xFF, 0xFF};
static u8 currentSFXIndex[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int moveY = 0, moveX = 0; // Player directional input
MenuPageItem* optionsMenuItems;

// Initialize the Menu Pages
static MenuPage* menuPage;
static MenuPage menuPages[8];

void mainMenuInitialize(){
	REG_DISPCNT = DCNT_MODE0; //black screen
	REG_BG0CNT = BG_4BPP | BG_SBB(STARRY_IMAGE_TILEMAP) | BG_CBB(STARRY_IMAGE_CHARDATA) | BG_PRIO(3) | BG_REG_64x64; //starry background layer
	REG_BG1CNT = BG_4BPP | BG_SBB(TITLE_CARD_TILEMAP) | BG_CBB(TITLE_CARD_CHARDATA) | BG_PRIO(2) | BG_REG_32x32; //title screen layer
	REG_BG2CNT = BG_4BPP | BG_SBB(MENU_WINDOW_TILEMAP) | BG_CBB(MENU_CHARDATA) | BG_PRIO(1) | BG_REG_32x32; //menu page ui layer
	REG_BLDCNT = BLD_TOP(BLD_BG2 | BLD_BACKDROP | BLD_OBJ) | BLD_WHITE;
	REG_BLDY = BLDY(0);

	updateObjBuffer();

	resetMainMenuWindowVariables();
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
	memcpy32(&paletteBufferObj[MENU_BUTTON_PROMPT_PAL << 4], menu_button_prompt_32x16Pal, sizeof(menu_button_prompt_32x16Pal) >> 2);
	memcpy32(&paletteBufferObj[MENU_SLIDER_BARS_PAL << 4], menu_slider_bars_8x16Pal, sizeof(menu_slider_bars_8x16Pal) >> 2);
	
	paletteData[1].size = 40;
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
	memcpy32(&characterBuffer5[MENU_SLIDER_PROMPT_GFX_START << 5], menu_slider_prompt_8x16Tiles, sizeof(menu_slider_prompt_8x16Tiles) >> 2);
	memcpy32(&characterBuffer5[MENU_TOGGLE_PROMPT_GFX_START << 5], menu_toggle_prompt_64x32Tiles, sizeof(menu_toggle_prompt_64x32Tiles) >> 2);
	memcpy32(&characterBuffer5[MENU_BUTTON_PROMPT_GFX_START << 5], menu_button_prompt_32x16Tiles, sizeof(menu_button_prompt_32x16Tiles) >> 2);
	memcpy32(&characterBuffer5[FONT_NUMBERS_GFX_START << 5], font_numbers_8x16Tiles, sizeof(font_numbers_8x16Tiles) >> 2);
	memcpy32(&characterBuffer5[FONT_PERCENT_GFX_START << 5], font_percent_16x16Tiles, sizeof(font_percent_16x16Tiles) >> 2);
	memcpy32(&characterBuffer5[MENU_SLIDER_BARS_GFX_START << 5], menu_slider_bars_8x16Tiles, sizeof(menu_slider_bars_8x16Tiles) >> 2);
	
	characterData[4].position = tile_mem[FLYING_COMET_CHARDATA];
	characterData[4].buffer = (void *)characterBuffer4;
	characterData[4].size = sizeof(characterBuffer4) >> 2;
	characterData[5].position = tile_mem[STAR_BLOCKER_CHARDATA];
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
	mDat.starryBG.scrollTimerCurrent = 0;
	mDat.starryBG.scrollTimerTarget = 32+16+2+53;
	mDat.starryBG.scrollStartPos = TITLE_CAM_PAN_BOTTOM; // Start position (scaled)
	mDat.starryBG.scrollTargetPos = TITLE_CAM_PAN_TOP; // Target position (scaled)

	// Set whatever current values are to the real options struct
	optionsMenuItems = menuPages[MPI_OPTIONS].items;
	optionsMenuItems[OPTID_MASTER_VOL].data.intVal = (int)options.masterVolume;
	optionsMenuItems[OPTID_BGM_VOL].data.intVal = (int)options.bgmVolume;
	optionsMenuItems[OPTID_SFX_VOL].data.intVal = (int)options.sfxVolume;
	optionsMenuItems[OPTID_GRID_ENABLED].data.boolVal = (int)options.gridOn;
}

void mainMenuNormal(){
	//temporary debug input
	if(inputs.pressed & KEY_START){
		// Start the match
		matchBegin();
	}

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

			if (mDat.actionTimer == 16) {
				playBGM(BGM_TITLE);
			}
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
		interpolateStarryBG(true);
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
		interpolateStarryBG(true);
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
		interpolateStarryBG(true);
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
		interpolateStarryBG(true);
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
			hideSpriteRange(PRESS_START_SPRITE1, PRESS_START_SPRITE3);
		}
		// Make the starry background scroll up-left
		scrollStarryBG(-1, -1);

		updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.titleCardBG.xPos, mDat.titleCardBG.yPos);
		
		updateObjBuffer();
		break;
	case TITLE_AFTER_PRESS_START:
		if(mDat.actionTimer >= mDat.actionTarget){
			hideSpriteRange(PRESS_START_SPRITE1, PRESS_START_SPRITE3);
			skipToMenu();
		}else{
			mDat.actionTimer++;
		}

		// Rapidly blink the "Press Start" graphic
		if(mDat.actionTimer % 16 >= 8){
			drawPressStart();
		}else{
			hideSpriteRange(PRESS_START_SPRITE1, PRESS_START_SPRITE3);
		}

		updateObjBuffer();
		break;
	case TITLE_FLY_OUT:
		if(mDat.actionTimer >= mDat.actionTarget){
			mDat.state = MAIN_MENU_FLY_IN;
			
			playBGM(BGM_MAINMENU);

			mDat.menuBG.xPos = 512 - 2;
			mDat.menuBG.yPos = 0;

			//Clear the menu tilemaps
			memset32(tilemapBuffer1, 0, 512);
			memset32(tilemapBuffer2, 0, 512);
			
			// Queue the tilemap with our drawing functions, using tilemapBuffer1
			tilemapData[1].position = se_mem[MENU_WINDOW_TILEMAP];
			tilemapData[1].buffer = (void *)tilemapBuffer1;
			tilemapData[1].size = 512;

			// Queue the tilemap with our drawing functions, using tilemapBuffer2
			tilemapData[2].position = se_mem[MENU_PAGE_TILEMAP];
			tilemapData[2].buffer = (void *)tilemapBuffer2;
			tilemapData[2].size = 512;

			loadGFX(MENU_CHARDATA, MENU_TEXT_GFX_START, (void *)menu_actionTiles, MENU_TEXT_TILE_WIDTH * 0, MENU_TEXT_TILE_WIDTH * 6, 0);
			loadGFX(MENU_CHARDATA, MENU_TEXT_FOCUSED_GFX_START, (void *)menu_action_focusedTiles, MENU_TEXT_TILE_WIDTH * 0, MENU_TEXT_TILE_WIDTH * 6, 1);
		}else{
			// Make the starry background scroll up, *very* quickly. Use quadratic interpolation			
			/*
				// Calculate the interpolation factor t, with proper scaling
				int actionTimerScaled = mDat.actionTimer * FIXED_POINT_SCALE;
				int actionTargetScaled = mDat.actionTarget * FIXED_POINT_SCALE;
				int t = (actionTimerScaled * FIXED_POINT_SCALE) / actionTargetScaled;
				int t2 = t * 8;

				// Apply ease-in-out function
				int easedT = easeOutQuint(t);
				int easedT2 = easeInOut(t2, 4);
				// Calculate the interpolated position and update yPos
				mDat.starryBG.yPos = lerp(starryBGYPosInit, starryBGYPosTarget, easedT) / FIXED_POINT_SCALE;
				mDat.titleCardBG.yPos = lerp(titleCardYStart, titleCardYTarget, easedT2) / FIXED_POINT_SCALE;
			*/
			mDat.starryBG.yPos = starryBGPanYPos[mDat.actionTimer];
			mDat.titleCardBG.yPos = titleCardBGPanYPos[clamp(mDat.actionTimer, 0, 9)];
			mDat.actionTimer++;
		}
		
		updateObjBuffer();
		
		if (mDat.actionTimer > 12){
			// Hide the title card after 12 frames in this state
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
		if(mDat.actionTimer >= mDat.actionTarget) {
			mDat.actionTimer = 0;
			mDat.actionTarget = 0;
			matchBegin();
		} else {
			mDat.actionTimer++;
		}
				
		/// Perform the fadeout wipe transition
		static int lastQuadrantWrittenTo = -1;
		const int BASE_ACTION_TIMER = 103;

		switch (mDat.actionTimer) {
			case BASE_ACTION_TIMER: {
				// Snap to the next quadrant
				int currentQuadrant = (mDat.starryBG.xPos / TILEMAP_QUADRANT_SIZE) % 2;
				int nextQuadrant = (currentQuadrant + 1) % 2;

				// Snap the starry BG's x position to the next quadrant
				mDat.starryBG.xPos = nextQuadrant * TILEMAP_QUADRANT_SIZE;
				mDat.starryBG.scrollStartPos = nextQuadrant * TILEMAP_QUADRANT_SIZE;
				mDat.starryBG.snappedThisFrame = true;

				// Clear the entire current quadrant
				tilemapData[0].position = se_mem[STARRY_IMAGE_TILEMAP + currentQuadrant];
				tilemapData[0].buffer = (void *)tilemapBuffer0;
				tilemapData[0].size = 512;

				// Update the last quadrant written to
				lastQuadrantWrittenTo = currentQuadrant;
				break;
			}
			case BASE_ACTION_TIMER + 12: {
				// Clear the opposite quadrant
				int oppositeQuadrant = (lastQuadrantWrittenTo + 1) % 2;
				tilemapData[0].position = se_mem[STARRY_IMAGE_TILEMAP + oppositeQuadrant];
				tilemapData[0].buffer = (void *)tilemapBuffer0;
				tilemapData[0].size = 512;
				break;
			}
			case BASE_ACTION_TIMER + 13:
			case BASE_ACTION_TIMER + 14: {
				// Clear quadrants 3 and 4 sequentially
				int quadrantToClear = mDat.actionTimer - (BASE_ACTION_TIMER + 11);
				tilemapData[0].position = se_mem[STARRY_IMAGE_TILEMAP + quadrantToClear];
				tilemapData[0].buffer = (void *)tilemapBuffer0;
				tilemapData[0].size = 512;
				break;
			}
			default:
				break;
		}

		// Make the starry background scroll up-left
		//scrollStarryBG(-1, 0);
		interpolateStarryBG(false);

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
			memset32(tilemapBuffer1, 0, sizeof(tilemapBuffer1) >> 2);
			tilemapData[1].size = sizeof(tilemapBuffer1) >> 2;
			tilemapData[1].buffer = tilemapBuffer1;
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

			// End the current BGM
			endCurrentBGM();

			playSFX(_sfxMenuConfirmC, AUDGROUP_MENUSFX);

			//starryBGYPosInit = mDat.starryBG.yPos * FIXED_POINT_SCALE; // Start position (scaled)
			//starryBGYPosTarget = -4504 * FIXED_POINT_SCALE; // Target position (scaled)
			//starryBGYPosTarget = -2004 * FIXED_POINT_SCALE; // Target position (scaled)
			//titleCardYStart = mDat.titleCardBG.yPos * FIXED_POINT_SCALE;
			//titleCardYTarget = (mDat.titleCardBG.yPos + 140) * FIXED_POINT_SCALE;

			updateBGScrollRegisters(mDat.starryBG.xPos, mDat.starryBG.yPos, mDat.titleCardBG.xPos, mDat.titleCardBG.yPos);
		}
	}
}

void initMainMenu(){
	// Initialize the menu pages
	initMenuPages(menuPages);

	mainMenuInitBlend();
	//set the active screen layers
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
//	REG_BG0CNT = BG_4BPP | BG_SBB(STARRY_IMAGE_TILEMAP) | BG_CBB(STARRY_IMAGE_CHARDATA) | BG_PRIO(3) | BG_REG_64x64; //starry background layer
	REG_BG1CNT = BG_4BPP | BG_SBB(MENU_WINDOW_TILEMAP) | BG_CBB(MENU_CHARDATA) | BG_PRIO(2) | BG_REG_32x32; //title screen layer
	REG_BG2CNT = BG_4BPP | BG_SBB(MENU_PAGE_TILEMAP) | BG_CBB(MENU_CHARDATA) | BG_PRIO(1) | BG_REG_32x32; //menu page ui layer

	//Clear all of the tilemaps
	memset32(tilemapBuffer0, 0, sizeof(tilemapBuffer0) >> 2);
	memset32(tilemapBuffer1, 0, sizeof(tilemapBuffer1) >> 2);
	memset32(tilemapBuffer2, 0, sizeof(tilemapBuffer2) >> 2);

	//send the starry BG tilemap
	tilemapData[0].position = &se_mem[STARRY_IMAGE_TILEMAP];
	tilemapData[0].buffer = (void *)main_menu_starfieldMetaTiles;
	tilemapData[0].size = sizeof(main_menu_starfieldMetaTiles) >> 2;

	// Queue the tilemap with our drawing functions, using tilemapBuffer1
	tilemapData[1].position = se_mem[MENU_WINDOW_TILEMAP];
	tilemapData[1].buffer = (void *)tilemapBuffer1;
	tilemapData[1].size = 512;

	// Queue the tilemap with our drawing functions, using tilemapBuffer2
	tilemapData[2].position = se_mem[MENU_PAGE_TILEMAP];
	tilemapData[2].buffer = (void *)tilemapBuffer2;
	tilemapData[2].size = 512;
	
	memcpy32(&characterBuffer4[MENU_PAGE_TEXT_GFX_START << 5], page_name_ui_64x16Tiles, sizeof(page_name_ui_64x16Tiles) >> 2);
	characterData[4].position = tile_mem[MENU_PAGE_TEXT_CHARDATA];
	characterData[4].buffer = (void *)characterBuffer4;
	characterData[4].size = sizeof(characterBuffer4) >> 2;

	//example usage to load the portion of the image starting 6 tile rows down, and 8 tile rows deep.
	//loadGFX(MENU_CHARDATA, MENU_TEXT_GFX_START, menu_actionTiles, MENU_TEXT_TILE_WIDTH * 6, MENU_TEXT_TILE_WIDTH * 8);

	resetMainMenuWindowVariables();
	updateOptions();

	// Draw the Menu Page UI Text now
	drawMenuPageUIText();
}

void updateMainMenu(){
	switch(mDat.windowState){
		default:
		case MMWS_DONE:
			break;
		case MMWS_APPLIED_OPTIONS:
			if (mDat.windowActionTimer >= mDat.windowActionTarget) {
				// Send the player back to the previous menu
				mDat.windowConfirmDirection = MWCD_BACKWARD;
				mDat.windowState = MMWS_CLOSING;
				
				// If there are any menu toggles/sliders/cursors visible, hide them right now
				hideSpriteRange(MENU_SLIDER_PROMPT_SPRITE1, FONT_PERCENT_SPRITE_LAST);

				mDat.updateSpriteDraw = true;

				// Lerp toward making the text completely visible, over 32 frames
				mDat.evaLerpStart = 16;
				mDat.evaLerpEnd = 0;
				mDat.evbLerpStart = 0;
				mDat.evbLerpEnd = 16;
				mDat.windowActionTimer = 0;
				mDat.windowActionTarget = 8;

				mDat.hideMenuCursor = true;
			} else
				mDat.windowActionTimer++;
	
			if (mDat.windowActionTimer % 8 == 0) {
				mDat.hideMenuCursor = !mDat.hideMenuCursor;
				mDat.updateUITileDraw = true;
			}
			mDat.updateSpriteDraw = true;
			break;
		case MMWS_OPENING:
			// Keep expanding the window height until it's the target size it needs to be; re-center as it expands
			if (mDat.winSliceHeight + 2 < mDat.windowTargetHeight){
				mDat.winSliceHeight+=2;
				//Ensure that this window never goes beyond the screen boundary
				if (mDat.windowCurrTileYPos - 1 >= 0)
					mDat.windowCurrTileYPos--;
			}else{
				// Set the window 9-slice height to the target window height (defined as an array at mDat.menuElementsWidth[<menu page>])
				mDat.winSliceHeight = mDat.windowTargetHeight;

				// Enable drawing sprites and the UI tiles for this frame
				mDat.updateSpriteDraw = true;
				mDat.updateUITileDraw = true;
				mDat.windowState = MMWS_READY;

				// Lerp toward making the text completely visible, over 32 frames
				mDat.evaLerpStart = 0;
				mDat.evaLerpEnd = 16;
				mDat.evbLerpStart = 16;
				mDat.evbLerpEnd = 0;
				mDat.windowActionTimer = 0;
				mDat.windowActionTarget = 8;
			}

			// Allow the background to draw tiles every frame that this state is active
			mDat.updateBGTileDraw = true;
			break;
		case MMWS_CLOSING:
			// Keep expanding the window height until it's the target size it needs to be; re-center as it expands
			if (mDat.winSliceHeight + 2 > 1){
				mDat.winSliceHeight-=2;
				//Ensure that this window never goes beyond the screen boundary
				if (mDat.windowCurrTileYPos + 1 < 19)
					mDat.windowCurrTileYPos++;
			}else{
				mDat.winSliceHeight = 1;//mDat.windowTargetHeight;
				//mDat.winSliceHeight += 2;
					
				mDat.windowCurrTileYPos -= 2;

				if (mDat.windowFinalizing)
					mDat.windowState = MMWS_ZIPPING_OUT;
				else
					mDat.windowState = MMWS_ZIPPING;
				
				mDat.zipSpeed = 3;
				mDat.wrappedAround = false;
			}
			mDat.updateBGTileDraw = true;
			break;
		case MMWS_FINALIZING:
			if (mDat.windowActionTimer >= mDat.windowActionTarget) {
				mDat.windowState = MMWS_CLOSING;
				mDat.windowFinalizing = true;
				mDat.windowActionTimer = 0;
				mDat.windowActionTarget = 8;
			} else {
				mDat.windowActionTimer++;
			}
			
			if (mDat.windowActionTimer % 8 == 0) {
				mDat.hideMenuCursor = !mDat.hideMenuCursor;
				mDat.updateUITileDraw = true;
			}
			break;
		case MMWS_READY:
			menuPage = &menuPages[mDat.currMenuPage];
			directionalInputEnabled();

			if (moveY != 0) {
				// Hide the slider prompt
				hideSpriteRange(MENU_SLIDER_PROMPT_SPRITE1, MENU_SLIDER_PROMPT_SPRITE2);
				mDat.updateSpriteDraw = true;
				mDat.updateUITileDraw = true;
			}

			// Navigate the menu; wrap around if we hit an edge
			if (menuPage != &menuPages[MPI_CREDITS]){
				if (moveY != 0){
					playSFX(_sfxMenuMove, AUDGROUP_MENUSFX);
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
			
				if (moveX != 0){
					MenuPageItem* thisMenuItem = &menuPage->items[mDat.menuCursorPos];
					switch(thisMenuItem->menuElement) {
						default:
							break;
						case ME_TOGGLE:
							playSFX(_sfxMenuConfirmA, AUDGROUP_MENUSFX);
							thisMenuItem->data.boolVal = !thisMenuItem->data.boolVal;
							mDat.updateSpriteDraw = true;
							break;
						case ME_SOUND_TESTER:
							thisMenuItem->data.intVal += moveX;
							switch(thisMenuItem->id){
								default:
									break;
								case MID_SOUND_TEST_BGM:
									if (thisMenuItem->data.intVal < 0)
										thisMenuItem->data.intVal = SOUND_TEST_BGM_COUNT - 1;
									else if (thisMenuItem->data.intVal > SOUND_TEST_BGM_COUNT - 1)
										thisMenuItem->data.intVal = 0;
									mDat.updateSpriteDraw = true;
									break;
								case MID_SOUND_TEST_SFX:
									if (thisMenuItem->data.intVal < 0)
										thisMenuItem->data.intVal = SOUND_TEST_SFX_COUNT - 1;
									else if (thisMenuItem->data.intVal >= SOUND_TEST_SFX_COUNT)
										thisMenuItem->data.intVal = 0;
									mDat.updateSpriteDraw = true;
									break;
							}
							playSFX(_sfxMenuMove, AUDGROUP_MENUSFX);
							break;
					}
				}

				// Allow R to stop the sound playing on the cursor option
				if (mDat.currMenuPage == MPI_SOUND_TEST) {
					if (inputs.pressed & KEY_R){
						MenuPageItem* thisMenuItem = &menuPage->items[mDat.menuCursorPos];
						switch(thisMenuItem->menuElement) {
							default:
								break;
							case ME_PAGE_TRANSFER:
							case ME_SOUND_TESTER:
								switch(thisMenuItem->id){
									default:
									case MID_SOUND_TEST_CANCEL:
										break;
									case MID_SOUND_TEST_BGM:
										// End the current BGM
										endCurrentBGM();
										break;
									case MID_SOUND_TEST_SFX:
										stopSFX(1);
										stopSFX(2);
										break;
								}
								break;
						}
					} else if (inputs.pressed & KEY_L){
						endAllSound();
					}
				}

				menuInputConfirmEnabled();
			}
			
			if (menuPage != &menuPages[MPI_MAIN_MENU]){
				menuInputCancelEnabled();
			}
			break;
		case MMWS_INITIAL_ZIPPING:
		case MMWS_ZIPPING:
		case MMWS_ZIPPING_OUT:
			if (mDat.windowState != MMWS_ZIPPING_OUT) {
				// Continue loading the VRAM graphics, if applicable
				loadMenuGraphics(menuPage);
			}

			//mDat.winSliceWidth = 1;
			mDat.windowCurrTileXPos-= mDat.zipSpeed;
			
			if (mDat.windowState != MMWS_ZIPPING_OUT) {
				if (mDat.windowCurrTileXPos < 0){
					if (!mDat.wrappedAround){
						mDat.windowCurrTileXPos = 29;
						mDat.wrappedAround = true;

						
						// If this is a Page Transfer, load the new menu page now
						MenuElementData* dat;
						//FunctionPtr datFunctPtr;
						int datIntVal = 0;
						//int* datIntArr;

						switch(mDat.windowConfirmDirection){
							default:
							case MWCD_NEUTRAL:
							case MWCD_FORWARD:
								dat = &menuPage->items[mDat.menuCursorPos].data;
								switch(menuPage->items[mDat.menuCursorPos].dataType){
									default:
									case MPIDT_FUNC_PTR:
										//datFunctPtr = dat->functionPtr;
										break;
									case MPIDT_INT:
										datIntVal = dat->intVal;
										break;
									case MPIDT_INT_ARRAY:
										//datIntArr = dat->intArray;
										break;
									case MPIDT_BOOL:
										//datIntArr = dat->intArray;
										break;
								}

								switch(menuPage->items[mDat.menuCursorPos].menuElement){
									default:
										break;
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
									case ME_CREDITS_DISPLAY:
										break;
								}
								break;
							case MWCD_BACKWARD:
								// nvm, maybe if there's extra time i'll implement this
								//mDat.windowState = MMWS_OPENING;
								//mDat.zipSpeed = 0;
								//mDat.windowActionTimer = 0;

								// Always perform a Page Transfer for backing out of a menu
								datIntVal = menuPage->backPage;
								performPageTransfer(datIntVal);
								break;
						}
						
						mDat.windowTargetTileX = menuPage->tileX;
						mDat.windowTargetTileY = menuPage->tileY;
						mDat.windowTargetWidth = menuPage->tileWidth;
						mDat.windowTargetHeight = menuPage->tileHeight;

						// Hide the Pade UI Text Sprite
						hideSprite(MENU_PAGE_TEXT_SPRITE);
					} 
				}else{
					if (mDat.wrappedAround){
						if (mDat.windowCurrTileXPos < mDat.windowTargetTileX){
							mDat.windowCurrTileXPos = mDat.windowTargetTileX;
							// Set the xPos Offset
							mDat.menuBG.xPos = menuPage->pxOffX;

							mDat.winSliceWidth = mDat.windowTargetWidth;
							mDat.wrappedAround = true;
							mDat.windowState = MMWS_OPENING;
						}
					}
				}
			} else {
				if (mDat.windowCurrTileXPos < -mDat.winSliceWidth - 1){
					playSFX(_sfxScreenPan, AUDGROUP_MENUSFX);		
					mDat.state = MAIN_MENU_FLY_OUT;
					mDat.windowState = MMWS_DONE;
					mDat.actionTimer = 0;
					mDat.actionTarget = 240;
					mDat.starryBG.scrollStartPos = mDat.starryBG.xPos;
					mDat.starryBG.scrollTargetPos = 6000;
					mDat.starryBG.scrollTimerCurrent = 0;
					mDat.starryBG.scrollTimerTarget = 200;
				}
			}

			mDat.updateBGTileDraw = true;
			mDat.windowActionTimer++;
			break;
		case MMWS_TWEAKING_DATA:
			directionalInputEnabled();
			menuInputConfirmEnabled();
			
			menuInputCancelEnabled();
			
			if (moveX != 0){
				MenuPageItem* thisMenuItem = &menuPage->items[mDat.menuCursorPos];
				switch(thisMenuItem->menuElement) {
					default:
						break;
					case ME_SLIDER:
						updateSoundVolumes(false);
						if (!(thisMenuItem->data.intVal == 0 && moveX < 0) && !(thisMenuItem->data.intVal == MAX_VOLUME && moveX > 0))
							playSFX(_sfxMenuMove, AUDGROUP_MENUSFX);
						thisMenuItem->data.intVal = clamp(thisMenuItem->data.intVal + moveX, 0, MAX_VOLUME + 1);
						mDat.updateSpriteDraw = true;
						break;
				}
			}
			break;
	}

	// Update the main menu sprite blending every frame, unless we're leaving the Main Menu
	if (mDat.windowState != MMWS_FINALIZING && mDat.windowState != MMWS_APPLIED_OPTIONS) {
		mDat.eva = interpolateValues(mDat.windowActionTimer, mDat.windowActionTarget, mDat.evaLerpStart, mDat.evaLerpEnd);
		mDat.evb = interpolateValues(mDat.windowActionTimer, mDat.windowActionTarget, mDat.evbLerpStart, mDat.evbLerpEnd);
		
		mainMenuUpdateBlend(mDat.eva, mDat.evb);
	}

	// Ensure the timer is always moving, if it's not at its target yet
	if (mDat.windowActionTimer < mDat.windowActionTarget){
		mDat.windowActionTimer++;
	}else{
		mDat.windowActionTimer = mDat.windowActionTarget;
	}
	updateObjBuffer();
}

void loadMenuGraphics(MenuPage *menuPage){
    static int itemIndex = 0;       // Track the current item index
    static u32 vramTileOffset = 0;  // Track the current VRAM tile offset
    static u32 queueChannel = 0;    // Track the current queue channel
    int numItems = menuPage->itemCount;

    // Process up to 4 graphics load operations per call
    for (int i = 0; i < 4; i+=2){
        if (itemIndex >= numItems){
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
	switch(mDat.windowState){
		default:
		case MMWS_DONE:
			break;
		case MMWS_OPENING:
		case MMWS_CLOSING:
		case MMWS_ZIPPING:
		case MMWS_INITIAL_ZIPPING:
		case MMWS_ZIPPING_OUT:
			//Clear the menu tilemap every frame
			memset32(tilemapBuffer1, 0, 512);
			memset32(tilemapBuffer2, 0, 512);

			// Draw the Menu Page Window
			if (mDat.showPageWindowBG) {
				if (mDat.windowFinalizing) {
					if (mDat.eva != mDat.evaLerpEnd)
						drawSecondaryNineSliceWindowStyle(10, mDat.secondaryNineSliceYOff, 10, 2, 2);
				} else
					drawSecondaryNineSliceWindowStyle(10, mDat.secondaryNineSliceYOff, 10, 2, 2);
			}

			if (mDat.windowState != MMWS_INITIAL_ZIPPING && mDat.windowState != MMWS_ZIPPING_OUT){
				drawNineSliceWindow(mDat.windowCurrTileXPos, mDat.windowCurrTileYPos, mDat.winSliceWidth, mDat.winSliceHeight, 1);
			}else
				drawLaserRow(mDat.windowCurrTileXPos, mDat.windowCurrTileYPos, mDat.winSliceWidth, 1, false);
			
			// Queue the tilemap with our drawing functions, using tilemapBuffer1
			tilemapData[1].position = se_mem[MENU_WINDOW_TILEMAP];
			tilemapData[1].buffer = (void *)tilemapBuffer1;
			tilemapData[1].size = 512;

			// Queue the tilemap with our drawing functions, using tilemapBuffer1
			tilemapData[2].position = se_mem[MENU_PAGE_TILEMAP];
			tilemapData[2].buffer = (void *)tilemapBuffer2;
			tilemapData[2].size = 512;
			
			if (!mDat.windowFinalizing)
				drawMenuButtonPrompts(true);
			else
				hideSpriteRange(MENU_BUTTON_PROMPT_SPRITE_FIRST, MENU_BUTTON_PROMPT_SPRITE_LAST);
			break;
		case MMWS_READY:
		case MMWS_TWEAKING_DATA:
		case MMWS_FINALIZING:
		case MMWS_APPLIED_OPTIONS:
			if (!mDat.showPageWindowBG)
				mDat.showPageWindowBG = true;
			
			if (mDat.updateBGTileDraw){
				//Clear the menu tilemap
				memset32(tilemapBuffer1, 0, 512);
				memset32(tilemapBuffer2, 0, 512);

				// Draw the Menu Page Window
				if (mDat.showPageWindowBG)
					drawSecondaryNineSliceWindowStyle(10, mDat.secondaryNineSliceYOff, 10, 2, 2);

				if (mDat.windowState != MMWS_INITIAL_ZIPPING){
					
					drawNineSliceWindow(mDat.windowCurrTileXPos, mDat.windowCurrTileYPos, mDat.winSliceWidth, mDat.winSliceHeight, 1);
				}else
					drawLaserRow(mDat.windowCurrTileXPos, mDat.windowCurrTileYPos, mDat.winSliceWidth, 1, false);
				
				menuPage = &menuPages[mDat.currMenuPage];
				mDat.updateBGTileDraw = false;
			}
			if (mDat.updateSpriteDraw){
				// First, hide all of these number/percent sprites from the previous frame
				hideSpriteRange(FONT_NUMBERS_SPRITE_FIRST, FONT_PERCENT_SPRITE_LAST);
				
				u8 numDrawnSliders = 0;
				u8 numDrawnToggles = 0;
				u8 numDrawnDigits = 0;
				u8 numDrawnPercentSigns = 0;
				for(int i = 0; i < menuPage->itemCount; ++i){
					MenuPageItem* thisMenuItem = &menuPage->items[i];
					bool cursorOnElement = (mDat.menuCursorPos == i && menuPage != &menuPages[MPI_CREDITS] && !mDat.hideMenuCursor);
					bool isTweakingData = (mDat.windowState == MMWS_TWEAKING_DATA);
					switch(thisMenuItem->menuElement) {
						default:
							break;
						case ME_SLIDER:
							int slBarX = ((mDat.windowCurrTileXPos + mDat.menuElementsWidth[mDat.currMenuPage]) * TILE_SIZE) + 3;
							int slBarY = ((mDat.windowCurrTileYPos + 2) * TILE_SIZE) + (i * TILE_SIZE * 2) + 2;
							drawSliderBar(numDrawnSliders, slBarX, slBarY, 0, thisMenuItem->data.intVal);
							numDrawnSliders++;
							numDrawnDigits += drawNumber(FONT_NUMBERS_SPRITE_FIRST + (i * 3), thisMenuItem->data.intVal * 10, slBarX + (8 * TILE_SIZE) + 2, slBarY + 1, true);
							numDrawnPercentSigns += drawPercent(FONT_PERCENT_SPRITE_FIRST + i, slBarX + (8 * TILE_SIZE) - 3, slBarY + 1);
							break;
						case ME_TOGGLE:
							int togglePosX = ((mDat.windowCurrTileXPos + mDat.menuElementsWidth[mDat.currMenuPage]) * TILE_SIZE) + 4;
							int togglePosY = ((mDat.windowCurrTileYPos + 2) * TILE_SIZE) + (i * TILE_SIZE * 2) + 1;
							drawToggle(numDrawnToggles, togglePosX, togglePosY, thisMenuItem->data.boolVal);
							numDrawnToggles++;

							if (cursorOnElement){
								drawSliderPrompt(123, 48 + (mDat.menuCursorPos * 16), MENU_SLIDER_PROMPT_SPRITE1, false);
								drawSliderPrompt(170, 48 + (mDat.menuCursorPos * 16), MENU_SLIDER_PROMPT_SPRITE2, true);
							}
							break;
						case ME_SOUND_TESTER:
							int posX = ((mDat.windowCurrTileXPos + mDat.menuElementsWidth[mDat.currMenuPage] - 5) * TILE_SIZE) + 1;
							int posY = ((mDat.windowCurrTileYPos + 2) * TILE_SIZE) + (i * TILE_SIZE * 2) + 2;
							numDrawnDigits += drawNumber(FONT_NUMBERS_SPRITE_FIRST + (i * 3), thisMenuItem->data.intVal, posX + (8 * TILE_SIZE) + 2, posY + 1, true);

							if (cursorOnElement){
								drawSliderPrompt(123, 48 + 16 + (mDat.menuCursorPos * 16), MENU_SLIDER_PROMPT_SPRITE1, false);
								drawSliderPrompt(170, 48 + 16 + (mDat.menuCursorPos * 16), MENU_SLIDER_PROMPT_SPRITE2, true);
							}
							break;
					}

				}
				
				if (mDat.windowState == MMWS_TWEAKING_DATA){
					drawSliderPrompt(123, 48 + (mDat.menuCursorPos * 16), MENU_SLIDER_PROMPT_SPRITE1, false);
					drawSliderPrompt(170, 48 + (mDat.menuCursorPos * 16), MENU_SLIDER_PROMPT_SPRITE2, true);
				}

				mDat.updateSpriteDraw = false;
			}
			
			// Draw the tiles that draw all of the text on the left half of the main menu, if we're allowed to update them on this frame
			if (mDat.updateUITileDraw) {
				for(int i = 0; i < menuPage->itemCount; ++i){
					MenuPageItem* thisMenuItem = &menuPage->items[i];
					bool cursorOnElement = (mDat.menuCursorPos == i && menuPage != &menuPages[MPI_CREDITS] && !mDat.hideMenuCursor);
					bool isTweakingData = (mDat.windowState == MMWS_TWEAKING_DATA);
					drawMenuTextSegment(mDat.windowCurrTileXPos, mDat.windowCurrTileYPos + 2 + (2 * i), i, 2, cursorOnElement && !isTweakingData, mDat.menuElementsWidth[mDat.currMenuPage]);
				}
			}
			mDat.updateUITileDraw = false;

			// Queue the tilemap with our drawing functions, using tilemapBuffer1
			tilemapData[1].position = se_mem[MENU_WINDOW_TILEMAP];
			tilemapData[1].buffer = (void *)tilemapBuffer1;
			tilemapData[1].size = 512;

			// Queue the tilemap with our drawing functions, using tilemapBuffer2
			tilemapData[2].position = se_mem[MENU_PAGE_TILEMAP];
			tilemapData[2].buffer = (void *)tilemapBuffer2;
			tilemapData[2].size = 512;

			drawMenuButtonPrompts(false);
			break;
	}
	
	// Draw the Menu Page UI Text now, if applicable
	if (mDat.windowState == MMWS_CLOSING && mDat.windowFinalizing) {
		if (mDat.eva > mDat.evaLerpEnd)
			drawMenuPageUIText();
		else
			hideSprite(MENU_PAGE_TEXT_SPRITE);
		updateObjBuffer();
	} else
		drawMenuPageUIText();
}

void mainMenuEnd(){
	currentScene.scenePointer = sceneList[GAMEPLAY];
	currentScene.state = INITIALIZE;
}

void scrollStarryBG(int addedX, int addedY){
	if(currentScene.sceneCounter % 8 <= 0){
		mDat.starryBG.xPos -= addedX;		
	}
	if(currentScene.sceneCounter % 8 <= 0){
		mDat.starryBG.yPos -= addedY;
	}
}

void interpolateStarryBG(bool scrollYAxis){
	// Calculate the interpolation factor t, with proper scaling
	int actionTimerScaled = mDat.starryBG.scrollTimerCurrent * FIXED_POINT_SCALE;
	int actionTargetScaled = mDat.starryBG.scrollTimerTarget * FIXED_POINT_SCALE;
	int t = (actionTimerScaled * FIXED_POINT_SCALE) / actionTargetScaled;

	// Apply ease-in-out function
	int easedT = t;
	
	easedT = easeInOut(t, 4);

	// Calculate the interpolated position and update yPos (or xPos)
	if (scrollYAxis)
		mDat.starryBG.yPos = lerp(mDat.starryBG.scrollStartPos * FIXED_POINT_SCALE, mDat.starryBG.scrollTargetPos * FIXED_POINT_SCALE, easedT) / FIXED_POINT_SCALE;
	else if (!mDat.starryBG.snappedThisFrame){
		mDat.starryBG.xPos = lerp(mDat.starryBG.scrollStartPos * FIXED_POINT_SCALE, mDat.starryBG.scrollTargetPos * FIXED_POINT_SCALE, easedT) / FIXED_POINT_SCALE;
	} else {
		mDat.starryBG.xPos += 19; // sorry for the spaghetti lol
	}

	if (mDat.starryBG.scrollTimerCurrent < mDat.starryBG.scrollTimerTarget)
		mDat.starryBG.scrollTimerCurrent++;
	else
		mDat.starryBG.scrollTimerCurrent = mDat.starryBG.scrollTimerTarget;
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
// Function to handle the correct wrapping and drawing for top-right and bottom-right tiles
void drawNineSliceWindow(int x, int y, int width, int height, int layer){
    int tilesetIndex = MENU_GFX_START;
    int palette = 2;

    if (height > 1){
        // Draw the top-middle row
        if (height >= 2){
            setTile(wrapTileX(x), y + 1, tilesetIndex + LM_UPPER, false, false, palette, layer); // Left-middle-upper tile
            for (int i = 1; i < width - 1; ++i){
                setTile(wrapTileX(x + i), y + 1, tilesetIndex + MIDDLE_UPPER, false, false, palette, layer); // Middle upper
            }
            setTile(wrapTileX(x + (width - 1)), y + 1, tilesetIndex + RM_UPPER, false, false, palette, layer); // Right-middle-upper tile
        }

        // Calculate the bottomY position
        int bottomY = y + (height - 1);

        // Draw center rows
        for (int j = 2; j < height - 1; ++j){
            setTile(wrapTileX(x), y + j, tilesetIndex + LM, false, false, palette, layer); // Left-middle tile
            for (int i = 1; i < width - 1; ++i){
                setTile(wrapTileX(x + i), y + j, tilesetIndex + CENTER, false, false, palette, layer); // Center
            }
            setTile(wrapTileX(x + (width - 1)), y + j, tilesetIndex + RM, false, false, palette, layer); // Right-middle tile
        }

        // Draw the bottom-middle row (mirrored)
        if (height > 4){
            setTile(wrapTileX(x), bottomY - 1, tilesetIndex + LM_UPPER, false, true, palette, layer); // Left-middle-lower tile (flipped vertically)
            for (int i = 1; i < width - 1; ++i){
                setTile(wrapTileX(x + i), bottomY - 1, tilesetIndex + MIDDLE_UPPER, false, true, palette, layer); // Middle lower (flipped vertically)
            }
            setTile(wrapTileX(x + (width - 1)), bottomY - 1, tilesetIndex + RM_UPPER, false, true, palette, layer); // Right-middle-lower tile (flipped vertically)
        }

        // Draw the bottom row
        if (width > 2){
            setTile(wrapTileX(x + 1), bottomY, tilesetIndex + TL_2, false, true, palette, layer); // Bottom-left corner Part 2 (flipped vertically)
        }
        if (width > 3){
            setTile(wrapTileX(x + 2), bottomY, tilesetIndex + TL_3, false, true, palette, layer); // Bottom-left corner Part 3 (flipped vertically)
        }
        for (int i = 3; i < width - 3; ++i){
            setTile(wrapTileX(x + i), bottomY, tilesetIndex + TOP_MIDDLE, false, true, palette, layer); // Bottom middle (flipped vertically)
        }
        if (width >= 3){
            setTile(wrapTileX(x + (width - 3)), bottomY, tilesetIndex + TR_1, false, true, palette, layer); // Bottom-right corner Part 1 (flipped vertically)
        }
        if (width >= 2){
            setTile(wrapTileX(x + (width - 2)), bottomY, tilesetIndex + TR_2, false, true, palette, layer); // Bottom-right corner Part 2 (flipped vertically)
        }

        // Draw the top row
        for (int i = 3; i < width - 3; ++i){
            setTile(wrapTileX(x + i), y, tilesetIndex + TOP_MIDDLE, false, false, palette, layer); // Top middle
        }

        if (width >= 3){
            setTile(wrapTileX(x + (width - 3)), y, tilesetIndex + TR_1, false, false, palette, layer); // Top-right corner Part 1
        }
        if (width >= 2){
            setTile(wrapTileX(x + 1), y, tilesetIndex + TL_2, false, false, palette, layer); // Top-left corner Part 2
        }
        if (width >= 3){
            setTile(wrapTileX(x + 2), y, tilesetIndex + TL_3, false, false, palette, layer); // Top-left corner Part 3
        }

        if (width >= 2){
            setTile(wrapTileX(x + (width - 2)), y, tilesetIndex + TR_2, false, false, palette, layer); // Top-right corner Part 2
        }

        // Correct top-right and bottom-right corners with wrapping
        setTile(wrapTileX(x + (width - 1)), bottomY, tilesetIndex + TR_3, false, true, palette, layer); // Bottom-right corner Part 3 (flipped vertically)
        setTile(wrapTileX(x + (width - 1)), y, tilesetIndex + TR_3, false, false, palette, layer); // Top-right corner Part 3

        setTile(wrapTileX(x), bottomY, tilesetIndex + TL_1, false, true, palette, layer); // Bottom-left corner Part 1 (flipped vertically)
        setTile(wrapTileX(x), y, tilesetIndex + TL_1, false, false, palette, layer); // Top-left corner Part 1
    }else{
		drawLaserRow(x, y, width, layer, true);
    }
}

// Helper function to ensure tilemapX remains within tilemap bounds (and wraps around the screen)
int wrapTileX(int tileXPos){
    return (tileXPos + 30) % 30; // Ensure x is within [0, 29]
}

// Helper function to ensure tilemapY remains within tilemap bounds
bool tileYInBounds(int tileYPos){
    return tileYPos >= 0 && tileYPos < 30;
}

// Draw the LASER_TOP and LASER_BOTTOM tiles with wrapping
void drawLaserRow(int x, int y, int width, int layer, bool wrapAround){
	int tilesetIndex = MENU_GFX_START;
    int palette = 2;
	for (int i = 0; i < width; ++i){
        int drawX = x + i;

        // Handle wrapping or boundary checking based on the boolean flag
        if (wrapAround){
            drawX = wrapTileX(drawX);
        }else if (!tileYInBounds(drawX)){
            continue; // Skip drawing if out of bounds and not wrapping
        }

        // Check bounds for y position (should be within the range 0-29)
        if (tileYInBounds(y) && tileYInBounds(y + 1)){
            setTile(drawX, y, tilesetIndex + LASER_TOP, false, false, palette, layer);
            setTile(drawX, y + 1, tilesetIndex + LASER_BOTTOM, false, false, palette, layer);
        }
    }
}

/// @brief Draws a nine slice window for the main menu's Menu Page window; Width and Height params are in terms of 8x8 tiles
/// @param width 
/// @param height 
void drawSecondaryNineSliceWindowStyle(int x, int y, int width, int height, int layer){
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            int tileX = (x + j) % TILEMAP_WIDTH;
            int tileY = (y + i) % TILEMAP_HEIGHT;
            int tileIndex = MENU_GFX_START + SEC_CENTER; // Default to center tile
            bool flipHorizontal = false;
            bool flipVertical = false;

            // Determine which tile to draw based on position in the nine-slice grid
            if (i == 0 && j == 0){
                tileIndex = MENU_GFX_START + SEC_TOP_LEFT;
            }else if (i == 0 && j == width - 1){
                tileIndex = MENU_GFX_START + SEC_TOP_LEFT;
                flipHorizontal = true; // Flip for top-right corner
            }else if (i == 0){
                tileIndex = MENU_GFX_START + SEC_TOP_MIDDLE;
            }else if (i == height - 1 && j == 0){
                tileIndex = MENU_GFX_START + SEC_TOP_LEFT;
                flipVertical = true; // Flip for bottom-left corner
            }else if (i == height - 1 && j == width - 1){
                tileIndex = MENU_GFX_START + SEC_TOP_LEFT;
                flipHorizontal = true; // Flip for bottom-right corner
                flipVertical = true;
            }else if (i == height - 1){
                tileIndex = MENU_GFX_START + SEC_TOP_MIDDLE;
                flipVertical = true; // Flip for bottom edge
            }else if (j == 0){
                tileIndex = MENU_GFX_START + SEC_LEFT;
            }else if (j == width - 1){
                tileIndex = MENU_GFX_START + SEC_LEFT;
                flipHorizontal = true; // Flip for right edge
            }

            setTile(tileX, tileY, tileIndex, flipHorizontal, flipVertical, 2, layer);
        }
    }
}

void drawMenuTextSegment(int tileXPos, int tileYPos, int menuElementPosition, int palette, bool highlighted, int numTextTileColumns){
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

void directionalInputEnabled(){
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
		moveX = 1;
	}
	else if((inputs.pressed & KEY_LEFT) && !(inputs.pressed & KEY_RIGHT)){
		moveX = -1;
	}
}

void menuInputConfirmEnabled(){
	if((inputs.pressed & KEY_A) || (inputs.pressed & KEY_START)){
		MenuPageItem* thisMenuItem = &menuPages[mDat.currMenuPage].items[mDat.menuCursorPos];
		switch(mDat.windowState){
			default:
				break;
			case MMWS_READY:
				switch(menuPage->items[mDat.menuCursorPos].menuElement){
					default:
						break;
					case ME_SCRIPT_RUNNER:
						// Execute the function at this menuElement, but only if it's actually valid.
						switch(thisMenuItem->dataType){
							default:
								break;
							case MPIDT_FUNC_PTR:
								thisMenuItem->data.functionPtr();
								break;
						}
						break;
					case ME_PAGE_TRANSFER:
						playSFX(_sfxMenuConfirmA, AUDGROUP_MENUSFX);
						mDat.windowConfirmDirection = MWCD_FORWARD;
						mDat.windowState = MMWS_CLOSING;
						mDat.updateSpriteDraw = true;

						// Lerp toward making the text completely invisible, over 32 frames
						mDat.evaLerpStart = 16;
						mDat.evaLerpEnd = 0;
						mDat.evbLerpStart = 0;
						mDat.evbLerpEnd = 16;
						mDat.windowActionTimer = 0;
						mDat.windowActionTarget = 8;
						
						// Hide the Pade UI Text Sprite
						hideSprite(MENU_PAGE_TEXT_SPRITE);
						
						// If there are any menu toggles/sliders/cursors visible, hide them right now
						hideSpriteRange(MENU_SLIDER_PROMPT_SPRITE1, FONT_PERCENT_SPRITE_LAST);
						break;
					case ME_SLIDER:
						playSFX(_sfxMenuConfirmA, AUDGROUP_MENUSFX);
						mDat.windowState = MMWS_TWEAKING_DATA;
						mDat.updateSpriteDraw = true;
						break;
					case ME_SHIFT:
						break;
					case ME_TOGGLE:
						playSFX(_sfxMenuConfirmA, AUDGROUP_MENUSFX);
						thisMenuItem->data.boolVal = !thisMenuItem->data.boolVal;
						mDat.updateSpriteDraw = true;
						break;
					case ME_SOUND_TESTER:
						switch(thisMenuItem->id) {
							default:
								break;
							case MID_SOUND_TEST_BGM:
								int index = thisMenuItem->data.intVal;
								playBGM(index);
								/*
								if (index >= 0 && index < SOUND_TEST_BGM_COUNT) {
									// Retrieve the group or single track
									const int* group = bgmGroups[index];
									
									// Check if it's a group or a single track
									if (group[1] == BGM_SINGLE) {
										// Single track, just play it
										currentBGMIndex[0] = playNewSound(group[0]);
										currentBGMIndex[1] = 0xFF;
									} else {
										// Group, play each item in the group
										for (int i = 0; i < 2; ++i) {
											if (group[i] != BGM_SINGLE) {
												if (i == 0) {
													currentBGMIndex[0] = playNewSound(group[i]);
												} else {
													currentBGMIndex[i] = playNewSound(group[i]);
												}
											}
										}
									}
								}*/
								break;
							case MID_SOUND_TEST_SFX:
								int sfxID = thisMenuItem->data.intVal + SFX_START;

								// Play _sfxShipMove only on SFX channel 2; all other SFX should play on SFX channel 1
								// The only SFX that should stop _sfxShipMove is itself, or _sfxShipIdle

								playSFX(sfxID, AUDGROUP_SOUNDT_SFX);
								
								break;
						}
						break;
					case ME_CREDITS_DISPLAY:
						break;
				}
				break;
			case MMWS_TWEAKING_DATA:
				switch(menuPage->items[mDat.menuCursorPos].menuElement){
					default:
						break;
					case ME_SCRIPT_RUNNER:
						break;
					case ME_PAGE_TRANSFER:
						break;
					case ME_SLIDER:
						playSFX(_sfxMenuConfirmA, AUDGROUP_MENUSFX);
						// Hide the slider prompt
						hideSpriteRange(MENU_SLIDER_PROMPT_SPRITE1, MENU_SLIDER_PROMPT_SPRITE2);
						mDat.windowState = MMWS_READY;
						mDat.updateSpriteDraw = true;
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

void menuInputCancelEnabled(){
	if((inputs.pressed & KEY_B)){
		switch(mDat.windowState){
			default:
				break;
			case MMWS_READY:
				playSFX(_sfxMenuCancel, AUDGROUP_MENUSFX);
				mDat.windowConfirmDirection = MWCD_BACKWARD;
				mDat.windowState = MMWS_CLOSING;
				
				// If there are any menu toggles/sliders/cursors visible, hide them right now
				hideSpriteRange(MENU_SLIDER_PROMPT_SPRITE1, FONT_PERCENT_SPRITE_LAST);

				// Lerp toward making the text completely invisible, over 32 frames
				mDat.evaLerpStart = 16;
				mDat.evaLerpEnd = 0;
				mDat.evbLerpStart = 0;
				mDat.evbLerpEnd = 16;
				mDat.windowActionTimer = 0;
				mDat.windowActionTarget = 8;

				mDat.updateSpriteDraw = true;

				if (mDat.currMenuPage == MPI_OPTIONS) {
					// We just backed out of the Options menu: Reset all current options values to the real options struct's values
					optionsMenuItems = menuPages[MPI_OPTIONS].items;
					optionsMenuItems[OPTID_MASTER_VOL].data.intVal = (int)options.masterVolume;
					optionsMenuItems[OPTID_BGM_VOL].data.intVal = (int)options.bgmVolume;
					optionsMenuItems[OPTID_SFX_VOL].data.intVal = (int)options.sfxVolume;
					optionsMenuItems[OPTID_GRID_ENABLED].data.boolVal = (int)options.gridOn;
					updateSoundVolumes(true);
				}
				break;
			case MMWS_TWEAKING_DATA:
				playSFX(_sfxMenuMove, AUDGROUP_MENUSFX);
				// Hide the slider prompt
				hideSpriteRange(MENU_SLIDER_PROMPT_SPRITE1, MENU_SLIDER_PROMPT_SPRITE2);
				mDat.windowState = MMWS_READY;
				mDat.updateSpriteDraw = true;
				break;
		}
	}
}

void performPageTransfer(int datIntVal){
	mDat.windowTargetWidth = menuPage->tileWidth;
	mDat.windowTargetHeight = menuPage->tileHeight;

	mDat.menuCursorPos = 0;

	// Get the new Menu Page based on the data we're reading
	mDat.currMenuPage = datIntVal;
	menuPage = &menuPages[mDat.currMenuPage];

	if (mDat.currMenuPage == MPI_SOUND_TEST){
		// Kill all sound except for currentSFXIndex[0] (should be _sfxMenuConfirm)
		stopAllSoundExcept(&currentSFXIndex[AUDGROUP_MENUSFX]);
	}else if (!isSoundPlaying(_musMainMenu, currentBGMIndex[0])){
		// Kill all sound except for currentSFXIndex[0] (should be _sfxMenuCancel)
		stopAllSoundExcept(&currentSFXIndex[AUDGROUP_MENUSFX]);

		// Play the Main Menu BGM again if it isn't currently playing when we leave
		playBGM(BGM_MAINMENU);
	}

	// Set the Page UI Drawing Y-Pos, depending on which page we're on
	mDat.secondaryNineSliceYOff = 2;
	if (mDat.currMenuPage == (int)MPI_CREDITS)
		mDat.secondaryNineSliceYOff--;

	// Start loading the new menu page's graphics into VRAM (this will take more than one frame, so this function will keep being called even during MMWS_ZIPPING state)
	loadMenuGraphics(menuPage);

	// Draw the Menu Page UI Text now
	drawMenuPageUIText();

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
	IOBuffer0[0] = bg0XPos;
	IOBuffer0[1] = bg0YPos;
	IOBuffer0[2] = bg1XPos;
	IOBuffer0[3] = bg1YPos;
	IOBuffer0[4] = 0;
	IOBuffer0[5] = 0;
	IOBuffer0[6] = 0;
	IOBuffer0[7] = 0;

	// Send the buffer to get processed
	IOData[0].position = (void *)(&REG_BG0HOFS);
	IOData[0].buffer = IOBuffer0;
	IOData[0].size = 4;
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

void drawMenuButtonPrompts(bool hideAll){
	if (menuPage->showConfirmPrompt && !hideAll){
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(18*8);
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST].attr1 = ATTR1_SIZE_32 | ATTR1_X(26*8);
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST].attr2 = ATTR2_ID(512 + MENU_BUTTON_PROMPT_GFX_START) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_BUTTON_PROMPT_PAL);
	}else
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST].attr0 = ATTR0_HIDE;

	// B Prompt
	if (menuPage->showBackPrompt && !hideAll){
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 1].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(18*8);
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 1].attr1 = ATTR1_SIZE_32 | ATTR1_X(0);
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 1].attr2 = ATTR2_ID(512 + MENU_BUTTON_PROMPT_GFX_START + 8) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_BUTTON_PROMPT_PAL);
	}else
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 1].attr0 = ATTR0_HIDE;
	
	// R Prompt
	if (menuPage->showSoundTestPrompts && !hideAll){
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 2].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(18*8);
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 2].attr1 = ATTR1_SIZE_32 | (ATTR1_X(20*7) - 4);
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 2].attr2 = ATTR2_ID(512 + MENU_BUTTON_PROMPT_GFX_START + 16) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_BUTTON_PROMPT_PAL);
	}else
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 2].attr0 = ATTR0_HIDE;
		
	// L Prompt
	if (menuPage->showSoundTestPrompts && !hideAll){
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 3].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_WIDE | ATTR0_Y(18*8);
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 3].attr1 = ATTR1_SIZE_32 | ATTR1_X(20*4);
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 3].attr2 = ATTR2_ID(512 + MENU_BUTTON_PROMPT_GFX_START + 24) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_BUTTON_PROMPT_PAL);
	}else
		objectBuffer[MENU_BUTTON_PROMPT_SPRITE_FIRST + 3].attr0 = ATTR0_HIDE;
}

void drawSliderPrompt(int xPos, int yPos, int sprIndex, bool flipSpriteHorizontally){
	objectBuffer[sprIndex].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SHAPE(2) | ATTR0_Y(yPos);
	objectBuffer[sprIndex].attr1 = ATTR1_SIZE(0) | ATTR1_X(xPos) | (flipSpriteHorizontally ? ATTR1_HFLIP : 0x0000);
	objectBuffer[sprIndex].attr2 = ATTR2_ID(512 + MENU_SLIDER_PROMPT_GFX_START) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_BUTTON_PROMPT_PAL);
}

void drawSliderBar(int sprIndex, int xPos, int yPos, int imgIndex, int barValue){
    u8 barCount = 5;
    for (u8 i = 0; i < barCount; i++) {
        // Determine imgIndex for the current sprite
        imgIndex = (barValue > (i * 2)) ? ((barValue > (i * 2 + 1)) ? 2 : 1) : 0;

        // Set the attributes for the sprite
        objectBuffer[MENU_SLIDER_BAR_SPRITE_FIRST + (sprIndex * barCount) + i].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SHAPE(2) | ATTR0_Y(yPos);
        objectBuffer[MENU_SLIDER_BAR_SPRITE_FIRST + (sprIndex * barCount) + i].attr1 = ATTR1_SIZE(0) | ATTR1_X(xPos + (i * 8));
        objectBuffer[MENU_SLIDER_BAR_SPRITE_FIRST + (sprIndex * barCount) + i].attr2 = ATTR2_ID(512 + MENU_SLIDER_BARS_GFX_START + (imgIndex * 2)) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_SLIDER_BARS_PAL);
    }
}

void drawDigit(int sprIndex, int singleDigit, int xPos, int yPos) {
    // Ensure the digit is within the valid range (0-9)
    singleDigit = clamp(singleDigit, 0, 10);

    // Calculate the correct tile ID based on the digit value
    int tileID = FONT_NUMBERS_GFX_START + (singleDigit * 2);

    // Set the attributes for the sprite
    objectBuffer[sprIndex].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SHAPE(2) | ATTR0_Y(yPos);
    objectBuffer[sprIndex].attr1 = ATTR1_SIZE(0) | ATTR1_X(xPos);
    objectBuffer[sprIndex].attr2 = ATTR2_ID(512 + tileID) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_SLIDER_BARS_PAL);
}

int drawPercent(int sprIndex, int xPos, int yPos) {
    // Set the attributes for the sprite
    objectBuffer[sprIndex].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SHAPE(0) | ATTR0_Y(yPos);
    objectBuffer[sprIndex].attr1 = ATTR1_SIZE(1) | ATTR1_X(xPos);
    objectBuffer[sprIndex].attr2 = ATTR2_ID(512 + FONT_PERCENT_GFX_START) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_SLIDER_BARS_PAL);
	return 1;
}

int drawNumber(int startIndex, int numberToDraw, int xPos, int yPos, bool rightAlign) {
    char digits[3];  // Array to hold the digits of the number
    int digitCount = 0;

    // Extract the digits from the number
    do {
        digits[digitCount++] = numberToDraw % 10;
        numberToDraw /= 10;
    } while (numberToDraw > 0 && digitCount < 3);

    // Adjust the starting x position for right alignment if needed
    if (rightAlign) {
        xPos -= (digitCount * 6);
    }

    // Flag to track if we’ve drawn any digit yet
    bool leadingZeroSkipped = false;

    // Draw each digit, from left to right
    for (int i = digitCount - 1; i >= 0; i--) {
        // Skip leading zeros until we hit a non-zero digit or it's the last digit
        if (digits[i] != 0 || leadingZeroSkipped || i == 0) {
            leadingZeroSkipped = true;
            drawDigit(startIndex++, digits[i], xPos, yPos);
            xPos += 6;  // Move to the next digit's position
        }
    }

    return startIndex; // Return the next available sprite index
}



void drawToggle(int sprIndex, int xPos, int yPos, bool isEnabled) {
	u8 imgIndex = 0;
	if (!isEnabled)
		imgIndex = 1;

	// Set the attributes for the sprite
	objectBuffer[MENU_TOGGLE_SPRITE_FIRST + sprIndex].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SHAPE(1) | ATTR0_Y(yPos);
	objectBuffer[MENU_TOGGLE_SPRITE_FIRST + sprIndex].attr1 = ATTR1_SIZE(3) | ATTR1_X(xPos);
	objectBuffer[MENU_TOGGLE_SPRITE_FIRST + sprIndex].attr2 = ATTR2_ID(512 + MENU_TOGGLE_PROMPT_GFX_START + (32 * imgIndex)) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_BUTTON_PROMPT_PAL);
}

void drawMenuPageUIText(){
	if (mDat.showPageWindowBG){
		if (mDat.eva >= 2 && mDat.evb <= 14){
			// Draw the Menu Page UI Text now				
			objectBuffer[MENU_PAGE_TEXT_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SHAPE(1) | ATTR0_Y(mDat.secondaryNineSliceYOff * 8) | ATTR0_BLEND;
			objectBuffer[MENU_PAGE_TEXT_SPRITE].attr1 = ATTR1_SIZE(3) | ATTR1_X(89);
			objectBuffer[MENU_PAGE_TEXT_SPRITE].attr2 = ATTR2_ID(MENU_PAGE_TEXT_GFX_START + (32 * mDat.currMenuPage)) | ATTR2_PRIO(0) | ATTR2_PALBANK(MENU_PAGE_TEXT_PAL_START);
		}else{
			//objectBuffer[MENU_PAGE_TEXT_SPRITE].attr0 = ATTR0_HIDE;
		}
	}else{
		//objectBuffer[MENU_PAGE_TEXT_SPRITE].attr0 = ATTR0_HIDE;
	}
}

void hideSprite(int sprIndex){
	objectBuffer[sprIndex].attr0 = ATTR0_HIDE;
}

void hideSpriteRange(int firstSprite, int lastSprite){
	for (u8 i = firstSprite; i <= lastSprite; i++){
		objectBuffer[i].attr0 = ATTR0_HIDE;
	}
}

bool sfxIsPlaying(int sfxGroupIndex) {
	for (int i = 0; i < audioGroupSizes[sfxGroupIndex]; i++) {
		int sfxIDToHuntFor = audioGroups[sfxGroupIndex][i];
		if (isSoundPlaying(sfxIDToHuntFor, currentSFXIndex[sfxGroupIndex]))
			return true;
	}
	return false;
}



void skipToMenu(){
	//mainMenuData.actionTarget = 0;
	//mainMenuData.actionTimer = 0;
	//mainMenuData.state = FLASH_WHITE;
	

	currentScene.state = NORMAL;
	mDat.state = TITLE_FLY_OUT;
	mDat.actionTimer = 1;
	mDat.actionTarget = 100;
}

void updateObjBuffer(){
	OAMData.position = (void *)oam_mem;
	OAMData.buffer = objectBuffer;
	OAMData.size = sizeof(objectBuffer) >> 2;
}

void mainMenuInitBlend(){
	REG_BLDCNT = BLD_STD | BLD_TOP(BLD_OBJ) | BLD_BOT(BLD_BG2);

/*
	//u8 blendLayer = 2;
	//
	
    mDat.blendMode = 0;

    // eva, evb and ey are .4 fixeds
    // eva is full, evb and ey are empty
    mDat.eva = 0x80;
	mDat.evb = 0;
	mDat.ey = 0;
	REG_BLDCNT= BLD_BUILD(
        BLD_OBJ,  // Top layers
        BLD_BG1,            // Bottom layers
        mDat.blendMode);              // Blend Mode
*/
}

void mainMenuUpdateBlend(u32 eva, u32 evb){
	// Clamp to allowable ranges
	eva = clamp(eva, 0, 0x81);
	evb = clamp(evb, 0, 0x81);
	//ey  = clamp(ey, 0, 0x81);
	//mode= clamp(mode, 0, 4);
	
	REG_BLDALPHA = BLD_EVA(eva) | BLD_EVB(evb);
	/*
	// Interactive blend weights
	mDat.eva += key_tri_horz();
	mDat.evb -= key_tri_vert();
	mDat.ey  += key_tri_fire();

	mDat.blendMode += bit_tribool(moveX, KI_R, KI_L);

	// Clamp to allowable ranges
	mDat.eva = clamp(mDat.eva, 0, 0x81);
	mDat.evb = clamp(mDat.evb, 0, 0x81);
	mDat.ey = clamp(mDat.ey, 0, 0x81);
	mDat.blendMode = clamp(mDat.blendMode, 0, 4);

	// Update blend mode
	BFN_SET(REG_BLDCNT, mDat.blendMode, BLD_MODE);

	// Update blend weights
	REG_BLDALPHA= BLDA_BUILD(mDat.eva/8, mDat.evb/8);
	REG_BLDY= BLDY_BUILD(mDat.ey/8);
	*/
}

int interpolateValues(int timer, int targetTime, int lerpStartVal, int lerpTargetVal){
	// Calculate the interpolation factor t, with proper scaling
	int actionTimerScaled = timer * FIXED_POINT_SCALE;
	int actionTargetScaled = targetTime * FIXED_POINT_SCALE;
	int t = (actionTimerScaled * FIXED_POINT_SCALE) / actionTargetScaled;

	// Apply ease-in-out function
	int easedT = easeInOut(t, 4);

	// Calculate the interpolated position and update yPos
	return lerp(lerpStartVal * FIXED_POINT_SCALE, lerpTargetVal * FIXED_POINT_SCALE, easedT) / FIXED_POINT_SCALE;
}

int calculatePercentage(int numerator, int denominator) {
	return (numerator * 100 + (denominator / 2)) / denominator;
}


void initMenuPages(MenuPage menuPages[]) {
    menuPages[MPI_MAIN_MENU] = (MenuPage) {
        .items = {
            {"Play Game", ME_PAGE_TRANSFER, .data.intVal = MPI_PLAY_GAME, .dataType = MPIDT_INT, .textGFXIndex = 0},
            {"Options", ME_PAGE_TRANSFER, .data.intVal = MPI_OPTIONS, .dataType = MPIDT_INT, .textGFXIndex = 2},
            {"Extras", ME_PAGE_TRANSFER, .data.intVal = MPI_EXTRAS, .dataType = MPIDT_INT, .textGFXIndex = 4}
        },
        .itemCount = 3,
        .pageName = "MAIN MENU",
        .tileX = 10,
        .tileY = 6,
        .tileWidth = 10,
        .tileHeight = 10,
        .pxOffX = 0,
        .backPage = (int)MPI_MAIN_MENU,
        .showConfirmPrompt = true,
        .showBackPrompt = false
    };

    menuPages[MPI_PLAY_GAME] = (MenuPage) {
        .items = {
            {"New Game", ME_PAGE_TRANSFER, .data.intVal = MPI_NEW_GAME, .dataType = MPIDT_INT, .textGFXIndex = 6},
            {"Continue", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecContinue, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 8},
            {"Load Game", ME_PAGE_TRANSFER, .data.intVal = MPI_LOAD_GAME, .dataType = MPIDT_INT, .textGFXIndex = 10},
            {"Back", ME_PAGE_TRANSFER, .data.intVal = MPI_MAIN_MENU, .dataType = MPIDT_INT, .textGFXIndex = 12}
        },
        .itemCount = 4,
        .pageName = "PLAY GAME",
        .tileX = 10,
        .tileY = 6,
        .tileWidth = 10,
        .tileHeight = 12,
        .pxOffX = 0,
        .backPage = (int)MPI_MAIN_MENU,
        .showConfirmPrompt = true,
        .showBackPrompt = true
    };

    menuPages[MPI_SOUND_TEST] = (MenuPage) {
        .items = {
            {"BGM", ME_SOUND_TESTER, .data.intVal = 0, .dataType = MPIDT_INT, .textGFXIndex = 16, .id = MID_SOUND_TEST_BGM},
            {"SFX", ME_SOUND_TESTER, .data.intVal = 0, .dataType = MPIDT_INT, .textGFXIndex = 18, .id = MID_SOUND_TEST_SFX},
            {"Back", ME_PAGE_TRANSFER, .data.intVal = MPI_EXTRAS, .dataType = MPIDT_INT, .textGFXIndex = 24, .id = MID_SOUND_TEST_CANCEL}
        },
        .itemCount = 3,
        .pageName = "SOUND TEST",
        .tileX = 5,
        .tileY = 6,
        .tileWidth = 22,
        .tileHeight = 10,
        .pxOffX = 4,
        .backPage = (int)MPI_EXTRAS,
        .showConfirmPrompt = true,
        .showSoundTestPrompts = true,
        .showBackPrompt = true
    };

    menuPages[MPI_EXTRAS] = (MenuPage) {
        .items = {
            {"Sound Test", ME_PAGE_TRANSFER, .data.intVal = MPI_SOUND_TEST, .dataType = MPIDT_INT, .textGFXIndex = 38},
            {"Credits", ME_PAGE_TRANSFER, .data.intVal = MPI_CREDITS, .dataType = MPIDT_INT, .textGFXIndex = 40},
            {"Back", ME_PAGE_TRANSFER, .data.intVal = MPI_MAIN_MENU, .dataType = MPIDT_INT, .textGFXIndex = 42}
        },
        .itemCount = 3,
        .pageName = "EXTRAS",
        .tileX = 11,
        .tileY = 6,
        .tileWidth = 9,
        .tileHeight = 10,
        .pxOffX = 4,
        .backPage = (int)MPI_MAIN_MENU,
        .showConfirmPrompt = true,
        .showBackPrompt = true
    };

    menuPages[MPI_CREDITS] = (MenuPage) {
        .items = {
            {"- Programming -", ME_CREDITS_DISPLAY, .textGFXIndex = 26},
            {"origamiscienceguy", ME_CREDITS_DISPLAY, .textGFXIndex = 32},
            {"- Graphics -", ME_CREDITS_DISPLAY, .textGFXIndex = 30},
            {"n67094", ME_CREDITS_DISPLAY, .textGFXIndex = 36},
            {"- Audio -", ME_CREDITS_DISPLAY, .textGFXIndex = 28},
            {"potatoTeto", ME_CREDITS_DISPLAY, .textGFXIndex = 34}
        },
        .itemCount = 6,
        .pageName = "CREDITS",
        .tileX = 9,
        .tileY = 4,
        .tileWidth = 13,
        .tileHeight = 16,
        .pxOffX = 4,
        .backPage = (int)MPI_EXTRAS,
        .showConfirmPrompt = false,
        .showBackPrompt = true
    };

    menuPages[MPI_OPTIONS] = (MenuPage) {
        .items = {
            {"Master Volume", ME_SLIDER, .dataType = MPIDT_INT_ARRAY, .textGFXIndex = 14, .data.intVal = DEFAULT_MASTER_VOLUME, .id = MID_OPT_MAST_VOL},
            {"BGM", ME_SLIDER, .dataType = MPIDT_INT_ARRAY, .textGFXIndex = 16, .data.intVal = DEFAULT_BGM_VOLUME, .id = MID_OPT_BGM_VOL},
            {"SFX", ME_SLIDER, .dataType = MPIDT_INT_ARRAY, .textGFXIndex = 18, .data.intVal = DEFAULT_SFX_VOLUME, .id = MID_OPT_SFX_VOL},
            {"Grid Enabled", ME_TOGGLE, .dataType = MPIDT_BOOL, .textGFXIndex = 20, .data.boolVal = DEFAULT_GRID_FLAG, .id = MID_OPT_GRID_ENABLED},
            {"Apply Changes", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecOptionsApplyChanges, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 22},
        },
        .itemCount = 5,
        .pageName = "OPTIONS",
        .tileX = 5,//10,
        .tileY = 6,
        .tileWidth = 22,//11
        .tileHeight = 14,
        .pxOffX = 4,
        .backPage = (int)MPI_MAIN_MENU,
        .showConfirmPrompt = true,
        .showBackPrompt = true
    };

    menuPages[MPI_NEW_GAME] = (MenuPage) {
        .items = {
            {"Slot 1", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecNewGame, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 44},
            {"Slot 2", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecNewGame, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 46},
            {"Slot 3", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecNewGame, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 48},
            {"Back", ME_PAGE_TRANSFER, .data.intVal = MPI_PLAY_GAME, .dataType = MPIDT_INT, .textGFXIndex = 12}
        },
        .itemCount = 4,
        .pageName = "PLAY GAME",
        .tileX = 10,
        .tileY = 6,
        .tileWidth = 10,
        .tileHeight = 12,
        .pxOffX = 0,
        .backPage = (int)MPI_PLAY_GAME,
        .showConfirmPrompt = true,
        .showBackPrompt = true
    };

    menuPages[MPI_LOAD_GAME] = (MenuPage) {
        .items = {
            {"Slot 1", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecLoadGame, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 44},
            {"Slot 2", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecLoadGame, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 46},
            {"Slot 3", ME_SCRIPT_RUNNER, .data.functionPtr = menuExecLoadGame, .dataType = MPIDT_FUNC_PTR, .textGFXIndex = 48},
            {"Back", ME_PAGE_TRANSFER, .data.intVal = MPI_PLAY_GAME, .dataType = MPIDT_INT, .textGFXIndex = 12}
        },
        .itemCount = 4,
        .pageName = "PLAY GAME",
        .tileX = 10,
        .tileY = 6,
        .tileWidth = 10,
        .tileHeight = 12,
        .pxOffX = 0,
        .backPage = (int)MPI_PLAY_GAME,
        .showConfirmPrompt = true,
        .showBackPrompt = true
    };
}


int menuExecNewGame(){
	stopAllSound();
	playSFX(_sfxMenuConfirmC, AUDGROUP_MENUSFX);
	
	mDat.windowState = MMWS_FINALIZING;
	mDat.windowConfirmDirection = MWCD_FORWARD;
	mDat.updateSpriteDraw = true;
	
	// Once it's relevant (during MMWS_CLOSING state), lerp toward making the text completely visible
	mDat.evaLerpStart = 16;
	mDat.evaLerpEnd = 0;
	mDat.evbLerpStart = 0;
	mDat.evbLerpEnd = 16;

	mDat.windowActionTimer = 0;
	mDat.windowActionTarget = 100;


	
	u8 saveSlot = clamp(mDat.menuCursorPos, 0, 3);

	initMapData(&mapData);
	initMap();
	//initOptions(&options);
	options.lastPlayedSaveSlot = saveSlot;



	return 0;
}

int menuExecContinue(){
	stopAllSound();
	playSFX(_sfxMenuConfirmC, AUDGROUP_MENUSFX);
	
	mDat.windowState = MMWS_FINALIZING;
	mDat.windowConfirmDirection = MWCD_FORWARD;
	mDat.updateSpriteDraw = true;
	
	// Once it's relevant (during MMWS_CLOSING state), lerp toward making the text completely visible
	mDat.evaLerpStart = 16;
	mDat.evaLerpEnd = 0;
	mDat.evbLerpStart = 0;
	mDat.evbLerpEnd = 16;

	mDat.windowActionTimer = 0;
	mDat.windowActionTarget = 100;


	
	u8 saveSlot = clamp(options.lastPlayedSaveSlot, 0, 3);
	loadGame(&mapData, saveSlot);
	//loadOptions(&options);
	options.lastPlayedSaveSlot = saveSlot;
	return 0;
}

int menuExecLoadGame(){

	/*
	

	// To save the game data and options
	saveGame(&mapData, saveSlot);
	saveOptions(&options);


*/
	stopAllSound();
	playSFX(_sfxMenuConfirmC, AUDGROUP_MENUSFX);
	
	mDat.windowState = MMWS_FINALIZING;
	mDat.windowConfirmDirection = MWCD_FORWARD;
	mDat.updateSpriteDraw = true;
	
	// Once it's relevant (during MMWS_CLOSING state), lerp toward making the text completely visible
	mDat.evaLerpStart = 16;
	mDat.evaLerpEnd = 0;
	mDat.evbLerpStart = 0;
	mDat.evbLerpEnd = 16;

	mDat.windowActionTimer = 0;
	mDat.windowActionTarget = 100;


	
	u8 saveSlot = clamp(mDat.menuCursorPos, 0, 3);
	loadGame(&mapData, saveSlot);
	//loadOptions(&options);
	options.lastPlayedSaveSlot = saveSlot;

	return 0;
}

int menuExecOptionsApplyChanges(){
	playSFX(_sfxMenuConfirmB, AUDGROUP_MENUSFX);
	updateOptions();
	mDat.windowState = MMWS_APPLIED_OPTIONS;
	mDat.windowActionTimer = 0;
	mDat.windowActionTarget = 50;

	// Set whatever current values are to the real options struct
	
    optionsMenuItems = menuPages[MPI_OPTIONS].items;
	options.masterVolume = (u8)menuPages[MPI_OPTIONS].items[OPTID_MASTER_VOL].data.intVal;
	options.bgmVolume = (u8)menuPages[MPI_OPTIONS].items[OPTID_BGM_VOL].data.intVal;
	options.sfxVolume = (u8)menuPages[MPI_OPTIONS].items[OPTID_SFX_VOL].data.intVal;
	options.gridOn = (u8)menuPages[MPI_OPTIONS].items[OPTID_GRID_ENABLED].data.boolVal;

	updateSoundVolumes(false);
	return 0;
}

// Finalizes whatever options are currently set in the Options Menu
void updateOptions(){	
	MenuPageItem *mp_items = menuPages[MPI_OPTIONS].items;
	options.gridOn = mp_items[OPTID_GRID_ENABLED].data.boolVal;
}

void matchBegin(){
	// Change the scene
	currentScene.scenePointer = sceneList[GAMEPLAY];
	currentScene.state = INITIALIZE;

	// End the current BGM
	endCurrentBGM();
}

int calculateEffectiveVolume(int soundAssetVol, int userVol) {
    // assetVolume: Volume defined in the asset (0-256 for 0-100%)
    // userVolume: User-defined volume (0-10)
    
    // Get the fixed-point multiplier for the user volume setting
    int userVolMultiplier = volumeTable[userVol];
    
    // Calculate the effective volume using fixed-point multiplication
    return (soundAssetVol * userVolMultiplier) >> 8;
}

u8 calculateFinalVolume(u8 assetVolume, int userVolume, int masterVolume) {
    // Get the fixed-point multipliers for the user volume and master volume
    int userVolMultiplier = volumeTable[userVolume];
    int masterVolMultiplier = volumeTable[masterVolume];
    
    // First apply the user volume, then apply the master volume
    int effectiveVolume = (assetVolume * userVolMultiplier) >> 8;
    return (effectiveVolume * masterVolMultiplier) >> 8;
}

void endCurrentBGM(){
    // Check if any BGM is playing in either slot, and then end it
    if (currentBGMIndex[0] < 0xFF) {
        endSound(currentBGMIndex[0]);
    }

    if (currentBGMIndex[1] < 0xFF) {
        endSound(currentBGMIndex[1]);
    }

    // Reset the indices to indicate no BGM is playing
    currentBGMIndex[0] = 0xFF;
    currentBGMIndex[1] = 0xFF;
}

void playBGM(u8 bgmIndex) {
	endCurrentBGM();

	int primaryTrack = bgmGroups[bgmIndex][0];
    int secondaryTrack = bgmGroups[bgmIndex][1];
    
	currentBGMIndex[0] = playNewSound(primaryTrack);
    
    if (secondaryTrack != BGM_SINGLE) {
        // Code to handle the secondary track if it exists
        currentBGMIndex[1] = playNewSound(secondaryTrack);
    } else
		currentBGMIndex[1] = 0xFF;
	
	// Update the volume for both BGM
	u8 finalVolume;

	if (currentBGMIndex[0] < 0xFF) {
		finalVolume = calculateFinalVolume(getAssetDefaultVolume(primaryTrack), options.bgmVolume, options.masterVolume);
		setAssetVolume(currentBGMIndex[0], finalVolume);
	}
	
	if (currentBGMIndex[1] < 0xFF) {
		finalVolume = calculateFinalVolume(getAssetDefaultVolume(secondaryTrack), options.bgmVolume, options.masterVolume);
		setAssetVolume(currentBGMIndex[1], finalVolume);
	}
}

void playSFX(u8 sfxID, int sfxGroupIndex) {
	stopSFX(sfxGroupIndex);
    currentSFXIndex[sfxGroupIndex] = playNewSound(sfxID);
    
    u8 masterVolume, sfxVolume;

    if (mDat.currMenuPage == MPI_OPTIONS) {
        // Read volumes directly from the options menu items
        optionsMenuItems = menuPages[MPI_OPTIONS].items;
        masterVolume = (u8)optionsMenuItems[OPTID_MASTER_VOL].data.intVal;
        sfxVolume = (u8)optionsMenuItems[OPTID_SFX_VOL].data.intVal;
    } else {
        // Use current options settings
        masterVolume = options.masterVolume;
        sfxVolume = options.sfxVolume;
    }
    
	masterVolume = (u8)optionsMenuItems[OPTID_MASTER_VOL].data.intVal;
	sfxVolume = (u8)optionsMenuItems[OPTID_SFX_VOL].data.intVal;

    // Calculate the final volume and set it for the SFX
	for (u8 sfxGroupIndex = 0; sfxGroupIndex < AUDGROUP_MAX; sfxGroupIndex++) {
		u8 finalVolume = calculateFinalVolume(getAssetDefaultVolume(sfxID), sfxVolume, masterVolume);
		setAssetVolume(currentSFXIndex[sfxGroupIndex], finalVolume);
	}
	// hacky af but it works lol
	//justLikeUpdateAllVolumesMan();
}

void justLikeUpdateAllVolumesMan(){
	u8 masterVolume = (u8)optionsMenuItems[OPTID_MASTER_VOL].data.intVal;
	u8 sfxVolume = (u8)optionsMenuItems[OPTID_SFX_VOL].data.intVal;

    // Calculate the final volume and set it for the SFX
	for (u8 sfxGroupIndex = 0; sfxGroupIndex < AUDGROUP_MAX; sfxGroupIndex++) {
		u8 finalVolume = calculateFinalVolume(getAssetDefaultVolume(currentSFXIndex[sfxGroupIndex]), sfxVolume, masterVolume);
		setAssetVolume(currentSFXIndex[sfxGroupIndex], finalVolume);
	}
}

void stopAllSoundExcept(const u8* exception) {
    // Stop all SFX except the one in the exception
    for (int i = 0; i < 5; i++) {
        if (&currentSFXIndex[i] != exception && currentSFXIndex[i] < 0xFF) {
            endSound(currentSFXIndex[i]);
            currentSFXIndex[i] = 0xFF;  // Reset the index to indicate the SFX has stopped
        }
    }

    // Stop all BGM except the one in the exception
    for (int i = 0; i < 2; i++) {
        if (&currentBGMIndex[i] != exception && currentBGMIndex[i] < 0xFF) {
            endSound(currentBGMIndex[i]);
            currentBGMIndex[i] = 0xFF;  // Reset the index to indicate the BGM has stopped
        }
    }
}

void stopAllSound() {
	endAllSound();
	for (u8 i = 0; i < sizeof(currentSFXIndex); i++){
		currentSFXIndex[i] = 0xFF;
	}
	currentBGMIndex[0] = 0xFF;
	currentBGMIndex[1] = 0xFF;
}

void stopSFX(u8 sfxGroupIndex){
	if (sfxIsPlaying(sfxGroupIndex)){
		endSound(currentSFXIndex[sfxGroupIndex]);
		currentSFXIndex[sfxGroupIndex] = 0xFF;
	}
}


void updateSoundVolumes(bool leavingOptionsMenu) {
    u8 masterVolume, bgmVolume, sfxVolume;

    if (mDat.currMenuPage == MPI_OPTIONS && !leavingOptionsMenu) {
        // Read volumes directly from the options menu items
        MenuPageItem *optionsMenuItems = menuPages[MPI_OPTIONS].items;
        masterVolume = (u8)optionsMenuItems[OPTID_MASTER_VOL].data.intVal;
        bgmVolume = (u8)optionsMenuItems[OPTID_BGM_VOL].data.intVal;
        sfxVolume = (u8)optionsMenuItems[OPTID_SFX_VOL].data.intVal;
    } else {
        // Use current options settings
        masterVolume = options.masterVolume;
        bgmVolume = options.bgmVolume;
        sfxVolume = options.sfxVolume;
    }

    // Update BGM volumes
    for (int i = 0; i < 2; i++) {
        if (currentBGMIndex[i] < 0xFF) {
            u8 finalVolume = calculateFinalVolume(getAssetDefaultVolume(currentBGMIndex[i]), bgmVolume, masterVolume);
            setAssetVolume(currentBGMIndex[i], finalVolume);
        }
    }

    // Update SFX volumes
    for (int i = 0; i < 5; i++) {
        if (currentSFXIndex[i] < 0xFF) {
            u8 finalVolume = calculateFinalVolume(getAssetDefaultVolume(currentSFXIndex[i]), sfxVolume, masterVolume);
            setAssetVolume(currentSFXIndex[i], finalVolume);
        }
    }
}

void resetMainMenuWindowVariables() {
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

	mDat.windowActionTimer = 0;
	mDat.windowActionTarget = 0;
	mDat.windowFinalizing = false;
	mDat.hideMenuCursor = false;

	mDat.evaLerpStart = 0;
	mDat.evaLerpEnd = 0;
	mDat.evbLerpStart = 0;
	mDat.evbLerpEnd = 0;

	mDat.menuBG.xPos = 0;
	mDat.menuBG.yPos = 0;

	mDat.zipSpeed = 3;
	mDat.wrappedAround = true;

	mDat.showPageWindowBG = false;
	
	mDat.secondaryNineSliceYOff = 2;

	mDat.updateSpriteDraw = false;
	mDat.updateBGTileDraw = false;
	mDat.updateUITileDraw = false;
	mDat.starryBG.snappedThisFrame = false;
}



// Compute SRAM address for a given saveSlot
u32 getSaveSlotAddress(u8 saveSlot) {
    return SRAM_BASE + (saveSlot * SRAM_BLOCK_SIZE);
}

// Compute SRAM address for the options
u32 getOptionsAddress(void) {
    return SRAM_BASE + (SAVE_SLOT_COUNT * SRAM_BLOCK_SIZE);
}

void loadGame(MapData *mapData, u8 saveSlot) {
    u32 address = getSaveSlotAddress(saveSlot);
    u8 *ptr = (u8*) mapData;
    for (size_t i = 0; i < sizeof(MapData); ++i) {
        ptr[i] = readSRAMByte(address + i);
    }
}

void loadOptions(Options *options) {
    u32 address = getOptionsAddress();
    u8 *ptr = (u8*) options;
    for (size_t i = 0; i < sizeof(Options); ++i) {
        ptr[i] = readSRAMByte(address + i);
    }
}

void saveGame(const MapData *mapData, u8 saveSlot) {
    u32 address = getSaveSlotAddress(saveSlot);
    const u8 *ptr = (const u8*) mapData;
    for (size_t i = 0; i < sizeof(MapData); ++i) {
        writeSRAMByte(address + i, ptr[i]);
    }
}

void saveOptions(const Options *options) {
    u32 address = getOptionsAddress();
    const u8 *ptr = (const u8*) options;
    for (size_t i = 0; i < sizeof(Options); ++i) {
        writeSRAMByte(address + i, ptr[i]);
    }
}


void initMapData(MapData *mapData) {
    memset(mapData, 0, sizeof(MapData)); // Set all bytes to zero
}


void initOptions(Options *options) {
    memset(options, 0, sizeof(Options)); // Set all bytes to zero
}

void writeSRAMByte(u32 address, u8 value) {
    *(volatile u8*)(address) = value;
}

u8 readSRAMByte(u32 address) {
    return *(volatile u8*)(address);
}