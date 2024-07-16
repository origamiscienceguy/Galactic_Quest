#include "gameplay.h"

u16 tilemapBuffer0[1024];
u16 tilemapBuffer1[1024];
u16 tilemapBuffer2[1024];
u16 tilemapBuffer3[1024];
u16 characterBuffer0[1024];
u16 characterBuffer1[1024];
OBJ_ATTR spriteBuffer[128];
u16 IOBuffer[30];

MapData mapData;


Scene gameplayScene = {
	.initialize = &gameplayInitialize,
	.intro = 0,
	.normal = &gameplayNormal,
	.pause = 0,
	.outro = 0,
	.end = &gameplayEnd,
};

void gameplayInitialize(){
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
	REG_BG0CNT = BG_4BPP | BG_SBB(BG_0_TILEMAP) | BG_CBB(BG_0_CHARDATA) | BG_PRIO(2); //grid layer
	REG_BG1CNT = BG_4BPP | BG_SBB(BG_1_TILEMAP) | BG_CBB(BG_1_CHARDATA) | BG_PRIO(0); //ship layer
	REG_BG2CNT = BG_4BPP | BG_SBB(BG_2_TILEMAP) | BG_CBB(BG_2_CHARDATA) | BG_PRIO(1); //highlight layer
	REG_BLDCNT = BLD_TOP(BLD_BG2) | BLD_BOT(BLD_BG0 | BLD_BACKDROP) | BLD_STD;
	REG_BLDALPHA = BLD_EVA(8) | BLD_EVB(8);
	
	//queue the palette to be sent
	paletteData[0].size = sizeof(bgGfxPal) >> 2;
	paletteData[0].buffer = (void *)bgGfxPal;
	paletteData[0].position = pal_bg_mem;
	
	paletteData[1].size = sizeof(bgGfxPal) >> 2;
	paletteData[1].buffer = (void *)bgGfxPal;
	paletteData[1].position = pal_obj_mem;
	
	//queue the tiles to be sent
	characterData[0].size = sizeof(bgGfxTiles) >> 2;
	characterData[0].buffer = (void *)bgGfxTiles;
	characterData[0].position = &tile8_mem[BG_0_CHARDATA];
	
	//queue the tilemap for layer 0 to be sent
	tilemapData[0].size = 512;
	tilemapData[0].buffer = tilemapBuffer0;
	tilemapData[0].position = &se_mem[BG_0_TILEMAP];
	
	//queue the tilemap for layer 1 to be sent
	tilemapData[1].size = 512;
	tilemapData[1].buffer = tilemapBuffer1;
	tilemapData[1].position = &se_mem[BG_1_TILEMAP];
	
	//fill the tilemap buffer for layer 0
	for(u32 i = 0; i < 16; i++){
		for(u32 j = 0; j < 16; j++){
			tilemapBuffer0[i * 64 + j * 2] = bgGfxMap[0];
			tilemapBuffer0[i * 64 + j * 2 + 1] = bgGfxMap[1];
			tilemapBuffer0[i * 64 + j * 2 + 32] = bgGfxMap[2];
			tilemapBuffer0[i * 64 + j * 2 + 33] = bgGfxMap[3];
		}
	}
	
	//send the graphics for the cursor
	characterData[1].size = sizeof(cursorTiles) >> 2;
	characterData[1].buffer = (void *)cursorTiles;
	characterData[1].position = &tile_mem_obj[0][CURSOR_GFX];
	
	//send the graphics for the highlights
	characterData[2].size = sizeof(HighlightTiles) >> 2;
	characterData[2].buffer = (void *)HighlightTiles;
	characterData[2].position = &tile_mem[BG_3_CHARDATA][HIGHLIGHT_OFFSET];
	
	//send the graphics for the minimap cursor
	characterData[3].size = sizeof(minimap_cursorTiles) >> 2;
	characterData[3].buffer = (void *)minimap_cursorTiles;
	characterData[3].position = &tile_mem_obj[0][MINIMAP_CURSOR_GFX];
	
	mapData.camera.xPos = 0;
	mapData.camera.yPos = 0;
	mapData.camera.state = CAM_STILL;
	mapData.xSize = 255;
	mapData.ySize = 255;
	mapData.state = TURN_START;
	mapData.teamTurn = RED_TEAM;
	mapData.selectedShip.index = 0;
	mapData.cursor.state = CUR_STILL;
	mapData.cursor.direction = CUR_NO_DIRECTION;
	mapData.cursor.counter = 0;
	
	
	//temporary function call to set up some ships like a saved scenareo would
	initMap();
	
	//set up all of the linked lists of every ship
	shipListInit();
	
	//generate the tilemap based on the camera position and the positions of all the ships
	createShipTilemap(tilemapBuffer1);
	
	currentScene.state = NORMAL;
}

void gameplayNormal(){
	//update the game for this tick
	switch(mapData.state){
	case TURN_START:
		turnStartState();
		break;
	case OPEN_MAP:
		openMapState();
		break;
	case SHIP_SELECTED:
		shipSelectedState();
		break;
	case SHIP_MOVEMENT_SELECT:
		shipMovementSelectState();
		break;
	case SHIP_MOVING:
		shipMovingState();
		break;
	case BATTLE:
		break;
	case TURN_END:
		turnEndState();
		break;
	case TURN_END_MOVEMENT:
		turnEndMovementState();
		break;
	case TURN_REPLAY:
		break;
	}
}

void gameplayEnd(){

}

void createShipTilemap(u16 *tilemapBuffer){	
	//convert pixel coordinates to map cell coordinates
	u8 mapXPos = mapData.camera.xPos >> 4;
	u8 mapYPos = mapData.camera.yPos >> 4; 
	
	//clear the tilemap
	memset32(tilemapBuffer, (bgGfxMap[3] | bgGfxMap[3] << 16), 512);
	
	//handle ship idle animations
	u8 globalIdleCounter; //0,2: center, 1: left, 3: right
	globalIdleCounter = (currentScene.sceneCounter & 0xC0) >> 6;
	if(globalIdleCounter == 2){
		globalIdleCounter = 0;
	}
	else if(globalIdleCounter == 3){
		globalIdleCounter = 2;
	}

	//if it is time to cycle to the next ship
	if((currentScene.sceneCounter & 0xFF) == 0){
		u8 handledList[256];
		for(u32 i = 0; i < 256; i++){
			handledList[i] = 0;
		}
		for(u32 shipIndex = 0; shipIndex < mapData.numShips; shipIndex++){
			if((mapData.ships[shipIndex].sameTileLink != shipIndex) && (handledList[shipIndex] == 0) && (isShipVisible(shipIndex))){
				//make the next ship visible
				makeShipVisible(mapData.ships[shipIndex].sameTileLink);
				//hide the current ship
				makeShipHidden(shipIndex);
				//mark all ships in this linked list as handled
				u8 checkingIndex = mapData.ships[shipIndex].sameTileLink;
				while(checkingIndex != shipIndex){
					handledList[checkingIndex]  = 1;
					checkingIndex = mapData.ships[checkingIndex].sameTileLink;
				}
			}
		}
	}

	//draw every ship that should be on screen
	for(u32 shipIndex = 0; shipIndex < mapData.numShips; shipIndex++){
		//if the ship is not visible, skip it
		if(!isShipVisible(shipIndex)){
			continue;
		}
		
		u8 shipXPos = mapData.ships[shipIndex].xPos;
		u8 shipYPos = mapData.ships[shipIndex].yPos;
		
		//if we are in the cycling animation
		if(((currentScene.sceneCounter & 0xFF) >= 0xF6) && (mapData.ships[shipIndex].sameTileLink != shipIndex) && 
		(shipXPos >= mapXPos) && (shipXPos < mapXPos + 16) && (shipYPos >= mapYPos) && (shipYPos < mapYPos + 16)){
			u16 baseIndex = (shipXPos % 16) * 2 + (shipYPos % 16) * 64;
			u16 tilemapBase = (CYCLE_GFX_START << 2) + (((currentScene.sceneCounter & 0xFF) - 0xF6) >> 1) * 4;
			tilemapBuffer[baseIndex] = bgGfxMap[tilemapBase];
			tilemapBuffer[baseIndex + 1] = bgGfxMap[tilemapBase + 1];
			tilemapBuffer[baseIndex + 32] = bgGfxMap[tilemapBase + 2];
			tilemapBuffer[baseIndex + 33] = bgGfxMap[tilemapBase + 3];
			continue;
		}
		
		s8 shipXVel = mapData.ships[shipIndex].xVel;
		s8 shipYVel = mapData.ships[shipIndex].yVel;
		u8 shipDirection; //0: right, 1: up, 2: left, 3: down
		
		if(ABS(shipXVel) > ABS(shipYVel)){
			if(shipXVel > 0){
				shipDirection = 0;
			}
			else{
				shipDirection = 2;
			}
		}
		else{
			if(shipYVel > 0){
				shipDirection = 3;
			}
			else{
				shipDirection = 1;
			}
		}
		
		//if this particular ship is in one of the update regions
		if((shipXPos >= mapXPos) && (shipXPos < mapXPos + 16) && (shipYPos >= mapYPos) && (shipYPos < mapYPos + 16)){
			u16 baseIndex = (shipXPos % 16) * 2 + (shipYPos % 16) * 64;
			u16 tilemapBase = ((mapData.ships[shipIndex].type + SHIP_GFX_START) * 4) + (globalIdleCounter * IDLE_CYCLE_OFFSET) + (shipDirection * DIRECTION_OFFSET);
			tilemapBuffer[baseIndex] = bgGfxMap[tilemapBase] | SE_PALBANK(mapData.ships[shipIndex].team);
			tilemapBuffer[baseIndex + 1] = bgGfxMap[tilemapBase + 1] | SE_PALBANK(mapData.ships[shipIndex].team);
			tilemapBuffer[baseIndex + 32] = bgGfxMap[tilemapBase + 2] | SE_PALBANK(mapData.ships[shipIndex].team);
			tilemapBuffer[baseIndex + 33] = bgGfxMap[tilemapBase + 3] | SE_PALBANK(mapData.ships[shipIndex].team);
		}
	}
}

