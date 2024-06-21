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
	REG_BG0CNT = BG_4BPP | BG_SBB(BG_0_TILEMAP) | BG_CBB(BG_0_CHARDATA);
	REG_BG1CNT = BG_4BPP | BG_SBB(BG_1_TILEMAP) | BG_CBB(BG_0_CHARDATA);
	
	//queue the palette to be sent
	paletteData[0].size = 64;
	paletteData[0].buffer = (void *)shipsPal;
	paletteData[0].position = pal_bg_mem;
	
	//queue the tiles to be sent
	characterData[0].size = 1760;
	characterData[0].buffer = (void *)shipsTiles;
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
			tilemapBuffer0[i * 64 + j * 2] = shipsMap[0];
			tilemapBuffer0[i * 64 + j * 2 + 1] = shipsMap[1];
			tilemapBuffer0[i * 64 + j * 2 + 32] = shipsMap[2];
			tilemapBuffer0[i * 64 + j * 2 + 33] = shipsMap[3];
		}
	}
	
	mapData.camera.xPos = 0;
	mapData.camera.yPos = 0;
	mapData.camera.xLastPos = 0x7fff;
	mapData.camera.yLastPos = 0x7fff;
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
	u16 xUpdateLow = 0; //lower bound of horizontal map cell that need to be updated
	u16 xUpdateHigh = 0; //upper bound of horizontal map cell that need to be updated
	u16 yUpdateLow = 0; //lower bound of vertical map cell that need to be updated
	u16 yUpdateHigh = 0; //upper bound of vertical map cell that need to be updated
	
	//convert pixel coordinates to map cell coordinates
	u8 mapXPos = mapData.camera.xPos >> 4;
	u8 mapYPos = mapData.camera.yPos >> 4; 
	u8 mapXLastPos = mapData.camera.xLastPos >> 4;
	u8 mapYLastPos = mapData.camera.yLastPos >> 4;

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
			tilemapBuffer[(i % 16) * 2 + j * 64] = shipsMap[3];
			tilemapBuffer[(i % 16) * 2 + j * 64 + 1] = shipsMap[3];
			tilemapBuffer[(i % 16) * 2 + j * 64 + 32] = shipsMap[3];
			tilemapBuffer[(i % 16) * 2 + j * 64 + 33] = shipsMap[3];
		}
	}
	
	//clear the horizontal column of newly arrived spaces
	for(int i = yUpdateLow; i < yUpdateHigh; i++){
		for(int j = 0; j < 16; j++){
			tilemapBuffer[(i % 16) * 64 + j * 2] = shipsMap[3];
			tilemapBuffer[(i % 16) * 64 + j * 2 + 1] = shipsMap[3];
			tilemapBuffer[(i % 16) * 64 + j * 2 + 32] = shipsMap[3];
			tilemapBuffer[(i % 16) * 64 + j * 2 + 33] = shipsMap[3];
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

	//draw every ship that should be on screen
	for(u32 shipIndex = 0; shipIndex < mapData.numShips; shipIndex++){
		//if the ship is not visible, skip it
		if((mapData.ships[shipIndex].state != READY_VISIBLE) && (mapData.ships[shipIndex].state != FINISHED_VISIBLE) && 
		(mapData.ships[shipIndex].state != WRONG_TEAM_VISIBLE)){
			continue;
		}

		u8 shipXPos = mapData.ships[shipIndex].xPos;
		u8 shipYPos = mapData.ships[shipIndex].yPos;
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
			u16 tilemapBase = ((mapData.ships[shipIndex].type + 1) * 4) + (globalIdleCounter * IDLE_CYCLE_OFFSET) + (shipDirection * DIRECTION_OFFSET);
			tilemapBuffer[baseIndex] = shipsMap[tilemapBase] | SE_PALBANK(mapData.ships[shipIndex].team);
			tilemapBuffer[baseIndex + 1] = shipsMap[tilemapBase + 1] | SE_PALBANK(mapData.ships[shipIndex].team);
			tilemapBuffer[baseIndex + 32] = shipsMap[tilemapBase + 2] | SE_PALBANK(mapData.ships[shipIndex].team);
			tilemapBuffer[baseIndex + 33] = shipsMap[tilemapBase + 3] | SE_PALBANK(mapData.ships[shipIndex].team);
		}
	}
	
	//update the map previous position
	mapData.camera.xLastPos = mapData.camera.xPos;
	mapData.camera.yLastPos = mapData.camera.yPos;
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
				mapData.selectedShip = shipIndex;
			}
			numAliveShips++;
		}
		else if(mapData.ships[shipIndex].state == WRONG_TEAM_HIDDEN){
			mapData.ships[shipIndex].state = READY_HIDDEN;
			//if this is the first alive ship found
			if(numAliveShips == 0){
				mapData.selectedShip = shipIndex;
			}
			numAliveShips++;
		}
		shipIndex = mapData.ships[shipIndex].teamLink;
	}
	if(mapData.ships[shipIndex].state == WRONG_TEAM_VISIBLE){
		mapData.ships[shipIndex].state = READY_VISIBLE;
		//if this is the first alive ship found
		if(numAliveShips == 0){
			mapData.selectedShip = shipIndex;
		}
		numAliveShips++;
	}
	else if(mapData.ships[shipIndex].state == WRONG_TEAM_HIDDEN){
		mapData.ships[shipIndex].state = READY_HIDDEN;
		//if this is the first alive ship found
		if(numAliveShips == 0){
			mapData.selectedShip = shipIndex;
		}
		numAliveShips++;
	}
	
	teamPointer->numAliveShips = numAliveShips;
	
	mapData.state = OPEN_MAP;
}

