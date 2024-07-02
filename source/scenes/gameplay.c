#include "gameplay.h"

u16 tilemapBuffer0[1024] EWRAM_DATA;
u16 tilemapBuffer1[1024] EWRAM_DATA;
u16 tilemapBuffer2[1024] EWRAM_DATA;
u16 tilemapBuffer3[1024] EWRAM_DATA;
u16 characterBuffer0[1024] EWRAM_DATA;
u16 characterBuffer1[1024] EWRAM_DATA;
OBJ_ATTR spriteBuffer[128] EWRAM_DATA;
u16 IOBuffer[30];

MapData mapData EWRAM_DATA;


Scene gameplayScene = {
	.initialize = &gameplayInitialize,
	.intro = 0,
	.normal = &gameplayNormal,
	.pause = 0,
	.outro = 0,
	.end = &gameplayEnd,
};

void gameplayInitialize(){
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
	REG_BG0CNT = BG_4BPP | BG_SBB(BG_0_TILEMAP) | BG_CBB(BG_0_CHARDATA);
	REG_BG1CNT = BG_4BPP | BG_SBB(BG_1_TILEMAP) | BG_CBB(BG_0_CHARDATA);
	
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
	
	playNewAsset(_AreaA_DMA_Only);
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
		break;
	case SHIP_MOVING:
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
	for(int i = mapXPos; i < mapXPos + 16; i++){
		for(int j = mapYPos; j < mapYPos + 16; j++){
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64] = bgGfxMap[3];
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64 + 1] = bgGfxMap[3];
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64 + 32] = bgGfxMap[3];
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64 + 33] = bgGfxMap[3];
		}
	}
	
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
		if((mapData.ships[shipIndex].state != READY_VISIBLE) && (mapData.ships[shipIndex].state != FINISHED_VISIBLE) && 
		(mapData.ships[shipIndex].state != WRONG_TEAM_VISIBLE)){
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
	for(u32 i = mapXPos; i < mapXPos + 16; i++){
		for(u32 j = mapYPos; j < mapYPos + 16; j++){
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64] = bgGfxMap[GRID_GFX_START * 4 + (7 * 4)];
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64 + 1] = bgGfxMap[GRID_GFX_START * 4 + (7 * 4) + 1];
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64 + 32] = bgGfxMap[GRID_GFX_START * 4 + (7 * 4) + 2];
			tilemapBuffer[(i % 16) * 2 + (j % 16) * 64 + 33] = bgGfxMap[GRID_GFX_START * 4 + (7 * 4) + 3];
		}
	}
	
	u32 cycle = currentScene.sceneCounter % 256;
	
	//draw the diagonal
	for(u32 i = (mapXPos % 128); i < ((mapXPos % 128) + 16); i++){
		u32 xPos = i;
		u32 yPos = ((cycle >> 1) - i) % 128;
		
		if(((yPos - mapYPos) % 128) < 16){
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64] = bgGfxMap[GRID_GFX_START * 4 + (cycle % 2) * 8];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 1] = bgGfxMap[GRID_GFX_START * 4 + (cycle % 2) * 8 + 1];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 32] = bgGfxMap[GRID_GFX_START * 4 + (cycle % 2) * 8 + 2];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 33] = bgGfxMap[GRID_GFX_START * 4 + (cycle % 2) * 8 + 3];
			
			xPos--;
			
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64] = bgGfxMap[GRID_GFX_START * 4 + ((cycle % 2) * 2 + 4) * 4];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 1] = bgGfxMap[GRID_GFX_START * 4 + ((cycle % 2) * 2 + 4) * 4 + 1];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 32] = bgGfxMap[GRID_GFX_START * 4 + ((cycle % 2) * 2 + 4) * 4 + 2];
			tilemapBuffer[(xPos % 16) * 2 + (yPos % 16) * 64 + 33] = bgGfxMap[GRID_GFX_START * 4 + ((cycle % 2) * 2 + 4) * 4 + 3];
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
	//if the selected ship sprite is being drawn
	u8 shipXPos = mapData.selectedShip.xPos - mapData.camera.xPos - 8;
	u8 shipYPos = mapData.selectedShip.yPos - mapData.camera.yPos - 8;
	
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

