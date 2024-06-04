#include "gameplay.h"

ShipData shipList[MAX_SHIPS] EWRAM_DATA;
u16 tilemapBuffer0[1024] EWRAM_DATA;
u16 tilemapBuffer1[1024] EWRAM_DATA;
u16 tilemapBuffer2[1024] EWRAM_DATA;
u16 tilemapBuffer3[1024] EWRAM_DATA;
MapData mapState EWRAM_DATA;


Scene gameplayScene = {
	.initialize = &gameplayInitialize,
	.intro = 0,
	.normal = &gameplayNormal,
	.pause = 0,
	.outro = 0,
	.end = &gameplayEnd,
};

void gameplayInitialize(){
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ;
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
	tilemapData[0].buffer = tilemapBuffer0;
	tilemapData[0].position = se_mem[BG_0_TILEMAP];
	
	//queue the tilemap for layer 1 to be sent
	tilemapData[1].size = 512;
	tilemapData[1].buffer = tilemapBuffer1;
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
	
	mapState.xPos = 0;
	mapState.yPos = 0;
	mapState.xLastPos = 0xffff;
	mapState.yLastPos = 0xffff;
	
	initMap();
	
	createShipTilemap(shipList, tilemapBuffer1, &mapState);
	
	playNewAsset(_AreaA_DMA_Only);
	currentScene.state = NORMAL;
}

void gameplayNormal(){

}

void gameplayEnd(){

}

void createShipTilemap(ShipData *shipList, u16 *tilemapBuffer, MapData *mapState){
	u8 xUpdateLow = 0; //lower bound of horizontal map cell that need to be updated
	u8 xUpdateHigh = 0; //upper bound of horizontal map cell that need to be updated
	u8 yUpdateLow = 0; //lower bound of vertical map cell that need to be updated
	u8 yUpdateHigh = 0; //upper bound of vertical map cell that need to be updated
	
	//convert pixel coordinates to map cell coordinates
	u8 mapXPos = mapState->xPos >> 4;
	u8 mapYPos = mapState->yPos >> 4; 
	u8 mapXLastPos = mapState->xLastPos >> 4;
	u8 mapYLastPos = mapState->yLastPos >> 4;

	//set the bound for x updates
	if(mapXPos > mapXLastPos){
		xUpdateLow = mapXLastPos + 16;
		xUpdateHigh = mapXPos + 16;
		
		if((xUpdateLow + 16) < xUpdateHigh){
			xUpdateLow = xUpdateHigh - 16;
		}
	}
	else if(mapXPos < mapXLastPos){
		xUpdateLow = mapXPos;
		xUpdateHigh = mapXLastPos;
		
		if((xUpdateLow + 16) < xUpdateHigh){
			xUpdateHigh = xUpdateLow + 16;
		}
	}
	
	//set the bounds for y updates
	if(mapYPos > mapYLastPos){
		yUpdateLow = mapYLastPos + 16;
		yUpdateHigh = mapYPos + 16;
		
		if((yUpdateLow + 16) < yUpdateHigh){
			yUpdateLow = yUpdateHigh - 16;
		}
	}
	else if(mapYPos < mapYLastPos){
		yUpdateLow = mapYPos;
		yUpdateHigh = mapYLastPos;
		
		if((yUpdateLow + 16) < yUpdateHigh){
			yUpdateHigh = yUpdateLow + 16;
		}
	}
	
	//clear the vertical column of newly arrived spaces
	for(int i = xUpdateLow; i < xUpdateHigh; i++){
		for(int j = 0; j < 16; j++){
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64] = 3;
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64 + 1] = 3;
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64 + 32] = 3;
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64 + 33] = 3;
		}
	}
	
	//draw every ship that should be on screen
	for(u32 shipIndex = 0; shipIndex < mapState->numShips; shipIndex++){
		u8 shipXPos = shipList[shipIndex].xPos;
		u8 shipYPos = shipList[shipIndex].yPos;
		
		//if this particular ship is in one of the update regions
		if(((shipXPos >= xUpdateLow) && (shipXPos < xUpdateHigh) && (shipYPos >= mapYPos) && (shipYPos <= mapYPos + 15))
		|| ((shipYPos >= yUpdateLow) && (shipYPos < yUpdateHigh) && (shipXPos >= mapXPos) && (shipXPos <= mapYPos + 15))){
			u16 baseIndex = (shipXPos % 16) * 2 + (shipYPos % 16) * 64;
			u8 tilemapBase = (shipList[shipIndex].type + 1) * 4;
			tilemapBuffer[baseIndex] = tilemapBase;
			tilemapBuffer[baseIndex + 1] = tilemapBase + 1;
			tilemapBuffer[baseIndex + 32] = tilemapBase + 2;
			tilemapBuffer[baseIndex + 33] = tilemapBase + 3;
		}
	}
	
	//update the map previous position
	mapState->xLastPos = mapState->xPos;
	mapState->yLastPos = mapState->yPos;
}

//a temprary function to initialize a test map.
void initMap(){
	mapState.numShips = 5;
	
	shipList[0].type = RED_SCOUT;
	shipList[0].state = READY;
	shipList[0].index = 0;
	shipList[0].health = 100;
	shipList[0].xPos = 0;
	shipList[0].yPos = 0;
	
	shipList[1].type = RED_BOMBER;
	shipList[1].state = READY;
	shipList[1].index = 1;
	shipList[1].health = 100;
	shipList[1].xPos = 5;
	shipList[1].yPos = 5;
	
	shipList[2].type = RED_DESTROYER;
	shipList[2].state = READY;
	shipList[2].index = 2;
	shipList[2].health = 100;
	shipList[2].xPos = 10;
	shipList[2].yPos = 10;
	
	shipList[3].type = RED_FIGHTER;
	shipList[3].state = READY;
	shipList[3].index = 3;
	shipList[3].health = 100;
	shipList[3].xPos = 5;
	shipList[3].yPos = 10;
	
	shipList[4].type = RED_CRUISER;
	shipList[4].state = READY;
	shipList[4].index = 4;
	shipList[4].health = 100;
	shipList[4].xPos = 10;
	shipList[4].yPos = 5;
}