void openMapState(){
	u16 scrollSpeed = 12;
	
	//the d pad scrolls the map
	if(inputs.current & KEY_RIGHT){
		s16 xTarget = mapData.camera.xPos + scrollSpeed;
		s16 yTarget = mapData.camera.yPos;
		
		cameraPanInit(xTarget, yTarget, 0);
	}
	if(inputs.current & KEY_LEFT){
		s16 xTarget = mapData.camera.xPos - scrollSpeed;
		s16 yTarget = mapData.camera.yPos;
		
		cameraPanInit(xTarget, yTarget, 0);
	}
	if(inputs.current & KEY_DOWN){
		s16 xTarget = mapData.camera.xPos;
		s16 yTarget = mapData.camera.yPos + scrollSpeed;
		
		cameraPanInit(xTarget, yTarget, 0);
	}
	if(inputs.current & KEY_UP){
		s16 xTarget = mapData.camera.xPos;
		s16 yTarget = mapData.camera.yPos - scrollSpeed;
		
		cameraPanInit(xTarget, yTarget, 0);
	}
	
	//L and R cycle backwards or forwards through the active ships for this team, and center the camera on the next ship in the cycle
	if((inputs.pressed & KEY_L) && (mapData.camera.state == STILL)){
		//cycle through the linked list until we end up one before where we started
		u32 currentIndex = mapData.selectedShip;
		u32 lastActiveIndex = 0;
		while(mapData.ships[currentIndex].teamLink != mapData.selectedShip){
			if((mapData.ships[currentIndex].state == READY_VISIBLE) || (mapData.ships[currentIndex].state = READY_HIDDEN)){
				lastActiveIndex = currentIndex;
			}
			currentIndex = mapData.ships[currentIndex].teamLink;
		}
		if((mapData.ships[currentIndex].state == READY_VISIBLE) || (mapData.ships[currentIndex].state = READY_HIDDEN)){
			lastActiveIndex = currentIndex;
		}
		mapData.selectedShip = lastActiveIndex;
		
		s16 xTarget = (mapData.ships[mapData.selectedShip].xPos << 4) - 112;
		s16 yTarget = (mapData.ships[mapData.selectedShip].yPos << 4) - 72;
		
		//pan the camera to this ship
		cameraPanInit(xTarget, yTarget, CYCLE_PAN_SPEED);
	}
	if((inputs.pressed & KEY_R) && (mapData.camera.state == STILL)){
		//cycle to the next ship in the linked list of active ships
		u32 currentIndex = mapData.ships[mapData.selectedShip].teamLink;
		while(mapData.ships[currentIndex].teamLink != mapData.selectedShip){
			if((mapData.ships[currentIndex].state == READY_VISIBLE) || (mapData.ships[currentIndex].state = READY_HIDDEN)){
				break;
			}
			currentIndex = mapData.ships[currentIndex].teamLink;
		}
		mapData.selectedShip = currentIndex;
		
		s16 xTarget = (mapData.ships[mapData.selectedShip].xPos << 4) - 112;
		s16 yTarget = (mapData.ships[mapData.selectedShip].yPos << 4) - 72;
		
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
		mapData.selectedShip = shipIndex;
		//pan the camera to this ship
		s16 xTarget = (mapData.ships[mapData.selectedShip].xPos << 4) - 112;
		s16 yTarget = (mapData.ships[mapData.selectedShip].yPos << 4) - 72;
		cameraPanInit(xTarget, yTarget, CYCLE_PAN_SPEED);
		mapData.actionTimer = 1;
	}
	//if the camera has finished focusing on the next ship
	else if(mapData.camera.state == STILL){
		mapData.actionTimer = 0;
		mapData.ships[mapData.selectedShip].state = FINISHED_VISIBLE;
	}
	
	//handle any changes to the camera that occured this frame
	processCamera();
}

