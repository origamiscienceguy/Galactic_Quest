#include "gameplay.h"

u16 tilemapBuffer0[1024] EWRAM_DATA;
u16 tilemapBuffer1[1024] EWRAM_DATA;
u16 tilemapBuffer2[1024] EWRAM_DATA;
u16 tilemapBuffer3[1024] EWRAM_DATA;
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
	
	mapData.camera.xPos = 0;
	mapData.camera.yPos = 0;
	mapData.camera.xLastPos = (s64)0xffff << POS_PRECISION;
	mapData.camera.yLastPos = (s64)0xffff << POS_PRECISION;
	mapData.camera.state = STILL;
	mapData.xSize = 255;
	mapData.ySize = 255;
	mapData.state = TURN_START;
	mapData.teamTurn = RED_TEAM;
	mapData.selectedShip = 0;
	
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
	case BATTLE:
		break;
	case TURN_END:
		turnEndState();
		break;
	case TURN_REPLAY:
		break;
	}
}

void gameplayEnd(){

}

void createShipTilemap(u16 *tilemapBuffer){
	u16 xUpdateLow = 0; //lower bound of horizontal map cell that need to be updated
	u16 xUpdateHigh = 0; //upper bound of horizontal map cell that need to be updated
	u16 yUpdateLow = 0; //lower bound of vertical map cell that need to be updated
	u16 yUpdateHigh = 0; //upper bound of vertical map cell that need to be updated
	
	//convert pixel coordinates to map cell coordinates
	u8 mapXPos = mapData.camera.xPos >> (4 + POS_PRECISION);
	u8 mapYPos = mapData.camera.yPos >> (4 + POS_PRECISION); 
	u8 mapXLastPos = mapData.camera.xLastPos >> (4 + POS_PRECISION);
	u8 mapYLastPos = mapData.camera.yLastPos >> (4 + POS_PRECISION);

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
			tilemapBuffer[(i % 16) * 2 + j * 64] = 3;
			tilemapBuffer[(i % 16) * 2 + j * 64 + 1] = 3;
			tilemapBuffer[(i % 16) * 2 + j * 64 + 32] = 3;
			tilemapBuffer[(i % 16) * 2 + j * 64 + 33] = 3;
		}
	}
	
	//clear the horizontal column of newly arrived spaces
	for(int i = yUpdateLow; i < yUpdateHigh; i++){
		for(int j = 0; j < 16; j++){
			tilemapBuffer[(i % 16) * 64 + j * 2] = 3;
			tilemapBuffer[(i % 16) * 64 + j * 2 + 1] = 3;
			tilemapBuffer[(i % 16) * 64 + j * 2 + 32] = 3;
			tilemapBuffer[(i % 16) * 64 + j * 2 + 33] = 3;
		}
	}
	
	//draw every ship that should be on screen
	for(u32 shipIndex = 0; shipIndex < mapData.numShips; shipIndex++){
		u8 shipXPos = mapData.ships[shipIndex].xPos;
		u8 shipYPos = mapData.ships[shipIndex].yPos;
		
		//if this particular ship is in one of the update regions
		if(((shipXPos >= xUpdateLow) && (shipXPos < xUpdateHigh) && (shipYPos >= mapYPos) && (shipYPos <= mapYPos + 15))
		|| ((shipYPos >= yUpdateLow) && (shipYPos < yUpdateHigh) && (shipXPos >= mapXPos) && (shipXPos <= mapXPos + 15))){
			u16 baseIndex = (shipXPos % 16) * 2 + (shipYPos % 16) * 64;
			u8 tilemapBase = (mapData.ships[shipIndex].type + 1) * 4;
			tilemapBuffer[baseIndex] = tilemapBase;
			tilemapBuffer[baseIndex + 1] = tilemapBase + 1;
			tilemapBuffer[baseIndex + 32] = tilemapBase + 2;
			tilemapBuffer[baseIndex + 33] = tilemapBase + 3;
			
		}
	}
	
	//update the map previous position
	mapData.camera.xLastPos = mapData.camera.xPos;
	mapData.camera.yLastPos = mapData.camera.yPos;
}

