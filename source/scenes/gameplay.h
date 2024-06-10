#ifndef gameplayh
#define gameplayh

//includes
#include "scenes.h"
#include "audio_engine_external.h"

//constants
#define MAX_SHIPS 256
#define MAX_MAP_SIZE 256
#define NUM_TEAMS 4

#define BG_0_TILEMAP 31
#define BG_0_CHARDATA 0

#define BG_1_TILEMAP 30

//enums
enum ShipType{
	SCOUT, FIGHTER, BOMBER, DESTROYER, CRUISER, BATTLESHIP, CARRIER,
};

enum ShipState{
	READY, FINISHED, UNAVAILABLE, DESTROYED, NOT_PARTICIPATING
};

enum MapState{
	TURN_START, OPEN_MAP, SHIP_SELECTED, BATTLE, TURN_END, TURN_REPLAY
};

enum TeamState{
	ABSENT, ACTIVE, DEFEATED
};

enum Team{
	RED_TEAM, BLUE_TEAM, GREEN_TEAM, YELLOW_TEAM
};

enum CameraState{
	STILL, PANNING, TRACKING, 
};

//structs
typedef struct ShipData{
	enum ShipType type; //the type of ship this one is
	enum ShipState state; //the state of this ship
	enum Team team; //which team this ship belongs to
	u8 health; //the health of this ship, 100 is max, 0 is dead
	u8 xPos; //the x position of this ship on the map
	u8 yPos; //the y position of this ship on the map
	u8 xVel; //the x velocity of this ship
	u8 yVel; //the y velocity of this ship
	u8 teamLink; //the index of the next ship of the same team, forming a linked list.
	u8 activeLink; //the index of the next ship that has not yet moved, forming a linked list.
	u8 sameTileLink; //the index of the next ship in the same tile as this one, forming a linked list. If this points to itself, there are no others
}ShipData;

typedef struct TeamData{
	enum TeamState state; //the state of this team in this particular battle
	u8 firstShip; //the index of the first ship in every team a new turn will begin with this ship highlighted
	u8 numStartingShips; //the number of ships each team began the battle with
	u8 numActiveShips; //the number of ships each team has that is currently alive
}TeamData;

typedef struct CameraData{
	enum CameraState state; //what the camera is currently doing
	s32 xPos; //the current x pixel position of the top left pixel on screen. 24.8 fixed point
	s32 yPos; //the current y pixel position of the top left pixel on screen. 24.8 fixed point
	s32 xLastPos; //the x position of the top left tile on screen last frame. 24.8 fixed point
	s32 yLastPos; //the y position of the top left tile on screen last frame. 24.8 fixed point
	s16 xInitialPos; //the x position before a camera movement action started
	s16 yInitialPos; //the y position before a camera movement action started
	s16 xTargetPos; //the x position the camera is currently seeking towards
	s16 yTargetPos; //the y position the camera is currently seeking towards
	s32 xVel; //the current velocity of the camera in the x direction. pixels per frame
	s32 yVel; //the current velocity of the camera in the y direction. pixels per frame
	s32 xacc; //the current acceleration of the camera in x direction. pixels per frame per frame. 24.8 fixed point
	s32 yacc; //the current acceleration of the camera in y direction. pixels per frame per frame. 24.8 fixed point
}CameraData;

typedef struct MapData{
	enum MapState state; //what is the stage of the game are we in
	enum Team teamTurn; //who's turn is it
	u8 xSize; //the number of tiles large the map is in the x direction
	u8 ySize; //number of tiles in the y direction
	u8 numShips; //the number of ships on the map.
	u8 selectedShip; //the index of the ship currently/last selected
	ShipData ships[MAX_SHIPS]; //the array of information about every ship in this battle
	TeamData teams[NUM_TEAMS]; //the array of information about each team in the battle
	CameraData camera; //the struct containing data about the camera
}MapData;

//globals
extern const unsigned int TestGfxPal[];
extern const unsigned int TestGfxTiles[];

//local functions
void gameplayInitialize();
void gameplayIntro();
void gameplayNormal();
void gameplayPause();
void gameplayOutro();
void gameplayEnd();
void shipListInit();
void createShipTilemap(u16 *);
void turnStartState();
void openMapState();
void turnEndState();
void cameraPanTo(s16, s16, u16);

//temp function
void initMap();

//external functions

#endif