void turnEndMovementState(){
	s16 xStart = mapData.ships[mapData.selectedShip].xPos;
	s16 yStart = mapData.ships[mapData.selectedShip].yPos;
	s16 xEnd = xStart + mapData.ships[mapData.selectedShip].xVel;
	s16 yEnd = yStart + mapData.ships[mapData.selectedShip].yVel;
	
	//if we are starting a new movement
	if(mapData.actionTimer == 0){
		mapData.actionTarget = SHIP_MOVE_SPEED;
	}
	//if the movement has concluded
	else if(mapData.actionTimer == mapData.actionTarget){
		mapData.ships[mapData.selectedShip].state = FINISHED_VISIBLE;
		mapData.state = TURN_END;
		mapData.actionTimer = 0;
	}
	
	mapData.actionTimer++;
	
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
		teamPointer->state = ABSENT;
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
				mapData.ships[shipIndex].state = READY_HIDDEN;
				break;
			}
		}
	}
	mapData.numShips = 112;
	
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
		if(mapData.teams[team].state == ACTIVE){
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
	}while(mapData.teams[team].state != ACTIVE);
	
	mapData.teamTurn = team;
	
	mapData.state = TURN_START;
}

void nextTurn(){
	mapData.turnNum++;
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
	
	//queue the tilemap for layer 1 to be sent
	tilemapData[1].size = 512;
	tilemapData[1].buffer = tilemapBuffer1;
	tilemapData[1].position = se_mem[BG_1_TILEMAP];
	
	//update the tilemap with the new position
	createShipTilemap(tilemapBuffer1);
	
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
		mapData.camera.state = STILL;
		mapData.camera.xPos = mapData.camera.xTargetPos;
		mapData.camera.yPos = mapData.camera.yTargetPos;
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
		mapData.camera.xPos = xTarget;
		mapData.camera.yPos = yTarget;
	}
	else{
		mapData.camera.xTargetPos = xTarget;
		mapData.camera.yTargetPos = yTarget;
		mapData.camera.xStartingPos = mapData.camera.xPos;
		mapData.camera.yStartingPos = mapData.camera.yPos;
		mapData.camera.state = PANNING;
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

//a temprary function to initialize a test map.
void initMap(){
	u8 index = 0;
	for(u32 team = 0; team <= YELLOW_TEAM; team++){
		u8 yPos = 8;
		for(u32 type = 0; type <= CARRIER; type++){
			u8 xPos = 8 + 4 * team;
			
			mapData.ships[index].type = type;
			mapData.ships[index].state = READY_VISIBLE;
			mapData.ships[index].team = team;
			mapData.ships[index].health = 100;
			mapData.ships[index].xPos = xPos;
			mapData.ships[index].yPos = yPos;
			mapData.ships[index].xVel = 1;
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
			mapData.ships[index].yVel = -1;
			
			index++;
			xPos++;
			
			mapData.ships[index].type = type;
			mapData.ships[index].state = READY_VISIBLE;
			mapData.ships[index].team = team;
			mapData.ships[index].health = 100;
			mapData.ships[index].xPos = xPos;
			mapData.ships[index].yPos = yPos;
			mapData.ships[index].xVel = -1;
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
			mapData.ships[index].yVel = 1;
			
			index++;
			yPos++;
		}
	}
	
	for(u32 i = index; i < MAX_SHIPS; i++){
		mapData.ships[i].state = NOT_PARTICIPATING;
	}
}