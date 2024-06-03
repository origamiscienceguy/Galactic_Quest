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
enum PlayerState{
	PLAYER1, PLAYER2, PLAYER3, PLAYER4
};

enum ShipType{
	SCOUT, FIGHTER, BOMBER, DESTROYER, CRUISER, BATTLESHIP, CARRIER
};

enum ShipState{
	READY, FINISHED, UNAVAILABLE, ACTION_0_PREMOVE, ACTION_1_PREMOVE, ACTION_2_PREMOVE,
	ACTION_3_PREMOVE, ACTION_0_POSTMOVE, ACTION_1_POSTMOVE, ACTION_2_POSTMOVE, ACTION_3_POSTMOVE
};

//structs
typedef struct ShipData{
	enum PlayerState player; //which player this ship belongs to
	enum ShipType type; //the type of ship this one is
	enum ShipState state; //the state of this ship
	u8 index; //the index of this ship in the array of all ships
	u8 xPos; //the x position of this ship on the map
	u8 yPos; //the y position of this ship on the map
	u8 xVel; //the x velocity of this ship
	u8 yVel; //the y velocity of this ship
	u8 health; //the health of this ship, 100 is max, 0 is dead
}ShipData;

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

//external functions

#endif