void turnStartState(){
	//create the active ship linked list based on which ships in the team linked list are still active
	TeamData *teamPointer = &mapData.teams[mapData.teamTurn];
	
	//if this team has no active ships, end the turn immediately
	if(teamPointer->state != ACTIVE){
		mapData.state = TURN_END;
		return;
	}
	
	u32 firstShipIndex = teamPointer->firstShip;
	u32 shipIndex = firstShipIndex;
	
	//find the first ship active on this team
	while((mapData.ships[shipIndex].state == DESTROYED) || (mapData.ships[shipIndex].state == NOT_PARTICIPATING)){
		shipIndex = mapData.ships[shipIndex].teamLink;
		//if we run into the first ship index, it means all ships are dead
		if(shipIndex == firstShipIndex){
			teamPointer->state = DEFEATED;
			mapData.state = TURN_END;
			return;
		}
	}
	
	u32 numActiveShips = 0;
	
	//shipIndex now contains the first active ship on this team.
	mapData.selectedShip = shipIndex;
	u32 lastShipIndex = shipIndex;
	u32 nextIndex = mapData.ships[shipIndex].teamLink;
	//add all other active ships to the linked list
	while(nextIndex != firstShipIndex){
		if((mapData.ships[nextIndex].state != DESTROYED) && (mapData.ships[nextIndex].state != NOT_PARTICIPATING)){
			mapData.ships[lastShipIndex].activeLink = nextIndex;
			lastShipIndex = nextIndex;
			numActiveShips++;
		}
		nextIndex = mapData.ships[nextIndex].teamLink;
	}
	
	//finish up the loop with the last ship
	mapData.ships[lastShipIndex].activeLink = shipIndex;
	
	teamPointer->numActiveShips = numActiveShips;
	
	mapData.state = OPEN_MAP;
}

void openMapState(){
	u16 scrollSpeed = 12;
	
	//the d pad scrolls the map
	if(inputs.current & KEY_RIGHT){
		s16 xTarget = (mapData.camera.xPos >> POS_PRECISION) + scrollSpeed;
		s16 yTarget = (mapData.camera.yPos >> POS_PRECISION);
		
		cameraPanInit(xTarget, yTarget, 0);
	}
	if(inputs.current & KEY_LEFT){
		s16 xTarget = (mapData.camera.xPos >> POS_PRECISION) - scrollSpeed;
		s16 yTarget = (mapData.camera.yPos >> POS_PRECISION);
		
		cameraPanInit(xTarget, yTarget, 0);
	}
	if(inputs.current & KEY_DOWN){
		s16 xTarget = (mapData.camera.xPos >> POS_PRECISION);
		s16 yTarget = (mapData.camera.yPos >> POS_PRECISION) + scrollSpeed;
		
		cameraPanInit(xTarget, yTarget, 0);
	}
	if(inputs.current & KEY_UP){
		s16 xTarget = (mapData.camera.xPos >> POS_PRECISION);
		s16 yTarget = (mapData.camera.yPos >> POS_PRECISION) - scrollSpeed;
		
		cameraPanInit(xTarget, yTarget, 0);
	}
	
	//L and R cycle backwards or forwards through the active ships for this team, and center the camera on the next ship in the cycle
	if(inputs.pressed & KEY_L){
		//cycle through the linked list until we end up one before where we started
		u32 currentIndex = mapData.selectedShip;
		while(mapData.ships[currentIndex].activeLink != mapData.selectedShip){
			currentIndex = mapData.ships[currentIndex].activeLink;
		}
		mapData.selectedShip = currentIndex;
		
		s64 xTarget = (s64)((mapData.ships[mapData.selectedShip].xPos << 4) - 112);
		s64 yTarget = (s64)((mapData.ships[mapData.selectedShip].yPos << 4) - 72);
		
		//pan the camera to this ship
		cameraPanInit(xTarget, yTarget, 50);
	}
	if(inputs.pressed & KEY_R){
		//cycle to the next ship in the linked list of active ships
		mapData.selectedShip = mapData.ships[mapData.selectedShip].activeLink;
		
		s64 xTarget = (s64)((mapData.ships[mapData.selectedShip].xPos << 4) - 112);
		s64 yTarget = (s64)((mapData.ships[mapData.selectedShip].yPos << 4) - 72);
		
		//pan the camera to this ship
		cameraPanInit(xTarget, yTarget, 50);
	}
	
	//handle any changes to the camera that occured this frame
	processCamera();
	
	//queue the tilemap for layer 1 to be sent
	tilemapData[1].size = 512;
	tilemapData[1].buffer = tilemapBuffer1;
	tilemapData[1].position = se_mem[BG_1_TILEMAP];
	
	//update the tilemap with the new position
	createShipTilemap(tilemapBuffer1);
	
	IOData[0].position = (void *)&REG_BG0HOFS;
	IOData[0].buffer = IOBuffer;
	IOData[0].size = 2;
	
	IOBuffer[0] = (mapData.camera.xPos >> POS_PRECISION) % 512;
	IOBuffer[1] = (mapData.camera.yPos >> POS_PRECISION) % 512;
	IOBuffer[2] = (mapData.camera.xPos >> POS_PRECISION) % 512;
	IOBuffer[3] = (mapData.camera.yPos >> POS_PRECISION) % 512;
}