void createGridTilemap(u16 *tilemapBuffer){
	//convert pixel coordinates to map cell coordinates
	u32 mapXPos = (mapData.camera.xPos >> 4);
	u32 mapYPos = (mapData.camera.yPos >> 4); 
	
	//clear the tilemap
	for(u32 i = 0; i < 16; i++){
		memset32(tilemapBuffer + (i * 64), bgGfxMap[GRID_GFX_START * 4 + 12] | (bgGfxMap[GRID_GFX_START * 4 + 13] << 16), 16);
		memset32(tilemapBuffer + (i * 64) + 32, bgGfxMap[GRID_GFX_START * 4 + 14] | (bgGfxMap[GRID_GFX_START * 4 + 15] << 16), 16);
	}
	
	u32 cycle = currentScene.sceneCounter % 256;
	
	//draw the diagonal
	for(u32 i = (mapXPos % 128); i < ((mapXPos % 128) + 16); i++){
		u32 xPos = i;
		u32 yPos = ((cycle >> 1) - i) % 128;
		
		if(((yPos - mapYPos) % 128) < 16){
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64] = bgGfxMap[GRID_GFX_START * 4 + (cycle % 2) * 4];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 1] = bgGfxMap[GRID_GFX_START * 4 + (cycle % 2) * 4 + 1];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 32] = bgGfxMap[GRID_GFX_START * 4 + (cycle % 2) * 4 + 2];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 33] = bgGfxMap[GRID_GFX_START * 4 + (cycle % 2) * 4 + 3];
			
			xPos--;
			
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64] = bgGfxMap[GRID_GFX_START * 4 + ((cycle % 2) + 2) * 4];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 1] = bgGfxMap[GRID_GFX_START * 4 + ((cycle % 2) + 2) * 4 + 1];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 32] = bgGfxMap[GRID_GFX_START * 4 + ((cycle % 2) + 2) * 4 + 2];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 33] = bgGfxMap[GRID_GFX_START * 4 + ((cycle % 2) + 2) * 4 + 3];
		}
	}
}
void drawSelectedShip(OBJ_ATTR *spriteBuffer){
	if(mapData.ships[mapData.selectedShip.index].state != SELECTED){
		spriteBuffer[SELECTED_SHIP_SPRITE].attr0 = ATTR0_HIDE;
		//clear the selected ships graphics from vram
		for(u32 tile = 0; tile < 16; tile++){
			u16 *VRAMPtr = &characterBuffer0[tile * 16];
			for(u32 i = 0; i < 16; i++){
				VRAMPtr[i] = 0;
			}
		}
		return;
	}
	
	//if the selected ship is offscreen, don't draw it
	if(((mapData.selectedShip.xPos - mapData.camera.xPos) < -24) || ((mapData.selectedShip.xPos - mapData.camera.xPos) > 248) || 
	((mapData.selectedShip.yPos - mapData.camera.yPos) < -24) || ((mapData.selectedShip.yPos - mapData.camera.yPos) > 168)){
		spriteBuffer[SELECTED_SHIP_SPRITE].attr0 = ATTR0_HIDE;
		return;
	}
	
	
	s32 shipYPos = (mapData.selectedShip.yPos - mapData.camera.yPos - 8) % 256; 
	s32 shipXPos = (mapData.selectedShip.xPos - mapData.camera.xPos - 8) % 256;
	
	if(shipXPos < 0){
		shipXPos = 512 + shipXPos;
	}
	if(shipYPos < 0){
		shipYPos = 256 + shipYPos;
	}
	
	spriteBuffer[SELECTED_SHIP_SPRITE].attr0 = ATTR0_AFF | ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(shipYPos);
	spriteBuffer[SELECTED_SHIP_SPRITE].attr1 = ATTR1_SIZE_32 | ATTR1_X(shipXPos) | ATTR1_AFF_ID(SELECTED_SHIP_AFFINE_MAT);
	spriteBuffer[SELECTED_SHIP_SPRITE].attr2 = ATTR2_ID(SELECTED_SHIP_GFX) | ATTR2_PRIO(0) | ATTR2_PALBANK(mapData.ships[mapData.selectedShip.index].team);
	
	//handle sprite rotation
	u32 angle = mapData.selectedShip.angle;
	spriteBuffer[SELECTED_SHIP_AFFINE_MAT * 4].fill = (sinTable[(angle + 0x40) % 256]) * 2;
	spriteBuffer[SELECTED_SHIP_AFFINE_MAT * 4 + 1].fill = (sinTable[angle % 256]) * 2;
	spriteBuffer[SELECTED_SHIP_AFFINE_MAT * 4 + 2].fill = -(sinTable[angle % 256]) * 2;
	spriteBuffer[SELECTED_SHIP_AFFINE_MAT * 4 + 3].fill = (sinTable[(angle + 0x40) % 256]) * 2;
	
	//load the graphics of the selected ship
	for(u32 tile = 0; tile < 16; tile++){
		cu16 *gfxPtr = &ships_selectedTiles[ships_selectedMap[tile] * 16];
		u16 *VRAMPtr = &characterBuffer0[tile * 16];
		//load the tile graphics
		for(u32 i = 0; i < 16; i++){
			VRAMPtr[i] = gfxPtr[i];
		}
		
	}
	
}

