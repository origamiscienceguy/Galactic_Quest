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

typedef struct MapData{
	enum MapState state; //what is the stage of the game are we in
	enum Team teamTurn; //who's turn is it
	u16 xPos; //the x position of the top left tile on screen
	u16 yPos; //the y position of the top left tile on screen
	u16 xLastPos; //the x position of the top left tile on screen last frame
	u16 yLastPos; //the y position of the top left tile on screen last frame
	u8 xSize; //the number of tiles large the map is in the x direction
	u8 ySize; //number of tiles in the y direction
	u8 numShips; //the number of ships on the map.
	u8 selectedShip; //the index of the ship currently/last selected
	ShipData ships[MAX_SHIPS]; //the array of information about every ship in this battle
	TeamData teams[NUM_TEAMS]; //the array of information about each team in the battle
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
void createShipTilemap(ShipData *, u16 *, MapData *);
void turnStartState();
void openMapState();
void turnEndState();

//temp function
void initMap();

//external functions

#endif

