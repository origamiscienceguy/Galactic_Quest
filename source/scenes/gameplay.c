#include "gameplay.h"

ShipData shipList[MAX_SHIPS] EWRAM_DATA;
u16 tilemapBuffer0[1024] EWRAM_DATA;
u16 tilemapBuffer1[1024] EWRAM_DATA;
u16 tilemapBuffer2[1024] EWRAM_DATA;
u16 tilemapBuffer3[1024] EWRAM_DATA;


Scene gameplayScene = {
	.initialize = &gameplayInitialize,
	.intro = 0,
	.normal = &gameplayNormal,
	.pause = 0,
	.outro = 0,
	.end = &gameplayEnd,
};

void gameplayInitialize(){
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | /*DCNT_BG1 |*/ DCNT_OBJ;
	REG_BG0CNT = BG_8BPP | BG_SBB(BG_0_TILEMAP) | BG_CBB(BG_0_CHARDATA);
	REG_BG1CNT = BG_8BPP | BG_SBB(BG_1_TILEMAP) | BG_CBB(BG_0_CHARDATA);
	
	//queue the palette to be sent
	paletteData[0].size = 128;
	paletteData[0].buffer = (void *)TestGfxPal;
	paletteData[0].position = pal_bg_mem;
	
	//queue the tiles to be sent
	characterData[0].size = 512;
	characterData[0].buffer = (void *)TestGfxTiles;
	characterData[0].position = tile8_mem[BG_0_CHARDATA];
	
	//queue the tilemap for layer 0 to be sent
	tilemapData[0].size = 512;
	tilemapData[0].buffer = (void *)tilemapBuffer0;
	tilemapData[0].position = se_mem[BG_0_TILEMAP];
	
	//queue the tilemap for layer 1 to be sent
	tilemapData[1].size = 512;
	tilemapData[1].buffer = (void *)tilemapBuffer1;
	tilemapData[1].position = se_mem[BG_1_TILEMAP];
	
	//fill the tilemap buffer for layer 0
	for(u32 i = 0; i < 16; i++){
		for(u32 j = 0; j < 16; j++){
			tilemapBuffer0[i * 64 + j * 2] = 0;
			tilemapBuffer0[i * 64 + j * 2 + 1] = 1;
			tilemapBuffer0[i * 64 + j * 2 + 32] = 2;
			tilemapBuffer0[i * 64 + j * 2 + 33] = 3;
		}
	}
	
	//fill the tilemap buffer for layer 1
	for(u32 i = 0; i < 16; i++){
		for(u32 j = 0; j < 16; j++){
			tilemapBuffer1[i * 64 + j * 2] = 1;
			tilemapBuffer1[i * 64 + j * 2 + 1] = 1;
			tilemapBuffer1[i * 64 + j * 2 + 32] = 1;
			tilemapBuffer1[i * 64 + j * 2 + 33] = 1;
		}
	}
	
	playNewAsset(_AreaA_DMA_Only);
	currentScene.state = NORMAL;
};

void gameplayNormal(){

};

void gameplayEnd(){

};