//after this player has selected "end turn"
void turnEndState(){
	
}

//initialize all of the linked lists of the ships in this scenario
void shipListInit(){
	//initialize all four teams
	for(u32 teamIndex = 0; teamIndex < NUM_TEAMS; teamIndex++){
		TeamData *teamPointer = &mapData.teams[teamIndex];
		teamPointer->state = ABSENT;
		teamPointer->numStartingShips = 0;
	}
	
	u32 lastShipIndex[NUM_TEAMS]; //the last found ship for this particular team
	
	//check every ship
	for(u32 shipIndex = 0; shipIndex < MAX_SHIPS; shipIndex++){
		ShipData *shipPointer = &mapData.ships[shipIndex];
		u32 shipTeam = shipPointer->team;
		TeamData *teamPointer = &mapData.teams[shipTeam];
		
		//all active ships must be initialized with the ready state
		if(shipPointer->state != READY){
			continue;
		}
		
		//increment the number of ships for the team this ship belongs to
		teamPointer->numStartingShips++;
		//if this was the first ship for a particular team, activate that team
		if(teamPointer->state == ABSENT){
			teamPointer->state = ACTIVE;
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
				break;
			}
		}
	}
	
	//complete the linked list loop for each team that has at least one ship
	for(u32 teamIndex = 0; teamIndex < NUM_TEAMS; teamIndex++){
		TeamData *teamPointer = &mapData.teams[teamIndex];
		if(teamPointer->state == ABSENT){
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
		if(mapData.ships[shipIndex].state != READY){
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
}



void processCamera(){
	
	//handle the camera based on its state
	switch(mapData.camera.state){
	case STILL:
		break;
	case PANNING:
		processCameraPan();
		break;
	case TRACKING:
		break;
	}
	
	cameraBoundsCheck(&mapData.camera.xPos, &mapData.camera.yPos);
}

void processCameraPan(){
	
	mapData.camera.xVel += mapData.camera.xAcc;
	mapData.camera.yVel += mapData.camera.yAcc;
	
	mapData.camera.xPos += mapData.camera.xVel;
	mapData.camera.yPos += mapData.camera.yVel;
	
	//if we are at the halfway point in this pan
	if(mapData.camera.actionTimer * 2 == mapData.camera.actionTarget){
		mapData.camera.xAcc = -mapData.camera.xAcc;
		mapData.camera.yAcc = -mapData.camera.yAcc;
	}
	//if we have finished this pan
	else if(mapData.camera.actionTimer == mapData.camera.actionTarget){
		mapData.camera.state = STILL;
		mapData.camera.xPos = (s64)mapData.camera.xTargetPos << POS_PRECISION;
		mapData.camera.yPos = (s64)mapData.camera.yTargetPos << POS_PRECISION;
	}
	mapData.camera.actionTimer++;
}

//initialize a pan that sends the camera to a specific location
void cameraPanInit(s16 xTarget, s16 yTarget, u8 panTime){
	
	//only initialize a pan if the camera is not performing another action
	if(mapData.camera.state != STILL){
		return;
	}
	
	//pan time must be an even number
	panTime >>= 1;
	
	//a pan time of zero means to teleport straight to the target.
	if(panTime == 0){
		mapData.camera.xPos = (s64)xTarget << POS_PRECISION;
		mapData.camera.yPos = (s64)yTarget << POS_PRECISION;
	}
	else{
		mapData.camera.xTargetPos = xTarget;
		mapData.camera.yTargetPos = yTarget;
		mapData.camera.state = PANNING;
		mapData.camera.actionTarget = panTime << 1;
		mapData.camera.actionTimer = 0;
		mapData.camera.xVel = 0;
		mapData.camera.yVel = 0;
		
		//figure out the acceleration 
		mapData.camera.xAcc = 2 * (inverseTimeSquared[panTime]) * (xTarget - (mapData.camera.xPos >> POS_PRECISION));
		mapData.camera.yAcc = 2 * (inverseTimeSquared[panTime]) * (yTarget - (mapData.camera.yPos >> POS_PRECISION));
	}
}

void cameraBoundsCheck(s64 *xPosPointer, s64 *yPosPointer){
	s64 xPos = *xPosPointer;
	s64 yPos = *yPosPointer;
	
	//bounds check the final result
	if(xPos < 0){
		xPos = 0;
	}
	else if(xPos > (s64)((mapData.xSize << 4) - 223) << POS_PRECISION){
		xPos = (s64)((mapData.xSize << 4) - 223) << POS_PRECISION;
	}
	if(yPos < 0){
		yPos = 0;
	}
	else if(yPos > (s64)((mapData.ySize << 4) - 143) << POS_PRECISION){
		yPos = (s64)((mapData.ySize << 4) - 143) << POS_PRECISION;
	}
	
	*xPosPointer = xPos;
	*yPosPointer = yPos;
}

//a temprary function to initialize a test map.
void initMap(){
	mapData.numShips = 6;
	mapData.teams[BLUE_TEAM].state = ABSENT;
	mapData.teams[GREEN_TEAM].state = ABSENT;
	mapData.teams[YELLOW_TEAM].state = ABSENT;
	
	mapData.ships[0].type = SCOUT;
	mapData.ships[0].state = READY;
	mapData.ships[0].team = RED_TEAM;
	mapData.ships[0].health = 100;
	mapData.ships[0].xPos = 0;
	mapData.ships[0].yPos = 0;
	mapData.ships[0].xVel = 5;
	mapData.ships[0].yVel = 5;
	
	mapData.ships[1].type = BOMBER;
	mapData.ships[1].state = READY;
	mapData.ships[1].team = RED_TEAM;
	mapData.ships[1].health = 100;
	mapData.ships[1].xPos = 16;
	mapData.ships[1].yPos = 16;
	mapData.ships[1].xVel = -1;
	mapData.ships[1].yVel = 1;
	
	mapData.ships[2].type = DESTROYER;
	mapData.ships[2].state = READY;
	mapData.ships[2].team = RED_TEAM;
	mapData.ships[2].health = 100;
	mapData.ships[2].xPos = 17;
	mapData.ships[2].yPos = 5;
	mapData.ships[2].xVel = -3;
	mapData.ships[2].yVel = 1;
	
	mapData.ships[3].type = FIGHTER;
	mapData.ships[3].state = READY;
	mapData.ships[3].team = RED_TEAM;
	mapData.ships[3].health = 100;
	mapData.ships[3].xPos = 5;
	mapData.ships[3].yPos = 17;
	mapData.ships[3].xVel = 4;
	mapData.ships[3].yVel = 10;
	
	mapData.ships[4].type = CRUISER;
	mapData.ships[4].state = READY;
	mapData.ships[4].team = RED_TEAM;
	mapData.ships[4].health = 100;
	mapData.ships[4].xPos = 10;
	mapData.ships[4].yPos = 5;
	mapData.ships[4].xVel = 16;
	mapData.ships[4].yVel = 16;
	
	mapData.ships[5].type = BATTLESHIP;
	mapData.ships[5].state = READY;
	mapData.ships[5].team = RED_TEAM;
	mapData.ships[5].health = 100;
	mapData.ships[5].xPos = 255;
	mapData.ships[5].yPos = 255;
	mapData.ships[5].xVel = -50;
	mapData.ships[5].yVel = -50;
	
	for(u32 i = mapData.numShips; i < MAX_SHIPS; i++){
		mapData.ships[i].state = NOT_PARTICIPATING;
	}
}