void drawCursor(OBJ_ATTR *){
	//if the cursor is hidden, don't draw anything.
	if(mapData.cursor.state == CUR_HIDDEN){
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
	if((inputs.pressed & KEY_L) && (mapData.camera.state == CAM_STILL)){
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
		mapData.selectedShip.index = lastActiveIndex;
		
		s16 xTarget = (mapData.ships[mapData.selectedShip.index].xPos << 4) - 112;
		s16 yTarget = (mapData.ships[mapData.selectedShip.index].yPos << 4) - 72;
		
		mapData.cursor.state = CUR_STILL;
		mapData.cursor.xPos = xTarget + 104;
		mapData.cursor.yPos = yTarget + 64;
		
		//pan the camera to this ship
		cameraPanInit(xTarget, yTarget, CYCLE_PAN_SPEED);
	}
	if((inputs.pressed & KEY_R) && (mapData.camera.state == CAM_STILL)){
		//cycle to the next ship in the linked list of active ships
		u32 currentIndex = mapData.ships[mapData.selectedShip.index].teamLink;
		while(mapData.ships[currentIndex].teamLink != mapData.selectedShip.index){
			if((mapData.ships[currentIndex].state == READY_VISIBLE) || (mapData.ships[currentIndex].state = READY_HIDDEN)){
				break;
			}
			currentIndex = mapData.ships[currentIndex].teamLink;
		}
		mapData.selectedShip.index = currentIndex;
		
		s16 xTarget = (mapData.ships[mapData.selectedShip.index].xPos << 4) - 112;
		s16 yTarget = (mapData.ships[mapData.selectedShip.index].yPos << 4) - 72;
		
		mapData.cursor.state = CUR_STILL;
		mapData.cursor.xPos = xTarget + 104;
		mapData.cursor.yPos = yTarget + 64;
		
		//pan the camera to this ship
		cameraPanInit(xTarget, yTarget, CYCLE_PAN_SPEED);
	}
	
	//if start is pressed, transition to the end turn sequence
	if(inputs.pressed & KEY_START){
		mapData.state = TURN_END;
		mapData.actionTimer = 0;
	}
	
	//handle any changes to the camera that occured this frame
	processCamera();
}

//after this player has selected "end turn"
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
		mapData.actionTimer = 0;
		mapData.ships[mapData.selectedShip.index].state = SELECTED;
		mapData.state = TURN_END_MOVEMENT;
		//start a movement of the selected ship
		s16 xTarget = (mapData.ships[mapData.selectedShip.index].xPos + mapData.ships[mapData.selectedShip.index].xVel) << 4;
		s16 yTarget = (mapData.ships[mapData.selectedShip.index].yPos + mapData.ships[mapData.selectedShip.index].yVel) << 4;
		shipMoveInit(xTarget, yTarget, SHIP_MOVE_SPEED);
		//if the ship is moving, break it's same tile link
		if((mapData.ships[mapData.selectedShip.index].xVel != 0) || (mapData.ships[mapData.selectedShip.index].yVel != 0)){
			u8 shipIndex = mapData.selectedShip.index;
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

//initialize all of the linked lists of the ships in this scenario
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
	
	//follow the cursor
	if(mapData.cursor.state != CUR_HIDDEN){
		if((mapData.cursor.xPos - mapData.camera.xPos) < 24){
			mapData.camera.xPos = mapData.cursor.xPos - 24;
		}
		else if((mapData.cursor.xPos - mapData.camera.xPos) > 184){
			mapData.camera.xPos = mapData.cursor.xPos - 184;
		}
		if((mapData.cursor.yPos - mapData.camera.yPos) < 24){
			mapData.camera.yPos = mapData.cursor.yPos - 24;
		}
		else if((mapData.cursor.yPos - mapData.camera.yPos) > 104){
			mapData.camera.yPos = mapData.cursor.yPos - 104;
		}
	}
	
	cameraBoundsCheck(&mapData.camera.xPos, &mapData.camera.yPos);
	
	//update the tilemap with the new position
	createShipTilemap(tilemapBuffer1);
	
	//update the grid
	createGridTilemap(tilemapBuffer2);
	
	//process the selected ship's sprite if applicable
	drawSelectedShip(spriteBuffer);
	
	//draw the cursor if applicable
	drawCursor(spriteBuffer);
	
	//queue the tilemap for layer 1 to be sent
	tilemapData[1].size = 512;
	tilemapData[1].buffer = tilemapBuffer1;
	tilemapData[1].position = se_mem[BG_1_TILEMAP];
	
	//queue the tilemap for layer 0 to be sent
	tilemapData[0].size = 512;
	tilemapData[0].buffer = tilemapBuffer2;
	tilemapData[0].position = se_mem[BG_0_TILEMAP];
	
	//queue the character data for the sprites
	characterData[0].buffer = characterBuffer0;
	characterData[0].size = 128;
	characterData[0].position = tile_mem_obj[0];
	
	//queue the OAM data for all of the sprites
	OAMData[0].position = (void *)oam_mem;
	OAMData[0].buffer = spriteBuffer;
	OAMData[0].size = sizeof(spriteBuffer) >> 2;
	
	//queue the background scroll registers
	IOData[0].position = (void *)&REG_BG0HOFS;
	IOData[0].buffer = IOBuffer;
	IOData[0].size = 2;
	IOBuffer[0] = mapData.camera.xPos % 512;
	IOBuffer[1] = mapData.camera.yPos % 512;
	IOBuffer[2] = mapData.camera.xPos % 512;
	IOBuffer[3] = mapData.camera.yPos % 512;
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

//initialize a pan that sends the camera to a specific location
void cameraPanInit(s16 xTarget, s16 yTarget, u8 panTime){
	
	//only initialize a pan if the camera is not performing another action
	if(mapData.camera.state != CAM_STILL){
		return;
	}
	
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
	u8 shipIndex = mapData.selectedShip.index;
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
	
	if(ABS(ship->xVel) > ABS(ship->yVel)){
		if(ship->xVel > 0){
			mapData.selectedShip.angle = 0;
		}
		else{
			mapData.selectedShip.angle = 0x80;
		}
	}
	else{
		if(ship->yVel > 0){
			mapData.selectedShip.angle = 0x40;
		}
		else{
			mapData.selectedShip.angle = 0xC0;
		}
	}
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
	}
	//if a new direction was just input
	else if(mapData.cursor.counter == 0){
		mapData.cursor.state = CUR_MOVE_ONCE_1;
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
	}
	//if direction is held and first movement finishes waiting
	else if(mapData.cursor.counter >= CURSOR_WAIT_FRAMES){
		mapData.cursor.state = CUR_MOVE_SLOW_1;
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
	//decide the x and y offsets based on direction
	switch(mapData.cursor.direction){
	case CUR_UP:
		deltaX = 0;
		deltaY = -vel;
		break;
	case CUR_DOWN:
		deltaX = 0;
		deltaY = vel;
		break;
	case CUR_LEFT:
		deltaX = -vel;
		deltaY = 0;
		break;
	case CUR_RIGHT:
		deltaX = vel;
		deltaY = 0;
		break;
	case CUR_UP_RIGHT:
		deltaX = vel;
		deltaY = -vel;
		break;
	case CUR_UP_LEFT:
		deltaX = -vel;
		deltaY = -vel;
		break;
	case CUR_DOWN_RIGHT:
		deltaX = vel;
		deltaY = vel;
		break;
	case CUR_DOWN_LEFT:
		deltaX = -vel;
		deltaY = vel;
		break;
	default:
		deltaX = 0;
		deltaY = 0;
		break;
	}
	
	//update position
	mapData.cursor.xPos += deltaX;
	mapData.cursor.yPos += deltaY;
	
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
}

//a temprary function to initialize a test map.
void initMap(){
	u8 index = 0;
	for(u32 team = 0; team < NUM_TEAMS; team++){
		u8 yPos = 8;
		for(u32 type = 0; type <= CARRIER; type++){
			u8 xPos = 8;
			
			mapData.ships[index].type = type;
			mapData.ships[index].state = READY_VISIBLE;
			mapData.ships[index].team = team;
			mapData.ships[index].health = 100;
			mapData.ships[index].xPos = xPos;
			mapData.ships[index].yPos = yPos;
			mapData.ships[index].xVel = team + 1;
			mapData.ships[index].yVel = 0;
			
			index++;
			xPos++;
			
			mapData.ships[index].type = type;
			mapData.ships[index].state = READY_VISIBLE;
			mapData.ships[index].team = team;
			mapData.ships[index].health = 100;
			mapData.ships[index].xPos = xPos;
			mapData.ships[index].yPos = yPos;
			mapData.ships[index].xVel = 0;
			mapData.ships[index].yVel = -team - 1;
			
			index++;
			xPos++;
			
			mapData.ships[index].type = type;
			mapData.ships[index].state = READY_VISIBLE;
			mapData.ships[index].team = team;
			mapData.ships[index].health = 100;
			mapData.ships[index].xPos = xPos;
			mapData.ships[index].yPos = yPos;
			mapData.ships[index].xVel = -team - 1;
			mapData.ships[index].yVel = 0;
			
			index++;
			xPos++;
			
			mapData.ships[index].type = type;
			mapData.ships[index].state = READY_VISIBLE;
			mapData.ships[index].team = team;
			mapData.ships[index].health = 100;
			mapData.ships[index].xPos = xPos;
			mapData.ships[index].yPos = yPos;
			mapData.ships[index].xVel = 0;
			mapData.ships[index].yVel = team + 1;
			
			index++;
			yPos++;
		}
	}
	
	for(u32 i = index; i < MAX_SHIPS; i++){
		mapData.ships[i].state = NOT_PARTICIPATING;
	}
}