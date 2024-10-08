#include "intro.h"

IntroData introData;

Scene introScene = {
	.initialize = &introInitialize,
	.intro = 0,
	.normal = &introNormal,
	.pause = 0,
	.outro = 0,
	.end = &introEnd,
};

void introInitialize(){
	REG_DISPCNT = DCNT_MODE4 | DCNT_BG2;
	REG_BG2CNT = BG_8BPP | BG_PRIO(0); //health box layer
	REG_BLDCNT = BLD_TOP(BLD_BG2 | BLD_BACKDROP) | BLD_BLACK;
	REG_BLDY = BLDY(16);
	
	//disable every sprite
	for(u32 i = 0; i < 128; i++){
		objectBuffer[i].attr0 = ATTR0_HIDE;
	}
	
	OAMData.position = (void *)oam_mem;
	OAMData.buffer = objectBuffer;
	OAMData.size = sizeof(objectBuffer) >> 2;
	
	memcpy32(pal_bg_mem, Intro_GBAJamPal, sizeof(Intro_GBAJamPal) >> 2);
	memcpy32(m4_mem, Intro_GBAJamBitmap, sizeof(Intro_GBAJamBitmap) >> 2);
	
	introData.state = GBAJAM_ENTER;
	introData.actionTimer = 0;
	introData.actionTarget = 16;
	
	currentScene.state = NORMAL;

	options.firstTimeBoot = true;
	loadOptions(&options);
	clearEntireSRAM(&options); // also checks if it was the first time ever booting up the game before actually doing so
}

void introNormal(){
	switch(introData.state){
	case GBAJAM_ENTER:
		if(introData.actionTimer == introData.actionTarget){
			introData.state = GBAJAM_HOLD;
			introData.actionTimer = 0;
			introData.actionTarget = GBAJAM_WAIT_FRAMES;
		} else if (introData.actionTimer == 1){
			// Seems to be too slow on the same frame as loading SRAM?
			playBGM(BGM_OPENING);
			updateSoundVolumes(false);
			IOBuffer0[0] = 16 - introData.actionTimer;
			introData.actionTimer++;
		} else{
			IOBuffer0[0] = 16 - introData.actionTimer;
			introData.actionTimer++;
		}
		break;
	case GBAJAM_HOLD:
		if(introData.actionTimer == introData.actionTarget){
			introData.state = GBAJAM_LEAVE;
			introData.actionTimer = 0;
			introData.actionTarget = 16;
		}
		else{
			introData.actionTimer++;
		}
		IOBuffer0[0] = 0;
		break;
	case GBAJAM_LEAVE:
			if(introData.actionTimer == introData.actionTarget){
			introData.state = AUDIO_ENGINE_ENTER;
			introData.actionTimer = 0;
			introData.actionTarget = 17;
			memcpy32(pal_bg_mem, Intro_Audio_enginePal, sizeof(Intro_Audio_enginePal) >> 2);
			memcpy32(m4_mem, Intro_Audio_engineBitmap, sizeof(Intro_Audio_engineBitmap) >> 2);
		}
		else{
			IOBuffer0[0] = introData.actionTimer;
			introData.actionTimer++;
		}
		break;
	case AUDIO_ENGINE_ENTER:
		if(introData.actionTimer == introData.actionTarget){
			introData.state = AUDIO_ENGINE_HOLD;
			introData.actionTimer = 0;
			introData.actionTarget = AUDIO_BY_WAIT_FRAMES;
		}
		else{
			IOBuffer0[0] = 16 - introData.actionTimer;
			introData.actionTimer++;
		}
		break;
	case AUDIO_ENGINE_HOLD:
		if(introData.actionTimer == introData.actionTarget){
			introData.state = AUDIO_ENGINE_LEAVE;
			introData.actionTimer = 0;
			introData.actionTarget = 17;
		}
		else{
			introData.actionTimer++;
		}
		IOBuffer0[0] = 0;
		break;
	case AUDIO_ENGINE_LEAVE:
			if(introData.actionTimer == introData.actionTarget){
			introData.state = STUDIO_ENTER;
			introData.actionTimer = 0;
			introData.actionTarget = 16;
			memcpy32(pal_bg_mem, Intro_studioPal, sizeof(Intro_studioPal) >> 2);
			memcpy32(m4_mem, Intro_studioBitmap, sizeof(Intro_studioBitmap) >> 2);
		}
		else{
			IOBuffer0[0] = introData.actionTimer;
			introData.actionTimer++;
		}
		break;
	case STUDIO_ENTER:
		if(introData.actionTimer == introData.actionTarget){
			introData.state = STUDIO_HOLD;
			introData.actionTimer = 0;
			introData.actionTarget = PIXEL_PLAY_STUDIOS_WAIT_FRAMES;
		}
		else{
			IOBuffer0[0] = 16 - introData.actionTimer;
			introData.actionTimer++;
		}
		break;
	case STUDIO_HOLD:
		if(introData.actionTimer == introData.actionTarget){
			introData.state = STUDIO_LEAVE;
			introData.actionTimer = 0;
			introData.actionTarget = 17;
		}
		else{
			introData.actionTimer++;
		}
		IOBuffer0[0] = 0;
		break;
	case STUDIO_LEAVE:
		if(introData.actionTimer == introData.actionTarget){
			introData.state = BLACK_SCREEN_HOLD;
			introData.actionTimer = 0;
			introData.actionTarget = BLACK_SCREEN_HOLD_WAIT_FRAMES;
			memset32(pal_bg_mem, 0, 128);
			memset32(m4_mem, 0, 8192);
		}
		else{
			IOBuffer0[0] = introData.actionTimer;
			introData.actionTimer++;
		}
		break;
	case BLACK_SCREEN_HOLD:
		if(introData.actionTimer == introData.actionTarget){
			introData.state = BEFORE_FINAL_TRANSITION;
			introData.actionTimer = 0;
			introData.actionTarget = FINAL_TRANSITION_WAIT_FRAMES;
		}
		else{
			IOBuffer0[0] = introData.actionTimer;
			introData.actionTimer++;
		}
		IOBuffer0[0] = 0;
		break;
	case BEFORE_FINAL_TRANSITION:
		if(introData.actionTimer == introData.actionTarget){
			currentScene.state = END;
			introData.actionTimer = 0;
			introData.actionTarget = 16;
		}
		else{
			IOBuffer0[0] = introData.actionTimer;
			introData.actionTimer++;
		}
		break;
	}
	
	if((inputs.pressed & KEY_A) || (inputs.pressed & KEY_START)){
		currentScene.state = END;
	}
	
	IOData[0].position = (void *)&REG_BLDY;
	IOData[0].buffer = IOBuffer0;
	IOData[0].size = 1;
}

void introEnd(){
	currentScene.scenePointer = sceneList[MAIN_MENU];
	currentScene.state = INITIALIZE;
	endCurrentBGM();
}