#ifndef gameplayh
#define gameplayh

//includes
#include "scenes.h"
#include "audio_engine_external.h"

//constants
#define MAX_SHIPS 256
#define MAX_MAP_SIZE 256

#define BG_0_TILEMAP 31
#define BG_0_CHARDATA 0

#define BG_1_TILEMAP 30

//enums
enum ShipType{
	RED_SCOUT, RED_FIGHTER, RED_BOMBER, RED_DESTROYER, RED_CRUISER, RED_BATTLESHIP, RED_CARRIER,
	BLUE_SCOUT, BLUE_FIGHTER, BLUE_BOMBER, BLUE_DESTROYER, BLUE_CRUISER, BLUE_BATTLESHIP, BLUE_CARRIER,
	GREEN_SCOUT, GREEN_FIGHTER, GREEN_BOMBER, GREEN_DESTROYER, GREEN_CRUISER, GREEN_BATTLESHIP, GREEN_CARRIER,
	YELLOW_SCOUT, YELLOW_FIGHTER, YELLOW_BOMBER, YELLOW_DESTROYER, YELLOW_CRUISER, YELLOW_BATTLESHIP, YELLOW_CARRIER,
};

enum ShipState{
	READY, FINISHED, UNAVAILABLE, ACTION_0_PREMOVE, ACTION_1_PREMOVE, ACTION_2_PREMOVE,
	ACTION_3_PREMOVE, ACTION_0_POSTMOVE, ACTION_1_POSTMOVE, ACTION_2_POSTMOVE, ACTION_3_POSTMOVE,
};

//structs
typedef struct ShipData{
	enum ShipType type; //the type of ship this one is
	enum ShipState state; //the state of this ship
	u8 index; //the index of this ship in the array of all ships
	u8 health; //the health of this ship, 100 is max, 0 is dead
	u8 xPos; //the x position of this ship on the map
	u8 yPos; //the y position of this ship on the map
	u8 xVel; //the x velocity of this ship
	u8 yVel; //the y velocity of this ship
}ShipData;

typedef struct MapData{
	u16 xPos; //the x position of the top left tile on screen
	u16 yPos; //the y position of the top left tile on screen
	u16 xLastPos; //the x position of the top left tile on screen last frame
	u16 yLastPos; //the y position of the top left tile on screen last frame
	u8 xSize; //the number of tiles large the map is in the x direction
	u8 ySize; //number of tiles in the y direction
	u8 numShips; //the number of ships on the map.
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
void createShipTilemap(ShipData *, u16 *, MapData *);

//temp function
void initMap();

//external functions

#endif