void drawCursor(){
	//if the cursor is hidden, don't draw anything.
	if(mapData.cursor.state == CUR_HIDDEN){
		spriteBuffer[CURSOR_SPRITE].attr0 = ATTR0_HIDE;
		return;
	}
	//if the cursor is offscreen, don't draw it
	if(((mapData.cursor.xPos - mapData.camera.xPos) < -24) || ((mapData.cursor.xPos - mapData.camera.xPos) > 248) || 
	((mapData.cursor.yPos - mapData.camera.yPos) < -24) || ((mapData.cursor.yPos - mapData.camera.yPos) > 168)){
		spriteBuffer[CURSOR_SPRITE].attr0 = ATTR0_HIDE;
		return;
	}
	
	u8 animationFrame = (currentScene.sceneCounter >> 5) % 2;
	
	s32 cursorYPos = (mapData.cursor.yPos - mapData.camera.yPos) % 256; 
	s32 cursorXPos = (mapData.cursor.xPos - mapData.camera.xPos) % 256;
	
	if(cursorXPos < 0){
		cursorXPos = 512 + cursorXPos;
	}
	if(cursorYPos < 0){
		cursorYPos = 256 + cursorYPos;
	}
	
	spriteBuffer[CURSOR_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE |  ATTR0_Y(cursorYPos);
	spriteBuffer[CURSOR_SPRITE].attr1 = ATTR1_SIZE_32 | ATTR1_X(cursorXPos);
	spriteBuffer[CURSOR_SPRITE].attr2 = ATTR2_ID(CURSOR_GFX + animationFrame * 16) | ATTR2_PRIO(0) | ATTR2_PALBANK(mapData.teamTurn);
}

void drawMinimap(){
	s16 minimap1YPos = MINIMAP_YPOS;
	s16 minimap1XPos = 0;
	s16 minimap2YPos = MINIMAP_YPOS;
	s16 minimap2XPos = 0;
	u8 useMinimap2 = 0;
	u8 state = mapData.minimap.state;
	
	//if the minimap is hidden, don't bother updating it
	if((state == MINIMAP_HIDDEN_LEFT) || (state == MINIMAP_HIDDEN_RIGHT)){
		spriteBuffer[MINIMAP1_SPRITE].attr0 = ATTR0_HIDE;
		spriteBuffer[MINIMAP2_SPRITE].attr0 = ATTR0_HIDE;
		spriteBuffer[MINIMAP_CURSOR_SPRITE].attr0 = ATTR0_HIDE;
		spriteBuffer[MINIMAP_CURSOR_SPRITE + 1].attr0 = ATTR0_HIDE;
		return;
	}
	
	//determine the position of the minimap
	if(state == MINIMAP_STILL_LEFT){
		minimap1XPos = -64 + minimapPositions[MINIMAP_MOVE_FRAMES - 1];
	}
	else if(state == MINIMAP_STILL_RIGHT){
		minimap1XPos = 240 - minimapPositions[MINIMAP_MOVE_FRAMES - 1];
	}
	else if(state == MINIMAP_MOVING_RIGHT){
		useMinimap2 = 1;
		minimap1XPos = -64 + (minimapPositions[MINIMAP_MOVE_FRAMES - 1] - minimapPositions[mapData.minimap.actionTimer]);
		minimap2XPos = 240 - minimapPositions[mapData.minimap.actionTimer];
		if((mapData.minimap.actionTimer + 1) == mapData.minimap.actionTarget){
			mapData.minimap.state = MINIMAP_STILL_RIGHT;
		}
		else{
			mapData.minimap.actionTimer++;
		}
	}
	else if(state == MINIMAP_MOVING_LEFT){
		useMinimap2 = 1;
		minimap1XPos = 240 - (minimapPositions[MINIMAP_MOVE_FRAMES - 1] - minimapPositions[mapData.minimap.actionTimer]);
		minimap2XPos = -64 + minimapPositions[mapData.minimap.actionTimer];
		if((mapData.minimap.actionTimer + 1) == mapData.minimap.actionTarget){
			mapData.minimap.state = MINIMAP_STILL_LEFT;
		}
		else{
			mapData.minimap.actionTimer++;
		}
	}
	else if(state == MINIMAP_HIDING_LEFT){
		minimap1XPos = -64 + minimapPositions[MINIMAP_MOVE_FRAMES - 1] - minimapPositions[mapData.minimap.actionTimer];
		if((mapData.minimap.actionTimer + 1) == mapData.minimap.actionTarget){
			mapData.minimap.state = MINIMAP_HIDDEN_LEFT;
		}
		else{
			mapData.minimap.actionTimer++;
		}
	}
	else if(state == MINIMAP_HIDING_RIGHT){
		minimap1XPos = 240 - (minimapPositions[MINIMAP_MOVE_FRAMES - 1] - minimapPositions[mapData.minimap.actionTimer]);
		if((mapData.minimap.actionTimer + 1) == mapData.minimap.actionTarget){
			mapData.minimap.state = MINIMAP_HIDDEN_RIGHT;
		}
		else{
			mapData.minimap.actionTimer++;
		}
	}
	else if(state == MINIMAP_EMERGING_LEFT){
		minimap1XPos = -64 + minimapPositions[mapData.minimap.actionTimer];
		if((mapData.minimap.actionTimer + 1) == mapData.minimap.actionTarget){
			mapData.minimap.state = MINIMAP_STILL_LEFT;
		}
		else{
			mapData.minimap.actionTimer++;
		}
	}
	else if(state == MINIMAP_EMERGING_RIGHT){
		minimap1XPos = 240 - minimapPositions[mapData.minimap.actionTimer];
		if((mapData.minimap.actionTimer + 1) == mapData.minimap.actionTarget){
			mapData.minimap.state = MINIMAP_STILL_RIGHT;
		}
		else{
			mapData.minimap.actionTimer++;
		}
	}
	
	//if there is an update request
	if(mapData.minimap.updateRequest){
		mapData.minimap.updateRequest = 0;
		updateMinimap();
	}
	
	//draw the sprite
	spriteBuffer[MINIMAP1_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE |  ATTR0_Y(minimap1YPos);
	spriteBuffer[MINIMAP1_SPRITE].attr1 = ATTR1_SIZE_64 | ATTR1_X(minimap1XPos);
	spriteBuffer[MINIMAP1_SPRITE].attr2 = ATTR2_ID(MINIMAP_GFX) | ATTR2_PRIO(0) | ATTR2_PALBANK(4);
	
	if(useMinimap2){
		spriteBuffer[MINIMAP2_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE |  ATTR0_Y(minimap2YPos);
		spriteBuffer[MINIMAP2_SPRITE].attr1 = ATTR1_SIZE_64 | ATTR1_X(minimap2XPos);
		spriteBuffer[MINIMAP2_SPRITE].attr2 = ATTR2_ID(MINIMAP_GFX ) | ATTR2_PRIO(0) | ATTR2_PALBANK(4);
	}
	else{
		spriteBuffer[MINIMAP2_SPRITE].attr0 = ATTR0_HIDE;
	}
	
	u32 xShiftAmount = 0;
	u32 yShiftAmount = 0;
	while((mapData.xSize >> xShiftAmount) > 64){
		xShiftAmount++;
	}
	while((mapData.ySize >> yShiftAmount) > 64){
		yShiftAmount++;
	}
	
	if((mapData.minimap.state != MINIMAP_STILL_LEFT) && (mapData.minimap.state != MINIMAP_STILL_RIGHT)){
		spriteBuffer[MINIMAP_CURSOR_SPRITE].attr0 = ATTR0_HIDE;
		spriteBuffer[MINIMAP_CURSOR_SPRITE + 1].attr0 = ATTR0_HIDE;
		return;
	}
	
	//draw the cursor that goes on top of the sprite
	if(currentScene.sceneCounter & 32){
		s32 cursorXPos = mapData.camera.xPos >> (4 + xShiftAmount);
		if(cursorXPos < 0){
			cursorXPos = 512 + cursorXPos;
		}
		s32 cursorYPos = mapData.camera.yPos >> (4 + yShiftAmount);
		spriteBuffer[MINIMAP_CURSOR_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE |  ATTR0_Y((minimap1YPos + cursorYPos) % 256);
		spriteBuffer[MINIMAP_CURSOR_SPRITE].attr1 = ATTR1_SIZE_8 | ATTR1_X((minimap1XPos + cursorXPos) % 512);
		spriteBuffer[MINIMAP_CURSOR_SPRITE].attr2 = ATTR2_ID(MINIMAP_CURSOR_GFX) | ATTR2_PRIO(0) | ATTR2_PALBANK(4);
		
		cursorXPos = ((mapData.camera.xPos + 240) >> (4 + xShiftAmount)) - 7;
		if(cursorXPos < 0){
			cursorXPos = 512 + cursorXPos;
		}
		cursorYPos = ((mapData.camera.yPos + 160) >> (4 + yShiftAmount)) - 7;
		spriteBuffer[MINIMAP_CURSOR_SPRITE + 1].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE |  ATTR0_Y((minimap1YPos + cursorYPos) % 256);
		spriteBuffer[MINIMAP_CURSOR_SPRITE + 1].attr1 = ATTR1_SIZE_8 | ATTR1_X((minimap1XPos + cursorXPos) % 512) | ATTR1_HFLIP | ATTR1_VFLIP;
		spriteBuffer[MINIMAP_CURSOR_SPRITE + 1].attr2 = ATTR2_ID(MINIMAP_CURSOR_GFX) | ATTR2_PRIO(0) | ATTR2_PALBANK(4);
	}
	else{
		s32 cursorXPos = mapData.camera.xPos >> (4 + xShiftAmount);
		if(cursorXPos < 0){
			cursorXPos = 512 + cursorXPos;
		}
		s32 cursorYPos = ((mapData.camera.yPos + 160) >> (4 + yShiftAmount)) - 7;
		spriteBuffer[MINIMAP_CURSOR_SPRITE].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE |  ATTR0_Y((minimap1YPos + cursorYPos) % 256);
		spriteBuffer[MINIMAP_CURSOR_SPRITE].attr1 = ATTR1_SIZE_8 | ATTR1_X((minimap1XPos + cursorXPos) % 512) | ATTR1_VFLIP;
		spriteBuffer[MINIMAP_CURSOR_SPRITE].attr2 = ATTR2_ID(MINIMAP_CURSOR_GFX) | ATTR2_PRIO(0) | ATTR2_PALBANK(4);
		
		cursorXPos = ((mapData.camera.xPos + 240) >> (4 + xShiftAmount)) - 7;
		if(cursorXPos < 0){
			cursorXPos = 512 + cursorXPos;
		}
		cursorYPos = mapData.camera.yPos >> (4 + yShiftAmount);
		spriteBuffer[MINIMAP_CURSOR_SPRITE + 1].attr0 = ATTR0_REG | ATTR0_4BPP | ATTR0_SQUARE |  ATTR0_Y((minimap1YPos + cursorYPos) % 256);
		spriteBuffer[MINIMAP_CURSOR_SPRITE + 1].attr1 = ATTR1_SIZE_8 | ATTR1_X((minimap1XPos + cursorXPos) % 512) | ATTR1_HFLIP;
		spriteBuffer[MINIMAP_CURSOR_SPRITE + 1].attr2 = ATTR2_ID(MINIMAP_CURSOR_GFX) | ATTR2_PRIO(0) | ATTR2_PALBANK(4);
	}
}

void updateMinimap(){
	//declare a bitmap buffer
	u8 minimapBuffer[2048];
	//clear the buffer
	memset32(minimapBuffer, 0x77777777, 512);
	
	//figure out how much to shift the coordinates by
	u32 xShiftAmount = 0;
	u32 yShiftAmount = 0;
	while((mapData.xSize >> xShiftAmount) > 64){
		xShiftAmount++;
	}
	while((mapData.ySize >> yShiftAmount) > 64){
		yShiftAmount++;
	}
	
	//go through each ship
	for(u32 shipIndex = 0; shipIndex < MAX_SHIPS; shipIndex++){
		u32 pixelXPos = mapData.ships[shipIndex].xPos >> xShiftAmount;
		u32 pixelYPos = mapData.ships[shipIndex].yPos >> yShiftAmount;
		
		u8 twoPixels = minimapBuffer[pixelYPos * 32 + (pixelXPos >> 1)];
		u8 existingColor;
		u32 color = 0;
		//determine what color the pixel should be.
		
		if(!(pixelXPos & 1)){
			existingColor = twoPixels & 0xf;
		}
		else{
			existingColor = (twoPixels & 0xf0) >> 4;
		}
		
		if(existingColor == 7){
			color = 1 + mapData.ships[shipIndex].team;
		}
		else if(existingColor != (1 + mapData.ships[shipIndex].team)){
			color = 6;
		}
		
		if(!(pixelXPos & 1)){
			existingColor = twoPixels & 0xf;
			twoPixels &= 0xf0;
			twoPixels |= color;
		}
		else{
			existingColor = (twoPixels & 0xf0) >> 4;
			twoPixels &= 0xf;
			twoPixels |= (color << 4);
		}
		
		minimapBuffer[pixelYPos * 32 + (pixelXPos >> 1)] = twoPixels;
	}
	
	//copy over the buffer into sprite vram, while converting from bitmap to tiles
	for(u32 yTile = 0; yTile < 8; yTile++){
		for(u32 xTile = 0; xTile < 8; xTile++){
			((u32 *)characterBuffer1)[yTile * 64 + xTile * 8] = ((u32 *)minimapBuffer)[yTile * 64 + xTile];
			((u32 *)characterBuffer1)[yTile * 64 + xTile * 8 + 1] = ((u32 *)minimapBuffer)[yTile * 64 + xTile + 8];
			((u32 *)characterBuffer1)[yTile * 64 + xTile * 8 + 2] = ((u32 *)minimapBuffer)[yTile * 64 + xTile + 16];
			((u32 *)characterBuffer1)[yTile * 64 + xTile * 8 + 3] = ((u32 *)minimapBuffer)[yTile * 64 + xTile + 24];
			((u32 *)characterBuffer1)[yTile * 64 + xTile * 8 + 4] = ((u32 *)minimapBuffer)[yTile * 64 + xTile + 32];
			((u32 *)characterBuffer1)[yTile * 64 + xTile * 8 + 5] = ((u32 *)minimapBuffer)[yTile * 64 + xTile + 40];
			((u32 *)characterBuffer1)[yTile * 64 + xTile * 8 + 6] = ((u32 *)minimapBuffer)[yTile * 64 + xTile + 48];
			((u32 *)characterBuffer1)[yTile * 64 + xTile * 8 + 7] = ((u32 *)minimapBuffer)[yTile * 64 + xTile + 56];
		}
	}
	
	characterData[1].buffer = characterBuffer1;
	characterData[1].size = 512;
	characterData[1].position = &tile_mem_obj[0][MINIMAP_GFX];
}

void drawHighlight(u16 *tilemapBuffer){

	//convert pixel coordinates to map cell coordinates
	s32 mapXPos = mapData.camera.xPos >> 4;
	s32 mapYPos = mapData.camera.yPos >> 4; 
	u32 shipIndex = mapData.selectedShip.index;
	s32 xTarget = mapData.ships[shipIndex].xPos + mapData.ships[shipIndex].xVel;
	s32 yTarget = mapData.ships[shipIndex].yPos + mapData.ships[shipIndex].yVel;
	//18 X 13 buffer, representing the 16 X 11 tiles on screen, plus one outer unseen ring
	u32 drawBuffer[236];
	
	//clear the tilemap
	memset32(tilemapBuffer, (bgGfxMap[3] | bgGfxMap[3] << 16), 512);
	
	//don't draw a highlight if there is none
	if(mapData.highlight.state == NO_HIGHLIGHT){
		return;
	}
	
	//clear drawBuffer
	memset32(drawBuffer, 0, 236);
	
	//draw the movement range of one ship
	if(mapData.highlight.state == MOVEMENT_RANGE_HIGHLIGHT){
		//designate bottom portion of ship movement range
		for(s32 i = 1; i <= SHIP_ACC; i++){
			if(((yTarget + i) < (mapYPos - 1)) || ((yTarget + i) >= (mapYPos + 11))){
				continue;
			}
			for(s32 j = -(SHIP_ACC - i); j <= (SHIP_ACC - i); j++){
				//if this highlight region is in range of the camera
				if(((xTarget + j) >= (mapXPos - 1)) && ((xTarget + j) < (mapXPos + 17))){
					drawBuffer[((xTarget - mapXPos) + j + 1) + (((yTarget - mapYPos) + i + 1) * 18)] = 1;
				}
			}
		}
		//designate the top portion of ship movement range
		for(s32 i = -1; i >= -SHIP_ACC; i--){
			if(((yTarget + i) < (mapYPos - 1)) || ((yTarget + i) >= (mapYPos + 11))){
				continue;
			}
			for(s32 j = -(SHIP_ACC + i); j <= (SHIP_ACC + i); j++){
				//if this highlight region is in range of the camera
				if(((xTarget + j) >= (mapXPos - 1)) && ((xTarget + j) < (mapXPos + 17))){
					drawBuffer[((xTarget - mapXPos) + j + 1) + (((yTarget - mapYPos) + i + 1) * 18)] = 1;
				}
			}
		}
		//designate the middle strip of ship movement range
		if(((yTarget) >= (mapYPos - 1)) && ((yTarget) < (mapYPos + 11))){
			for(s32 j = -SHIP_ACC; j <= SHIP_ACC; j++){
				//if this highlight region is in range of the camera
				if(((xTarget + j) >= (mapXPos - 1)) && ((xTarget + j) < (mapXPos + 17))){
					drawBuffer[((xTarget - mapXPos) + j + 1) + (((yTarget - mapYPos) + 1) * 18)] = 1;
				}
			}
		}
	}
	
	//send the highlight tiles to the buffer if this tile is in range.
	for(u32 i = 0; i < 16; i++){
		for(u32 j = 0; j < 11; j++){
			if(drawBuffer[(i + 1) + (j + 1) * 18] == 1){
				u32 baseIndex = (i % 16) * 2 + (j % 16) * 64;
				//figure out which corners and edges are touching
				u32 tilemapBase = 0;
				if(drawBuffer[(i + 2) + (j + 1) * 18]){
					tilemapBase += 1;
				}
				if(drawBuffer[(i + 2) + (j) * 18]){
					tilemapBase += 2;
				}
				if(drawBuffer[(i + 1) + (j) * 18]){
					tilemapBase += 4;
				}
				if(drawBuffer[(i) + (j) * 18]){
					tilemapBase += 8;
				}
				if(drawBuffer[(i) + (j + 1) * 18]){
					tilemapBase += 16;
				}
				if(drawBuffer[(i) + (j + 2) * 18]){
					tilemapBase += 32;
				}
				if(drawBuffer[(i + 1) + (j + 2) * 18]){
					tilemapBase += 64;
				}
				tilemapBase *= 4;
				
				tilemapBuffer[baseIndex] = (HighlightMap[tilemapBase] + HIGHLIGHT_OFFSET) | SE_PALBANK(4);
				tilemapBuffer[baseIndex + 1] = (HighlightMap[tilemapBase + 1] + HIGHLIGHT_OFFSET) | SE_PALBANK(4);
				tilemapBuffer[baseIndex + 32] = (HighlightMap[tilemapBase + 2] + HIGHLIGHT_OFFSET) | SE_PALBANK(4);
				tilemapBuffer[baseIndex + 33] = (HighlightMap[tilemapBase + 3] + HIGHLIGHT_OFFSET) | SE_PALBANK(4);
			}
		}
	}
}
void turnStartState(){
	TeamData *teamPointer = &mapData.teams[mapData.teamTurn];
	u32 numAliveShips = 0;
	
	//find the first active ship on this team and count how many are alive
	u32 shipIndex = teamPointer->firstShip;
	while(mapData.ships[shipIndex].teamLink != teamPointer->firstShip){
		if(mapData.ships[shipIndex].state == WRONG_TEAM_VISIBLE){
			mapData.ships[shipIndex].state = READY_VISIBLE;
			//if this is the first alive ship found
			if(numAliveShips == 0){
				mapData.selectedShip.index = shipIndex;
			}
			numAliveShips++;
		}
		else if(mapData.ships[shipIndex].state == WRONG_TEAM_HIDDEN){
			mapData.ships[shipIndex].state = READY_HIDDEN;
			//if this is the first alive ship found
			if(numAliveShips == 0){
				mapData.selectedShip.index = shipIndex;
			}
			numAliveShips++;
		}
		shipIndex = mapData.ships[shipIndex].teamLink;
	}
	if(mapData.ships[shipIndex].state == WRONG_TEAM_VISIBLE){
		mapData.ships[shipIndex].state = READY_VISIBLE;
		//if this is the first alive ship found
		if(numAliveShips == 0){
			mapData.selectedShip.index = shipIndex;
		}
		numAliveShips++;
	}
	else if(mapData.ships[shipIndex].state == WRONG_TEAM_HIDDEN){
		mapData.ships[shipIndex].state = READY_HIDDEN;
		//if this is the first alive ship found
		if(numAliveShips == 0){
			mapData.selectedShip.index = shipIndex;
		}
		numAliveShips++;
	}
	
	teamPointer->numAliveShips = numAliveShips;
	
	mapData.state = OPEN_MAP;
}

void openMapState(){
	
	moveCursor();
	
	//L and R cycle backwards or forwards through the active ships for this team, and center the camera on the next ship in the cycle
	checkCycleButtons();
	
	//if a is pressed, select the ship that is under the cursor
	if(inputs.pressed & KEY_A){
		//find which active ship on this team is at the cursor's select location
		u8 currentIndex = mapData.teams[mapData.teamTurn].firstShip;
		u16 selectedIndex = 0xffff;
		while(mapData.ships[currentIndex].teamLink != mapData.teams[mapData.teamTurn].firstShip){
			if((mapData.ships[currentIndex].xPos == mapData.cursor.selectXPos) && (mapData.ships[currentIndex].yPos == mapData.cursor.selectYPos) && 
			((mapData.ships[currentIndex].state == READY_VISIBLE) || (mapData.ships[currentIndex].state == READY_HIDDEN))){
				selectedIndex = currentIndex;
				break;
			}
			currentIndex = mapData.ships[currentIndex].teamLink;
		}
		if(selectedIndex != 0xffff){
			selectShip(selectedIndex);
		}
	}
	
	//if start is pressed, transition to the end turn sequence
	if(inputs.pressed & KEY_START){
		mapData.state = TURN_END;
		mapData.cursor.state = CUR_HIDDEN;
		mapData.actionTimer = 0;
	}
	
	if(((mapData.cursor.xPos - mapData.camera.xPos) < 64) && (mapData.minimap.state == MINIMAP_STILL_LEFT)){
		mapData.minimap.state = MINIMAP_MOVING_RIGHT;
		mapData.minimap.actionTarget = 4;
		mapData.minimap.actionTimer = 0;
	}
	if(((mapData.cursor.xPos - mapData.camera.xPos) > 144) && (mapData.minimap.state == MINIMAP_STILL_RIGHT)){
		mapData.minimap.state = MINIMAP_MOVING_LEFT;
		mapData.minimap.actionTarget = 4;
		mapData.minimap.actionTimer = 0;
	}
	
	//if the minimap is hidden, start bringing it visible again
	if((mapData.minimap.state == MINIMAP_HIDDEN_LEFT) || (mapData.minimap.state == MINIMAP_HIDDEN_RIGHT)){
		revealMinimap();
	}
	
	//handle any changes to the camera that occured this frame
	processCamera();
}

void shipSelectedState(){

	//calculate the ship's intented next position
	u8 shipIndex = mapData.selectedShip.index;
	s16 xTarget = mapData.ships[shipIndex].xPos + mapData.ships[shipIndex].xVel;
	s16 yTarget = mapData.ships[shipIndex].yPos + mapData.ships[shipIndex].yVel;
	
	//set the cursor to this ship's current trajectory
	mapData.cursor.state = CUR_STILL;
	mapData.cursor.xPos = (xTarget << 4) - 8;
	mapData.cursor.yPos = (yTarget << 4) - 8;
	mapData.cursor.selectXPos = xTarget;
	mapData.cursor.selectYPos = yTarget;

	//pan to that location
	cameraPanInit((xTarget << 4) - 112, (yTarget << 4) - 72, CYCLE_PAN_SPEED);
	
	mapData.state = SHIP_MOVEMENT_SELECT;
}

void shipMovementSelectState(){
	//draw the highlighted region of movement for the selected ship
	mapData.highlight.state = MOVEMENT_RANGE_HIGHLIGHT;
	u8 shipIndex = mapData.selectedShip.index;
	
	s16 xTarget = mapData.ships[shipIndex].xPos + mapData.ships[shipIndex].xVel;
	s16 yTarget = mapData.ships[shipIndex].yPos + mapData.ships[shipIndex].yVel;
	
	//L and R cycle backwards or forwards through the active ships for this team, and center the camera on the next ship in the cycle
	if((inputs.pressed & KEY_L) || (inputs.pressed & KEY_R)){
		mapData.ships[mapData.selectedShip.index].state = READY_VISIBLE; 
		mapData.cursor.selectXPos = mapData.ships[mapData.selectedShip.index].xPos;
		mapData.cursor.selectYPos = mapData.ships[mapData.selectedShip.index].yPos;
		checkCycleButtons();
	}
	
	//b canceles the movement of this ship
	if(inputs.pressed & KEY_B){
		mapData.state = OPEN_MAP;
		mapData.ships[mapData.selectedShip.index].state = READY_VISIBLE; 
		mapData.highlight.state = NO_HIGHLIGHT;
		mapData.cursor.selectXPos = mapData.ships[mapData.selectedShip.index].xPos;
		mapData.cursor.selectYPos = mapData.ships[mapData.selectedShip.index].yPos;
		mapData.cursor.xPos = (mapData.ships[mapData.selectedShip.index].xPos << 4) - 8;
		mapData.cursor.yPos = (mapData.ships[mapData.selectedShip.index].yPos << 4) - 8; 
	}
	
	//arrows will move the cursor within the movement range
	u8 cursorLastXPos = mapData.cursor.selectXPos;
	u8 cursorLastYPos = mapData.cursor.selectYPos;
	moveCursor();
	
	//cancel the cursor movement if it leaves the ship's movement range.
	if((ABS(mapData.cursor.selectXPos - xTarget) + ABS(mapData.cursor.selectYPos - yTarget)) > SHIP_ACC){
		mapData.cursor.selectXPos = cursorLastXPos;
		mapData.cursor.selectYPos = cursorLastYPos;
		mapData.cursor.xPos = (cursorLastXPos << 4) - 8;
		mapData.cursor.yPos = (cursorLastYPos << 4) - 8;
		mapData.cursor.state = CUR_STILL;
		mapData.cursor.counter = 0;
	}
	
	mapData.selectedShip.angle = arctan2(mapData.cursor.selectXPos - mapData.ships[shipIndex].xPos, mapData.cursor.selectYPos - mapData.ships[shipIndex].yPos);
	
	//pressing A starts a ship movement
	if(inputs.pressed & KEY_A){
		//start a movement of the selected ship
		
		s16 xTarget = (mapData.cursor.selectXPos) << 4;
		s16 yTarget = (mapData.cursor.selectYPos) << 4;
		
		mapData.ships[mapData.selectedShip.index].xVel = mapData.cursor.selectXPos - mapData.ships[mapData.selectedShip.index].xPos;
		mapData.ships[mapData.selectedShip.index].yVel = mapData.cursor.selectYPos - mapData.ships[mapData.selectedShip.index].yPos;
		
		mapData.ships[mapData.selectedShip.index].state = SELECTED;
		mapData.highlight.state = NO_HIGHLIGHT;
		mapData.state = SHIP_MOVING;
		mapData.actionTimer = 0;
		shipMoveInit(xTarget, yTarget, SHIP_MOVE_SPEED);
	}
	
	//handle any changes to the camera that occured this frame
	processCamera();
}

void shipMovingState(){
	//if the ship movement is not done yet
	if(mapData.actionTimer == 0){
		processShipMovement();
	}
	//if the ship movement is finished
	else{
		mapData.ships[mapData.selectedShip.index].xPos += mapData.ships[mapData.selectedShip.index].xVel;
		mapData.ships[mapData.selectedShip.index].yPos += mapData.ships[mapData.selectedShip.index].yVel;
		mapData.ships[mapData.selectedShip.index].state = FINISHED_VISIBLE;
		mapData.ships[mapData.selectedShip.index].sameTileLink = mapData.selectedShip.index;
		checkForOverlap(mapData.selectedShip.index);
		mapData.state = OPEN_MAP;
		mapData.actionTimer = 0;
		updateMinimap();
	}

	//handle any changes to the camera that occured this frame
	processCamera();
}
void turnEndState(){	
	//if we need to move to a new ship
	if(mapData.actionTimer == 0){
		//find the first ship on this team that is still active
		u32 shipIndex = mapData.teams[mapData.teamTurn].firstShip;
		while((mapData.ships[shipIndex].state != READY_VISIBLE) && (mapData.ships[shipIndex].state != READY_HIDDEN)){
			//if we have looped around to the original ship, all ships have finished movement.
			if(mapData.ships[shipIndex].teamLink == mapData.teams[mapData.teamTurn].firstShip){
				nextPlayer();
				processCamera();
				return;
			}
			shipIndex = mapData.ships[shipIndex].teamLink;
		}
		mapData.selectedShip.index = shipIndex;
		//pan the camera to this ship
		s16 xTarget = (mapData.ships[mapData.selectedShip.index].xPos << 4) - 112;
		s16 yTarget = (mapData.ships[mapData.selectedShip.index].yPos << 4) - 72;
		cameraPanInit(xTarget, yTarget, CYCLE_PAN_SPEED);
		mapData.actionTimer = 1;
	}
	//if the camera has finished focusing on the next ship
	else if(mapData.camera.state == CAM_STILL){
		//start a movement of the selected ship
		s16 xTarget = (mapData.ships[mapData.selectedShip.index].xPos + mapData.ships[mapData.selectedShip.index].xVel) << 4;
		s16 yTarget = (mapData.ships[mapData.selectedShip.index].yPos + mapData.ships[mapData.selectedShip.index].yVel) << 4;
		
		mapData.ships[mapData.selectedShip.index].state = SELECTED;
		mapData.state = TURN_END_MOVEMENT;
		shipMoveInit(xTarget, yTarget, SHIP_MOVE_SPEED);
	}
	
	//handle any changes to the camera that occured this frame
	processCamera();
}

void turnEndMovementState(){
	//if the ship movement is not done yet
	if(mapData.actionTimer == 0){
		processShipMovement();
	}
	//if the ship movement is finished
	else{
		mapData.ships[mapData.selectedShip.index].xPos += mapData.ships[mapData.selectedShip.index].xVel;
		mapData.ships[mapData.selectedShip.index].yPos += mapData.ships[mapData.selectedShip.index].yVel;
		mapData.ships[mapData.selectedShip.index].state = FINISHED_VISIBLE;
		mapData.ships[mapData.selectedShip.index].sameTileLink = mapData.selectedShip.index;
		checkForOverlap(mapData.selectedShip.index);
		mapData.state = TURN_END;
		mapData.actionTimer = 0;
	}
	
	//handle any changes to the camera that occured this frame
	processCamera();
}

void shipListInit(){
	//reset to 0 turns elapsed
	mapData.turnNum = 0;

	//initialize all four teams
	for(u32 teamIndex = 0; teamIndex < NUM_TEAMS; teamIndex++){
		TeamData *teamPointer = &mapData.teams[teamIndex];
		teamPointer->state = TEAM_ABSENT;
		teamPointer->numStartingShips = 0;
	}
	
	u32 lastShipIndex[NUM_TEAMS]; //the previous found ship for this particular team
	u32 numShips = 0;
	
	//check every ship
	for(u32 shipIndex = 0; shipIndex < MAX_SHIPS; shipIndex++){
		ShipData *shipPointer = &mapData.ships[shipIndex];
		u32 shipTeam = shipPointer->team;
		TeamData *teamPointer = &mapData.teams[shipTeam];
		
		//all active ships must be initialized with the ready_visible state
		if(shipPointer->state != READY_VISIBLE){
			continue;
		}
		
		numShips++;
		
		//increment the number of ships for the team this ship belongs to
		teamPointer->numStartingShips++;
		//if this was the first ship for a particular team, activate that team
		if(teamPointer->state == TEAM_ABSENT){
			teamPointer->state = TEAM_ACTIVE;
			teamPointer->firstShip = shipIndex;
		}
		//if this team has other ships as well, link this ship to the previous to extend the linked list
		else{
			mapData.ships[lastShipIndex[shipTeam]].teamLink = shipIndex;
		}
		lastShipIndex[shipTeam] = shipIndex;
		
		mapData.ships[shipIndex].sameTileLink = shipIndex;
		
		//check if any of the other ships before this one have the same exact position on the map
		u32 xPos = mapData.ships[shipIndex].xPos;
		u32 yPos = mapData.ships[shipIndex].yPos;
		for(s32 overlapShipIndex = shipIndex - 1; overlapShipIndex >= 0; overlapShipIndex--){
			if((mapData.ships[overlapShipIndex].xPos == xPos) && (mapData.ships[overlapShipIndex].yPos == yPos)){
				mapData.ships[overlapShipIndex].sameTileLink = shipIndex;
				mapData.ships[shipIndex].state = READY_HIDDEN;
				break;
			}
		}
	}
	mapData.numShips = 112;
	
	//complete the linked list loop for each team that has at least one ship
	for(u32 teamIndex = 0; teamIndex < NUM_TEAMS; teamIndex++){
		TeamData *teamPointer = &mapData.teams[teamIndex];
		if(teamPointer->state == TEAM_ABSENT){
			continue;
		}
		
		mapData.ships[lastShipIndex[teamIndex]].teamLink = teamPointer->firstShip;
	}
	
	//complete the linked list loops for every group of ships that share a tile
	for(u32 shipIndex = 0; shipIndex < MAX_SHIPS; shipIndex++){
		//if it points to itself, this ship does not share a tile with any others
		if(mapData.ships[shipIndex].sameTileLink == shipIndex){
			continue;
		}
		//if this ship is not active, skip it.
		if(mapData.ships[shipIndex].state != READY_VISIBLE){
			continue;
		}
		
		u32 nextIndex = mapData.ships[shipIndex].sameTileLink;
		//otherwise, follow this linked list until we reach the end, and then connect the end to this front.
		while(mapData.ships[nextIndex].sameTileLink != shipIndex){
			//if we find a ship on this chain that points to itself, it is the last ship in the chain
			if(mapData.ships[nextIndex].sameTileLink == nextIndex){
				mapData.ships[nextIndex].sameTileLink = shipIndex;
				break;
			}
			nextIndex = mapData.ships[nextIndex].sameTileLink;
		}
	}
	
	//find out which team is going first
	for(u32 team = 0; team < NUM_TEAMS; team++){
		if(mapData.teams[team].state == TEAM_ACTIVE){
			mapData.teamTurn = team;
			break;
		}
	}

	//all teams that aren't currently in their turn have their ships set to wrong_team
	for(u32 team = 0; team < NUM_TEAMS; team++){
		if(team == mapData.teamTurn){
			continue;
		}
		u32 shipIndex = mapData.teams[team].firstShip;
		while(mapData.ships[shipIndex].teamLink != mapData.teams[team].firstShip){
			if(mapData.ships[shipIndex].state == READY_VISIBLE){
				mapData.ships[shipIndex].state = WRONG_TEAM_VISIBLE;
			}
			else if(mapData.ships[shipIndex].state == READY_HIDDEN){
				mapData.ships[shipIndex].state = WRONG_TEAM_HIDDEN;
			}
			shipIndex = mapData.ships[shipIndex].teamLink;
		}
		if(mapData.ships[shipIndex].state == READY_VISIBLE){
			mapData.ships[shipIndex].state = WRONG_TEAM_VISIBLE;
		}
		else if(mapData.ships[shipIndex].state == READY_HIDDEN){
			mapData.ships[shipIndex].state = WRONG_TEAM_HIDDEN;
		}
	}
	
	//pan the camera to the first ship
	mapData.selectedShip.index = mapData.teams[mapData.teamTurn].firstShip;
	s16 xTarget = (mapData.ships[mapData.selectedShip.index].xPos << 4) - 112;
	s16 yTarget = (mapData.ships[mapData.selectedShip.index].yPos << 4) - 72;
	cameraPanInit(xTarget, yTarget, CYCLE_PAN_SPEED);
	
	//setup the cursor
	mapData.cursor.xPos = xTarget + 104;
	mapData.cursor.yPos = yTarget + 64;
	mapData.cursor.selectXPos = (mapData.cursor.xPos >> 4) + 1;
	mapData.cursor.selectYPos = (mapData.cursor.yPos >> 4) + 1;
	
	//set no highlight
	mapData.highlight.state = NO_HIGHLIGHT;
	
	//enable the minimap
	mapData.minimap.state = MINIMAP_HIDDEN_LEFT;
	mapData.minimap.updateRequest = 1;
}

void nextPlayer(){
	u32 team = mapData.teamTurn;
	//set all ships on this team as wrong_team
	u32 shipIndex = mapData.teams[team].firstShip;
	while(mapData.ships[shipIndex].teamLink != mapData.teams[team].firstShip){
		if(mapData.ships[shipIndex].state == FINISHED_VISIBLE){
			mapData.ships[shipIndex].state = WRONG_TEAM_VISIBLE;
		}
		else if(mapData.ships[shipIndex].state == FINISHED_HIDDEN){
			mapData.ships[shipIndex].state = WRONG_TEAM_HIDDEN;
		}
		shipIndex = mapData.ships[shipIndex].teamLink;
	}
	if(mapData.ships[shipIndex].state == FINISHED_VISIBLE){
		mapData.ships[shipIndex].state = WRONG_TEAM_VISIBLE;
	}
	else if(mapData.ships[shipIndex].state == FINISHED_HIDDEN){
		mapData.ships[shipIndex].state = WRONG_TEAM_HIDDEN;
	}
	mapData.state = TURN_START;
	//find the next team
	do{
		team++;
		//if all teams have gone, go to next turn
		if(team >= NUM_TEAMS){
			team = 0;
			nextTurn();
		}
	}while(mapData.teams[team].state != TEAM_ACTIVE);
	
	mapData.teamTurn = team;
	
	mapData.state = TURN_START;
}

void nextTurn(){
	mapData.turnNum++;
}

void processCamera(){
	
	//handle the camera based on its state
	switch(mapData.camera.state){
	case CAM_STILL:
		break;
	case CAM_PANNING:
		processCameraPan();
		break;
	case CAM_TRACKING:
		break;
	}
	
	//bounds check the cursor
	cursorBoundsCheck();

	//bounds check the camera
	cameraBoundsCheck(&mapData.camera.xPos, &mapData.camera.yPos);
	
	//update the tilemap with the new position
	createShipTilemap(tilemapBuffer1);
	
	//update the grid
	createGridTilemap(tilemapBuffer0);
	
	//process the selected ship's sprite if applicable
	drawSelectedShip(spriteBuffer);
	
	//draw the cursor if applicable
	drawCursor();
	
	//draw the highlight layer
	drawHighlight(tilemapBuffer2);
	
	//draw the minimap
	drawMinimap();
	
	//queue the tilemap for layer 1 to be sent
	tilemapData[1].size = 512;
	tilemapData[1].buffer = tilemapBuffer0;
	tilemapData[1].position = se_mem[BG_0_TILEMAP];
	
	//queue the tilemap for layer 0 to be sent
	tilemapData[0].size = 512;
	tilemapData[0].buffer = tilemapBuffer1;
	tilemapData[0].position = se_mem[BG_1_TILEMAP];
	
	//queue the tilemap for layer 2 to be sent
	tilemapData[2].size = 512;
	tilemapData[2].buffer = tilemapBuffer2;
	tilemapData[2].position = se_mem[BG_2_TILEMAP];
	
	//queue the character data for the sprites
	characterData[0].buffer = characterBuffer0;
	characterData[0].size = 128;
	characterData[0].position = &tile_mem_obj[0];
	
	//queue the OAM data for all of the sprites
	OAMData[0].position = (void *)oam_mem;
	OAMData[0].buffer = spriteBuffer;
	OAMData[0].size = sizeof(spriteBuffer) >> 2;
	
	//queue the background scroll registers
	IOData[0].position = (void *)&REG_BG0HOFS;
	IOData[0].buffer = IOBuffer;
	IOData[0].size = 4;
	IOBuffer[0] = mapData.camera.xPos % 512;
	IOBuffer[1] = mapData.camera.yPos % 512;
	IOBuffer[2] = mapData.camera.xPos % 512;
	IOBuffer[3] = mapData.camera.yPos % 512;
	IOBuffer[4] = mapData.camera.xPos % 16;
	IOBuffer[5] = mapData.camera.yPos % 16;
}

void processCameraPan(){
	s64 xAcc = (inverseTimeSquared[(mapData.camera.actionTarget >> 1) - 1]) * (s64)(mapData.camera.xTargetPos - mapData.camera.xStartingPos);
	s64 yAcc = (inverseTimeSquared[(mapData.camera.actionTarget >> 1) - 1]) * (s64)(mapData.camera.yTargetPos - mapData.camera.yStartingPos);
	
	//if we are in the first half of the pan, camera accellerates
	if(mapData.camera.actionTimer * 2 < mapData.camera.actionTarget){
		s64 time = mapData.camera.actionTimer;
		
		mapData.camera.xPos = ((xAcc * time * time) >> 32) + mapData.camera.xStartingPos;
		mapData.camera.yPos = ((yAcc * time * time) >> 32) + mapData.camera.yStartingPos;
	}
	//if we are in the second half of the pan, camera decellerates
	else if(mapData.camera.actionTimer < mapData.camera.actionTarget){
		s64 time = mapData.camera.actionTarget - mapData.camera.actionTimer;
		
		mapData.camera.xPos = -((xAcc * time * time) >> 32) + mapData.camera.xTargetPos;
		mapData.camera.yPos = -((yAcc * time * time) >> 32) + mapData.camera.yTargetPos;
	}
	//if we have finished this pan
	else if(mapData.camera.actionTimer == mapData.camera.actionTarget){
		mapData.camera.state = CAM_STILL;
		mapData.camera.xPos = mapData.camera.xTargetPos;
		mapData.camera.yPos = mapData.camera.yTargetPos;
	}
	mapData.camera.actionTimer++;
}
void cameraPanInit(s16 xTarget, s16 yTarget, u8 panTime){
		
	//pan time must be an even number
	panTime >>= 1;
	
	//a pan time of zero means to teleport straight to the target.
	if(panTime == 0){
		mapData.camera.xPos = xTarget;
		mapData.camera.yPos = yTarget;
	}
	else{
		mapData.camera.xTargetPos = xTarget;
		mapData.camera.yTargetPos = yTarget;
		mapData.camera.xStartingPos = mapData.camera.xPos;
		mapData.camera.yStartingPos = mapData.camera.yPos;
		mapData.camera.state = CAM_PANNING;
		mapData.camera.actionTarget = panTime << 1;
		mapData.camera.actionTimer = 0;
	}
}

void cameraBoundsCheck(s16 *xPosPointer, s16 *yPosPointer){
	s16 xPos = *xPosPointer;
	s16 yPos = *yPosPointer;
	
	//bounds check the final result
	if(xPos < 0){
		xPos = 0;
	}
	else if(xPos > (mapData.xSize << 4) - 223){
		xPos = (mapData.xSize << 4) - 223;
	}
	if(yPos < 0){
		yPos = 0;
	}
	else if(yPos > (mapData.ySize << 4) - 143){
		yPos = (mapData.ySize << 4) - 143;
	}
	
	*xPosPointer = xPos;
	*yPosPointer = yPos;
}

void shipMoveInit(s16 xTarget, s16 yTarget, u8 moveTime){
	mapData.actionTimer = 0;
	u8 shipIndex = mapData.selectedShip.index;
	
	//if the ship is moving, break it's same tile link
	if((mapData.ships[shipIndex].xVel != 0) || (mapData.ships[shipIndex].yVel != 0)){
		u8 checkedIndex = shipIndex;
		while(mapData.ships[checkedIndex].sameTileLink != shipIndex){
			checkedIndex = mapData.ships[checkedIndex].sameTileLink;
			if(isShipVisible(checkedIndex)){
				makeShipHidden(checkedIndex);
			}
		}
		makeShipVisible(checkedIndex);
		mapData.ships[checkedIndex].sameTileLink = mapData.ships[shipIndex].sameTileLink;
		mapData.ships[shipIndex].sameTileLink = shipIndex;
	}

	ShipData *ship = &mapData.ships[shipIndex];
	
	mapData.selectedShip.xPos = ship->xPos << 4;
	mapData.selectedShip.yPos = ship->yPos << 4;
	mapData.selectedShip.xInitial = ship->xPos << 4;
	mapData.selectedShip.yInitial = ship->yPos << 4;
	mapData.selectedShip.xTarget = xTarget;
	mapData.selectedShip.yTarget = yTarget;
	mapData.selectedShip.animationTimer = 0;
	mapData.selectedShip.actionTimer = 1;
	mapData.selectedShip.actionTarget = moveTime;
	mapData.selectedShip.angle = arctan2((xTarget>> 4) - ship->xPos, (yTarget >> 4) - ship->yPos);
}

void processShipMovement(){
	//if this move is done
	if(mapData.selectedShip.actionTimer == mapData.selectedShip.actionTarget){
		mapData.actionTimer = 1;
	}
	else{
		u32 multiplier = inverseTime[mapData.selectedShip.actionTarget - 1] * mapData.selectedShip.actionTimer;
		s16 xPos = mapData.selectedShip.xInitial + (((mapData.selectedShip.xTarget - mapData.selectedShip.xInitial) * multiplier) >> 15);
		s16 yPos = mapData.selectedShip.yInitial + (((mapData.selectedShip.yTarget - mapData.selectedShip.yInitial) * multiplier) >> 15);
		mapData.selectedShip.xPos = xPos;
		mapData.selectedShip.yPos = yPos;
		mapData.selectedShip.actionTimer++;
	}
}

void checkForOverlap(u8 shipIndex){
	u8 xPos = mapData.ships[shipIndex].xPos;
	u8 yPos = mapData.ships[shipIndex].yPos;
	
	//check all 256 ships for the first example of a ship in this position
	for(s32 i = 0; i < MAX_SHIPS; i++){
		if((mapData.ships[i].state == DOCKED) || (mapData.ships[i].state == DESTROYED) || (mapData.ships[i].state == NOT_PARTICIPATING) || (i == shipIndex)){
			continue;
		}
		//if the position is identical
		if((mapData.ships[i].xPos == xPos) && (mapData.ships[i].yPos == yPos)){
			
			//insert this ship into the same-tile linked list
			mapData.ships[shipIndex].sameTileLink = mapData.ships[i].sameTileLink;
			mapData.ships[i].sameTileLink = shipIndex;
			
			//set whichever ship on this tile is currently visible to hidden
			u8 checkedIndex = mapData.ships[shipIndex].sameTileLink;
			while(!isShipVisible(checkedIndex)){
				checkedIndex = mapData.ships[checkedIndex].sameTileLink;
			}
			makeShipHidden(checkedIndex);
			return;
		}
	}
}

void makeShipVisible(u8 shipIndex){
	if(mapData.ships[shipIndex].state == READY_HIDDEN){
		mapData.ships[shipIndex].state = READY_VISIBLE;
	}
	else if(mapData.ships[shipIndex].state == FINISHED_HIDDEN){
		mapData.ships[shipIndex].state = FINISHED_VISIBLE;
	}
	else if(mapData.ships[shipIndex].state == WRONG_TEAM_HIDDEN){
		mapData.ships[shipIndex].state = WRONG_TEAM_VISIBLE;
	}
}

void makeShipHidden(u8 shipIndex){
	if(mapData.ships[shipIndex].state == READY_VISIBLE){
		mapData.ships[shipIndex].state = READY_HIDDEN;
	}
	else if(mapData.ships[shipIndex].state == FINISHED_VISIBLE){
		mapData.ships[shipIndex].state = FINISHED_HIDDEN;
	}
	else if(mapData.ships[shipIndex].state == WRONG_TEAM_VISIBLE){
		mapData.ships[shipIndex].state = WRONG_TEAM_HIDDEN;
	}
}

u8 isShipVisible(u8 shipindex){
	if((mapData.ships[shipindex].state == READY_VISIBLE) || (mapData.ships[shipindex].state == FINISHED_VISIBLE) || 
	(mapData.ships[shipindex].state == WRONG_TEAM_VISIBLE)){
		return 1;
	}
	else{
		return 0;
	}
}

void moveCursor(){
	//if the cursor is not on, then don't move it
	if(mapData.cursor.state == CUR_HIDDEN){
		return;
	}
	
	u8 cursorMoving = 0;
	u8 isBHeld = 0;
	u8 squaresMoved = 0;
	
	//if the cursor is in the middle of a multi-frame movement, ignore inputs
	if((mapData.cursor.state != CUR_MOVE_ONCE_1) && (mapData.cursor.state != CUR_MOVE_ONCE_2)
	&& (mapData.cursor.state != CUR_MOVE_SLOW_1)){
		//if the cursor is moving up right
		if(((inputs.current & KEY_UP) && !(inputs.current & KEY_DOWN)) && ((inputs.current & KEY_RIGHT) && !(inputs.current & KEY_LEFT))){
			//if the cursor is already moving in this direction
			if((mapData.cursor.direction == CUR_UP) || (mapData.cursor.direction == CUR_RIGHT) || (mapData.cursor.direction == CUR_UP_RIGHT)){
				mapData.cursor.counter++;
				mapData.cursor.direction = CUR_UP_RIGHT;
			}
			//if this is a distinct new input
			else{
				mapData.cursor.direction = CUR_UP_RIGHT;
				mapData.cursor.counter = 0;
			}
			cursorMoving = 1;
		}
		//if the cursor is moving up left
		else if(((inputs.current & KEY_UP) && !(inputs.current & KEY_DOWN)) && ((inputs.current & KEY_LEFT) && !(inputs.current & KEY_RIGHT))){
			//if the cursor is already moving in this direction
			if((mapData.cursor.direction == CUR_UP) || (mapData.cursor.direction == CUR_LEFT) || (mapData.cursor.direction == CUR_UP_LEFT)){
				mapData.cursor.counter++;
				mapData.cursor.direction = CUR_UP_LEFT;
			}
			//if this is a distinct new input
			else{
				mapData.cursor.direction = CUR_UP_LEFT;
				mapData.cursor.counter = 0;
			}
			cursorMoving = 1;		
		}	
		//if the cursor is moving down right
		else if(((inputs.current & KEY_DOWN) && !(inputs.current & KEY_UP)) && ((inputs.current & KEY_RIGHT) && !(inputs.current & KEY_LEFT))){
			//if the cursor is already moving in this direction
			if((mapData.cursor.direction == CUR_DOWN) || (mapData.cursor.direction == CUR_RIGHT) || (mapData.cursor.direction == CUR_DOWN_RIGHT)){
				mapData.cursor.counter++;
				mapData.cursor.direction = CUR_DOWN_RIGHT;
			}
			//if this is a distinct new input
			else{
				mapData.cursor.direction = CUR_DOWN_RIGHT;
				mapData.cursor.counter = 0;
			}
			cursorMoving = 1;		
		}
		//if the cursor is moving down left
		else if(((inputs.current & KEY_DOWN) && !(inputs.current & KEY_UP)) && ((inputs.current & KEY_LEFT) && !(inputs.current & KEY_RIGHT))){
			//if the cursor is already moving in this direction
			if((mapData.cursor.direction == CUR_DOWN) || (mapData.cursor.direction == CUR_LEFT) || (mapData.cursor.direction == CUR_DOWN_LEFT)){
				mapData.cursor.counter++;
				mapData.cursor.direction = CUR_DOWN_LEFT;
			}	
			//if this is a distinct new input
			else{
				mapData.cursor.direction = CUR_DOWN_LEFT;
				mapData.cursor.counter = 0;
			}
			cursorMoving = 1;		
		}
		//if the cursor is moving up
		else if((inputs.current & KEY_UP) && !(inputs.current & KEY_DOWN)){
			//if the cursor is already moving in this direction
			if((mapData.cursor.direction == CUR_UP) || (mapData.cursor.direction == CUR_UP_LEFT) || (mapData.cursor.direction == CUR_UP_RIGHT)){
				mapData.cursor.counter++;
				mapData.cursor.direction = CUR_UP;
			}
			//if this is a distinct new input
			else{
				mapData.cursor.direction = CUR_UP;
				mapData.cursor.counter = 0;
			}	
			cursorMoving = 1;	
		}	
		//if the cursor is moving down
		else if((inputs.current & KEY_DOWN) && !(inputs.current & KEY_UP)){
			//if the cursor is already moving in this direction
			if((mapData.cursor.direction == CUR_DOWN) || (mapData.cursor.direction == CUR_DOWN_LEFT) || (mapData.cursor.direction == CUR_DOWN_RIGHT)){
				mapData.cursor.counter++;
				mapData.cursor.direction = CUR_DOWN;
			}
			//if this is a distinct new input
			else{
				mapData.cursor.direction = CUR_DOWN;
				mapData.cursor.counter = 0;
			}
			cursorMoving = 1;	
		}
		//if the cursor is moving left
		else if((inputs.current & KEY_LEFT) && !(inputs.current & KEY_RIGHT)){
			//if the cursor is already moving in this direction
			if((mapData.cursor.direction == CUR_LEFT) || (mapData.cursor.direction == CUR_DOWN_LEFT) || (mapData.cursor.direction == CUR_UP_LEFT)){
				mapData.cursor.counter++;
				mapData.cursor.direction = CUR_LEFT;
			}
			//if this is a distinct new input
			else{
				mapData.cursor.direction = CUR_LEFT;
				mapData.cursor.counter = 0;
			}
			cursorMoving = 1;	
		}
		//if the cursor is moving right
		else if((inputs.current & KEY_RIGHT) && !(inputs.current & KEY_LEFT)){
			//if the cursor is already moving in this direction
			if((mapData.cursor.direction == CUR_RIGHT) || (mapData.cursor.direction == CUR_DOWN_RIGHT) || (mapData.cursor.direction == CUR_UP_RIGHT)){
				mapData.cursor.counter++;
				mapData.cursor.direction = CUR_RIGHT;
			}
			//if this is a distinct new input
			else{
				mapData.cursor.direction = CUR_RIGHT;
				mapData.cursor.counter = 0;
			}
			cursorMoving = 1;	
		}
		//check if B is held
		if(inputs.current & KEY_B){
			isBHeld = 1;
		}
	}
	//if the cursor is mid-movement
	else{
		cursorMoving = 1;
		mapData.cursor.counter++;
	}
	
	//if the cursor is not moving
	if(cursorMoving == 0){
		mapData.cursor.state = CUR_STILL;
		mapData.cursor.direction = CUR_NO_DIRECTION;
	}
	//if B is held
	else if(isBHeld){
		mapData.cursor.state = CUR_MOVE_FAST;
		squaresMoved = 1;
	}
	//if a new direction was just input
	else if(mapData.cursor.counter == 0){
		mapData.cursor.state = CUR_MOVE_ONCE_1;
		squaresMoved = 1;
	}
	//if direction is held and first movement completes
	else if(mapData.cursor.state == CUR_MOVE_ONCE_3){
		mapData.cursor.state = CUR_MOVE_ONCE_WAIT;
	}
	//advance to the next frame of movement
	else if(mapData.cursor.state == CUR_MOVE_ONCE_1){
		mapData.cursor.state = CUR_MOVE_ONCE_2;
	}
	//advance to the next frame of movement
	else if(mapData.cursor.state == CUR_MOVE_SLOW_1){
		mapData.cursor.state = CUR_MOVE_SLOW_2;
	}
	//advance to the next frame of movement
	else if(mapData.cursor.state == CUR_MOVE_ONCE_2){
		mapData.cursor.state = CUR_MOVE_ONCE_3;
	}
	//advance to the next frame of movement
	else if(mapData.cursor.state == CUR_MOVE_SLOW_2){
		mapData.cursor.state = CUR_MOVE_SLOW_1;
		squaresMoved = 1;
	}
	//if direction is held and first movement finishes waiting
	else if(mapData.cursor.counter >= CURSOR_WAIT_FRAMES){
		mapData.cursor.state = CUR_MOVE_SLOW_1;
		squaresMoved = 1;
	}
	
	u32 vel;
	//decide the speed based on state
	if(mapData.cursor.state == CUR_MOVE_FAST){
		vel = CURSOR_FAST_SPEED;
	}
	else if((mapData.cursor.state == CUR_MOVE_ONCE_1) || (mapData.cursor.state == CUR_MOVE_ONCE_3)){
		vel = CURSOR_SLOW_SPEED_1;
	}
	else if((mapData.cursor.state == CUR_MOVE_ONCE_2) || (mapData.cursor.state == CUR_MOVE_SLOW_2) || (mapData.cursor.state == CUR_MOVE_SLOW_1)){
		vel = CURSOR_SLOW_SPEED_2;
	}
	else{
		vel = 0;
	}
	
	s32 deltaX;
	s32 deltaY;
	s32 selectedDeltaX;
	s32 selectedDeltaY;
	//decide the x and y offsets based on direction
	switch(mapData.cursor.direction){
	case CUR_UP:
		deltaX = 0;
		deltaY = -vel;
		selectedDeltaY = -squaresMoved;
		selectedDeltaX = 0;
		break;
	case CUR_DOWN:
		deltaX = 0;
		deltaY = vel;
		selectedDeltaY = squaresMoved;
		selectedDeltaX = 0;
		break;
	case CUR_LEFT:
		deltaX = -vel;
		deltaY = 0;
		selectedDeltaY = 0;
		selectedDeltaX = -squaresMoved;
		break;
	case CUR_RIGHT:
		deltaX = vel;
		deltaY = 0;
		selectedDeltaY = 0;
		selectedDeltaX = squaresMoved;
		break;
	case CUR_UP_RIGHT:
		deltaX = vel;
		deltaY = -vel;
		selectedDeltaY = -squaresMoved;
		selectedDeltaX = squaresMoved;
		break;
	case CUR_UP_LEFT:
		deltaX = -vel;
		deltaY = -vel;
		selectedDeltaY = -squaresMoved;
		selectedDeltaX = -squaresMoved;
		break;
	case CUR_DOWN_RIGHT:
		deltaX = vel;
		deltaY = vel;
		selectedDeltaY = squaresMoved;
		selectedDeltaX = squaresMoved;
		break;
	case CUR_DOWN_LEFT:
		deltaX = -vel;
		deltaY = vel;
		selectedDeltaY = squaresMoved;
		selectedDeltaX = -squaresMoved;
		break;
	default:
		deltaX = 0;
		deltaY = 0;
		selectedDeltaY = 0;
		selectedDeltaX = 0;
		break;
	}
	
	//update position
	mapData.cursor.xPos += deltaX;
	mapData.cursor.yPos += deltaY;
	if((mapData.cursor.selectXPos + selectedDeltaX) < 0){
		mapData.cursor.selectXPos = 0;
	}
	else if((mapData.cursor.selectXPos + selectedDeltaX) >= 256){
		mapData.cursor.selectXPos = 255;
	}
	else{
		mapData.cursor.selectXPos += selectedDeltaX;
	}
	if((mapData.cursor.selectYPos + selectedDeltaY) < 0){
		mapData.cursor.selectYPos = 0;
	}
	else if((mapData.cursor.selectYPos + selectedDeltaY) >= 256){
		mapData.cursor.selectYPos = 255;
	}
	else{
		mapData.cursor.selectYPos += selectedDeltaY;
	}

	
	//bounds check
	if(mapData.cursor.xPos < -8){
		mapData.cursor.xPos = -8;
	}
	else if(mapData.cursor.xPos > (mapData.xSize * 16 - 8)){
		mapData.cursor.xPos = mapData.xSize * 16 - 8;
	}
	if(mapData.cursor.yPos < -8){
		mapData.cursor.yPos = -8;
	}
	else if(mapData.cursor.yPos > (mapData.ySize * 16 - 8)){
		mapData.cursor.yPos = mapData.ySize * 16 - 8;
	}
	
	//update the selected Position;
	
}

void selectShip(u8 shipIndex){
		mapData.selectedShip.index = shipIndex;
		mapData.ships[shipIndex].state = SELECTED;
		mapData.state = SHIP_SELECTED;
		
		//make sure all ships sharing this tile are hidden
		u8 currentIndex = mapData.ships[shipIndex].sameTileLink;
		while(mapData.ships[currentIndex].sameTileLink != mapData.ships[shipIndex].sameTileLink){
			makeShipHidden(currentIndex);
			currentIndex = mapData.ships[currentIndex].sameTileLink;
		}
		
		mapData.selectedShip.xPos = (mapData.ships[shipIndex].xPos << 4);
		mapData.selectedShip.yPos = (mapData.ships[shipIndex].yPos << 4);
		mapData.selectedShip.angle = arctan2(mapData.cursor.selectXPos - mapData.ships[shipIndex].xPos, mapData.cursor.selectYPos - mapData.ships[shipIndex].yPos);
		
		s16 xTarget = (mapData.ships[shipIndex].xPos << 4) - 112;
		s16 yTarget = (mapData.ships[shipIndex].yPos << 4) - 72;
		
		mapData.cursor.state = CUR_STILL;
		mapData.cursor.selectXPos = mapData.ships[shipIndex].xPos;
		mapData.cursor.selectYPos = mapData.ships[shipIndex].yPos;
		
		//pan the camera to this ship
		mapData.camera.state = CAM_STILL;
		cameraPanInit(xTarget, yTarget, CYCLE_PAN_SPEED);
		
		//disable the minimap
		hideMinimap();
}

u8 arctan2(s16 deltaX, s16 deltaY){
	u8 xNegative = 0;
	u8 yNegative = 0;
	u8 angle;
	//check the signs of the inputs
	if(deltaX < 0){
		xNegative = 1;
		deltaX = -deltaX;
	}
	if(deltaY < 0){
		yNegative = 1;
		deltaY = -deltaY;
	}
	//if we are pointing up
	if((deltaX == 0) && yNegative){
		return 192;
	}
	//if we are not moving
	else if((deltaX == 0) && (deltaY == 0)){
		return 0;
	}
	//if we are pointing down
	else if(deltaX == 0){
		return 64;
	}
	else if(deltaX >= deltaY){
		angle = (arctanTable1[(inverseTime[deltaX - 1] * deltaY) >> 8] >> 2);
	}
	else{
		angle = (arctanTable2[(inverseTime[deltaX - 1] * deltaY) >> 11] >> 2);
	}
	
	if(xNegative){
		angle = 128 - angle;
	}
	
	if(yNegative){
		angle = 256 - angle;
	}
	
	return angle;
	
}

void checkCycleButtons(){
	if(inputs.pressed & KEY_L){
		//cycle through the linked list until we end up one before where we started
		u32 currentIndex = mapData.selectedShip.index;
		u32 lastActiveIndex = 0;
		while(mapData.ships[currentIndex].teamLink != mapData.selectedShip.index){
			if((mapData.ships[currentIndex].state == READY_VISIBLE) || (mapData.ships[currentIndex].state = READY_HIDDEN)){
				lastActiveIndex = currentIndex;
			}
			currentIndex = mapData.ships[currentIndex].teamLink;
		}
		if((mapData.ships[currentIndex].state == READY_VISIBLE) || (mapData.ships[currentIndex].state = READY_HIDDEN)){
			lastActiveIndex = currentIndex;
		}
		selectShip(lastActiveIndex);
	}
	else if(inputs.pressed & KEY_R){
		//cycle to the next ship in the linked list of active ships
		u32 currentIndex = mapData.ships[mapData.selectedShip.index].teamLink;
		while(mapData.ships[currentIndex].teamLink != mapData.selectedShip.index){
			if((mapData.ships[currentIndex].state == READY_VISIBLE) || (mapData.ships[currentIndex].state = READY_HIDDEN)){
				break;
			}
			currentIndex = mapData.ships[currentIndex].teamLink;
			
		}
		selectShip(currentIndex);
	}
}

void revealMinimap(){
	//if the minimap is already out, or coming out, do nothing
	if((mapData.minimap.state != MINIMAP_HIDDEN_LEFT) && (mapData.minimap.state != MINIMAP_HIDDEN_RIGHT)){
		return;
	}
	//bring the minimap back where it last was
	if(mapData.minimap.state == MINIMAP_HIDDEN_LEFT){
		mapData.minimap.state = MINIMAP_EMERGING_LEFT;
	}
	else if(mapData.minimap.state == MINIMAP_HIDDEN_RIGHT){
		mapData.minimap.state = MINIMAP_EMERGING_RIGHT;
	}
	mapData.minimap.actionTarget = 4;
	mapData.minimap.actionTimer = 0;
}

void hideMinimap(){
	//if the minimap is already hidden or hiding, do nothing
	if((mapData.minimap.state == MINIMAP_HIDING_RIGHT) || (mapData.minimap.state == MINIMAP_HIDING_LEFT) || 
	(mapData.minimap.state == MINIMAP_HIDDEN_RIGHT) || (mapData.minimap.state == MINIMAP_HIDDEN_LEFT)){
		return;
	}
	if((mapData.minimap.state == MINIMAP_STILL_LEFT) || (mapData.minimap.state == MINIMAP_MOVING_RIGHT) || (mapData.minimap.state == MINIMAP_EMERGING_RIGHT)){
		mapData.minimap.state = MINIMAP_HIDING_LEFT;
	}
	else if((mapData.minimap.state == MINIMAP_STILL_RIGHT) || (mapData.minimap.state == MINIMAP_MOVING_LEFT) || (mapData.minimap.state == MINIMAP_EMERGING_LEFT)){
		mapData.minimap.state = MINIMAP_HIDING_RIGHT;
	}
	mapData.minimap.actionTarget = 4;
	mapData.minimap.actionTimer = 0;
}

void cursorBoundsCheck(){
	if((mapData.cursor.state == CUR_HIDDEN) || (mapData.camera.state != CAM_STILL)){
		return;
	}
	
	//bounds when there is no minimap
	if(mapData.minimap.state != MINIMAP_STILL_LEFT && mapData.minimap.state != MINIMAP_STILL_RIGHT){
		if((mapData.cursor.xPos - mapData.camera.xPos) < 40){
			mapData.camera.xPos = mapData.cursor.xPos - 40;
		}
		else if((mapData.cursor.xPos - mapData.camera.xPos) > 168){
			mapData.camera.xPos = mapData.cursor.xPos - 168;
		}
	}
	
	//x bounds when the minimap is on the left
	else if(mapData.minimap.state == MINIMAP_STILL_LEFT){
		if((mapData.cursor.xPos - mapData.camera.xPos) < 88){
			mapData.camera.xPos = mapData.cursor.xPos - 88;
		}
		else if((mapData.cursor.xPos - mapData.camera.xPos) > 168){
			mapData.camera.xPos = mapData.cursor.xPos - 168;
		}
	}
	
	//x bounds when the minimap is on the right
	else if(mapData.minimap.state == MINIMAP_STILL_RIGHT){
		if((mapData.cursor.xPos - mapData.camera.xPos) < 40){
			mapData.camera.xPos = mapData.cursor.xPos - 40;
		}
		else if((mapData.cursor.xPos - mapData.camera.xPos) > 120){
			mapData.camera.xPos = mapData.cursor.xPos - 120;
		}
	}
	
	//y bounds
	if((mapData.cursor.yPos - mapData.camera.yPos) < 24){
		mapData.camera.yPos = mapData.cursor.yPos - 24;
	}
	else if((mapData.cursor.yPos - mapData.camera.yPos) > 104){
		mapData.camera.yPos = mapData.cursor.yPos - 104;
	}
}
//a temprary function to initialize a test map.
void initMap(){
	u8 index = 0;
	u32 yPos = 8;
	u32 xPos = 10;
	
	for(u32 team = 0; team < NUM_TEAMS; team++){
		for(u32 type = 0; type <= CARRIER; type++){
			xPos = (xPos * 1103515245) + 12345;
			yPos = (yPos * 1103515245) + 12345;
			
			mapData.ships[index].type = type;
			mapData.ships[index].state = READY_VISIBLE;
			mapData.ships[index].team = team;
			mapData.ships[index].health = 100;
			mapData.ships[index].xPos = (xPos >> 16) % 256;
			mapData.ships[index].yPos = (yPos >> 16) % 256;
			mapData.ships[index].xVel = team + 1;
			mapData.ships[index].yVel = 0;
			
			index++;
			xPos = (xPos * 1103515245) + 12345;
			yPos = (yPos * 1103515245) + 12345;
			
			mapData.ships[index].type = type;
			mapData.ships[index].state = READY_VISIBLE;
			mapData.ships[index].team = team;
			mapData.ships[index].health = 100;
			mapData.ships[index].xPos = (xPos >> 16) % 256;
			mapData.ships[index].yPos = (yPos >> 16) % 256;
			mapData.ships[index].xVel = 0;
			mapData.ships[index].yVel = -team - 1;
			
			index++;
			xPos = (xPos * 1103515245) + 12345;
			yPos = (yPos * 1103515245) + 12345;
			
			mapData.ships[index].type = type;
			mapData.ships[index].state = READY_VISIBLE;
			mapData.ships[index].team = team;
			mapData.ships[index].health = 100;
			mapData.ships[index].xPos = (xPos >> 16) % 256;
			mapData.ships[index].yPos = (yPos >> 16) % 256;
			mapData.ships[index].xVel = -team - 1;
			mapData.ships[index].yVel = 0;
			
			index++;
			xPos = (xPos * 1103515245) + 12345;
			yPos = (yPos * 1103515245) + 12345;
			
			mapData.ships[index].type = type;
			mapData.ships[index].state = READY_VISIBLE;
			mapData.ships[index].team = team;
			mapData.ships[index].health = 100;
			mapData.ships[index].xPos = (xPos >> 16) % 256;
			mapData.ships[index].yPos = (yPos >> 16) % 256;
			mapData.ships[index].xVel = 0;
			mapData.ships[index].yVel = team + 1;
			
			index++;
			xPos = (xPos * 1103515245) + 12345;
			yPos = (yPos * 1103515245) + 12345;
		}
	}
	
	for(u32 i = index; i < MAX_SHIPS; i++){
		mapData.ships[i].state = NOT_PARTICIPATING;
	}
}