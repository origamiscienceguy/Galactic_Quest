#ifndef gameplayh
#define gameplayh

//includes
#include "scenes.h"
#include "audio_engine_external.h"

//constants
#define MAX_SHIPS 256
#define MAX_MAP_SIZE 256
#define NUM_TEAMS 4

#define BG_SHIPS_CHARDATA 0
#define BG_GRID_HIGHLIGHT_CHARDATA 0
#define BG_STARRY_IMAGE_CHARDATA 1
#define BG_HEALTH_SQUARES_CHARDATA 0
#define OBJ_SPRITE_CHARDATA 0
#define BG_SHIPS_TILEMAP 31
#define BG_GRID_HIGHLIGHT_TILEMAP 30
#define BG_STARRY_IMAGE_TILEMAP 26
#define BG_HEALTH_SQUARES_TILEMAP 25

#define BG_SHIPS_PAL_START 0
#define BG_SHIPS_PAL_SIZE 8
#define BG_SHIPS_GFX_START 0
#define BG_SHIPS_GFX_SIZE 231
#define BG_SHIPS_ROTATION_OFFSET 28
#define BG_SHIPS_ANIMATION_OFFSET 112

#define BG_GRID_HIGHLIGHT_PAL_START (BG_SHIPS_PAL_START + BG_SHIPS_PAL_SIZE)
#define BG_GRID_HIGHLIGHT_PAL_SIZE 1
#define BG_GRID_HIGHLIGHT_GFX_START (BG_SHIPS_GFX_START + BG_SHIPS_GFX_SIZE)
#define BG_GRID_HIGHLIGHT_GFX_SIZE 48
#define BG_GRID_HIGHLIGHT_GRID_OFF_OFFSET 0
#define BG_GRID_HIGHLIGHT_GRID_ON_OFFSET 5

#define BG_CYCLE_ANIMATION_PAL_START (BG_GRID_HIGHLIGHT_PAL_START + BG_GRID_HIGHLIGHT_PAL_SIZE)
#define BG_CYCLE_ANIMATION_PAL_SIZE 1
#define BG_CYCLE_ANIMATION_GFX_START (BG_GRID_HIGHLIGHT_GFX_START + BG_GRID_HIGHLIGHT_GFX_SIZE)
#define BG_CYCLE_ANIMATION_GFX_SIZE 19

#define BG_STARRY_IMAGE_PAL_START 12
#define BG_STARRY_IMAGE_PAL_SIZE 1
#define BG_STARRY_IMAGE_GFX_START 0
#define BG_STARRY_IMAGE_GFX_SIZE 73

#define OBJ_SELECTED_SHIP_SPRITE 0
#define OBJ_SELECTED_SHIP_GFX 0
#define OBJ_SELECTED_SHIP_GFX_SIZE 16
#define OBJ_SELECTED_SHIP_AFFINE_MAT 0
#define OBJ_SELECTED_SHIP_PAL_START 0
#define OBJ_SELECTED_SHIP_PAL_SIZE 4

#define OBJ_MINIMAP1_SPRITE 6
#define OBJ_MINIMAP2_SPRITE 7
#define OBJ_MINIMAP_GFX (OBJ_SELECTED_SHIP_GFX + OBJ_SELECTED_SHIP_GFX_SIZE)
#define OBJ_MINIMAP_GFX_SIZE 64

#define OBJ_MINIMAP_CURSOR_SPRITE 2
#define OBJ_MINIMAP_CURSOR_GFX (OBJ_MINIMAP_GFX + OBJ_MINIMAP_GFX_SIZE)
#define OBJ_MINIMAP_CURSOR_GFX_SIZE 1
#define OBJ_MINIMAP_CURSOR_PAL_START (OBJ_SELECTED_SHIP_PAL_START + OBJ_SELECTED_SHIP_PAL_SIZE)
#define OBJ_MINIMAP_CURSOR_PAN_SIZE 1

#define OBJ_CURSOR_SPRITE 1
#define OBJ_CURSOR_GFX (OBJ_MINIMAP_CURSOR_GFX + OBJ_MINIMAP_CURSOR_GFX_SIZE)
#define OBJ_CURSOR_GFX_SIZE 32



#define CYCLE_PAN_SPEED 20
#define SHIP_MOVE_SPEED 32
#define CURSOR_WAIT_FRAMES 20
#define CURSOR_SLOW_SPEED_1 4
#define CURSOR_SLOW_SPEED_2 8
#define CURSOR_FAST_SPEED 16

#define SHIP_ACC 5
#define MINIMAP_MOVE_FRAMES 4
#define MINIMAP_YPOS 48

//enums
enum ShipType{
	SCOUT, FIGHTER, BOMBER, DESTROYER, CRUISER, BATTLESHIP, CARRIER,
};

enum ShipState{
	READY_VISIBLE, READY_HIDDEN, FINISHED_VISIBLE, FINISHED_HIDDEN, WRONG_TEAM_VISIBLE, WRONG_TEAM_HIDDEN, SELECTED, DOCKED, DESTROYED, NOT_PARTICIPATING
};

enum MapState{
	TURN_START, OPEN_MAP, SHIP_SELECTED, SHIP_MOVEMENT_SELECT, SHIP_MOVING, BATTLE, TURN_END, TURN_END_MOVEMENT, TURN_REPLAY
};

enum TeamState{
	TEAM_ABSENT, TEAM_ACTIVE, TEAM_DEFEATED
};

enum Team{
	RED_TEAM, BLUE_TEAM, GREEN_TEAM, YELLOW_TEAM
};

enum CameraState{
	CAM_STILL, CAM_PANNING, CAM_TRACKING, 
};

enum CursorState{
	CUR_HIDDEN, CUR_STILL, CUR_MOVE_ONCE_1, CUR_MOVE_ONCE_2, CUR_MOVE_ONCE_3, CUR_MOVE_ONCE_WAIT, CUR_MOVE_SLOW_1, CUR_MOVE_SLOW_2, CUR_MOVE_FAST
};

enum CursorDirection{
	CUR_NO_DIRECTION, CUR_RIGHT, CUR_UP, CUR_LEFT, CUR_DOWN, CUR_UP_RIGHT, CUR_UP_LEFT, CUR_DOWN_RIGHT, CUR_DOWN_LEFT
};

enum HighlightState{
	NO_HIGHLIGHT, MOVEMENT_RANGE_HIGHLIGHT, VISIBILITY_HIGHLIGHT, POTENTIAL_ENCOUNTERS_HIGHLIGHT
};

enum MinimapState{
	MINIMAP_HIDDEN_LEFT, MINIMAP_HIDDEN_RIGHT, MINIMAP_STILL_LEFT, MINIMAP_STILL_RIGHT, 
	MINIMAP_MOVING_LEFT, MINIMAP_MOVING_RIGHT, MINIMAP_HIDING_LEFT, MINIMAP_HIDING_RIGHT,
	MINIMAP_EMERGING_LEFT, MINIMAP_EMERGING_RIGHT
};

//structs
typedef struct ShipData{
	enum ShipType type; //the type of ship this one is
	enum ShipState state; //the state of this ship
	enum Team team; //which team this ship belongs to
	u8 health; //the health of this ship, 100 is max, 0 is dead
	u8 xPos; //the x position of this ship on the map
	u8 yPos; //the y position of this ship on the map
	s8 xVel; //the x velocity of this ship
	s8 yVel; //the y velocity of this ship
	u8 teamLink; //the index of the next ship of the same team, forming a linked list.
	u8 sameTileLink; //the index of the next ship in the same tile as this one, forming a linked list. If this points to itself, there are no others
	u8 padding[6];
}ShipData;

typedef struct SelectedShip{
	u8 index; //the index of the currently selected ship
	u8 angle; //the angle the ship graphic is facing, right is 0 and 256, cw
	s16 xPos; //the current pixel position of the center of the selected ship
	s16 yPos; //the current pixel position of the center of the selected ship
	s16 xInitial; //the initial position of the selected ship before movement
	s16 yInitial; //the initial position of the selected ship before movement
	s16 xTarget; //the position the selected ship is moving towards
	s16 yTarget; //the position the selected ship is moving towards
	u8 animationTimer; //the timer of the selected ships animation
	u8 actionTimer; //the timer of the selected ships movement
	u8 actionTarget; //how long this movement should take
}SelectedShip;

typedef struct TeamData{
	enum TeamState state; //the state of this team in this particular battle
	u8 firstShip; //the index of the first ship in every team a new turn will begin with this ship highlighted
	u8 numStartingShips; //the number of ships each team began the battle with
	u8 numAliveShips; //the number of ships each team has that is currently alive
}TeamData;

typedef struct CameraData{
	enum CameraState state; //what the camera is currently doing
	u8 actionTimer; //how long the camera has been performing an action
	u8 actionTarget; //how long the camera is supposed to be performing an action
	s16 xPos; //the current x pixel position of the top left pixel on screen. 16.48 fixed point
	s16 yPos; //the current y pixel position of the top left pixel on screen. 16.48 fixed point
	s16 xStartingPos; //the initial x position of the camera for this movement
	s16 yStartingPos; //the initial y position of the camera for this movement
	s16 xTargetPos; //the x position the camera is currently seeking towards
	s16 yTargetPos; //the y position the camera is currently seeking towards
}CameraData;

typedef struct CursorData{
	s16 xPos; //the pixel position of the cursor in the x axis
	s16 yPos; //the pixel position of the cursor in the y axis
	u8 selectXPos; //the map position that the cursor will select
	u8 selectYPos; //the map position that the cursor will select
	enum CursorState state; //the state of the cursor
	enum CursorDirection direction; //the direction of the cursor
	u8 counter; //how long the button to move the cursor has been held down
}CursorData;

typedef struct HighlightData{
	enum HighlightState state;
}HighlightData;

typedef struct MinimapData{
	enum MinimapState state;
	u8 actionTimer;
	u8 actionTarget;
	u8 updateRequest;
}MinimapData;

typedef struct MapData{
	enum MapState state; //what is the stage of the game are we in
	enum Team teamTurn; //who's turn is it
	u8 turnNum; //how many turns have elapsed
	u8 xSize; //the number of tiles large the map is in the x direction
	u8 ySize; //number of tiles in the y direction
	u8 numShips; //the number of ships on the map.
	u8 actionTimer; //how many frames have elapsed in a state action
	u8 actionTarget; //how many frames a state action should take
	ShipData ships[MAX_SHIPS]; //the array of information about every ship in this battle
	TeamData teams[NUM_TEAMS]; //the array of information about each team in the battle
	CameraData camera; //the struct containing data about the camera
	SelectedShip selectedShip; //the struct containing data about the currently selected ship
	CursorData cursor; //the struct containing data about the cursor
	HighlightData highlight; //the data about the highlight layer
	MinimapData minimap; //the data about the minimap layer
}MapData;

//globals
extern const u32 inverseTimeSquared[];
extern const u16 inverseTime[];
extern const s16 sinTable[];
extern const u8 arctanTable1[];
extern const u8 arctanTable2[];
extern const unsigned short ships_selectedTiles[];
extern const unsigned short ships_selectedMap[];
extern const unsigned short ships_selectedPal[64];
extern const unsigned short cursorTiles[512];
extern const unsigned short minimap_cursorTiles[16];
extern const unsigned short minimap_cursorPal[16];
extern const unsigned short startfield_samplePal[64];
extern const unsigned short startfield_sampleMap[1600];
extern const unsigned short startfield_sampleTiles[1168];
extern const unsigned short startfield_sampleMetaTiles[4096];
extern const unsigned short QuickStarMapTiles[688];
extern const unsigned short QuickStarMapMetaTiles[4096];
extern const unsigned short QuickStarMapMap[4096];
extern const unsigned short QuickStarMapPal[64];
extern const unsigned short bg_shipsTiles[3696];
extern const unsigned short bg_shipsMap[336];
extern const unsigned short bg_shipsPal[128];
extern const unsigned short bg_grid_highlightTiles[768];
extern const unsigned short bg_grid_highlightMap[100];
extern const unsigned short bg_grid_highlightPal[16];
extern const unsigned short bg_cycle_animTiles[304];
extern const unsigned short bg_cycle_animMap[36];
extern const unsigned short bg_cycle_animPal[16];

extern const u8 minimapPositions[];

//local functions
void gameplayInitialize();
void gameplayIntro();
void gameplayNormal();
void gameplayPause();
void gameplayOutro();
void gameplayEnd();
void shipListInit();
void createShipTilemap(u16 *);
void createGridTilemap(u16 *);
void drawSelectedShip(OBJ_ATTR *, u8 *);
void drawCursor(OBJ_ATTR *);
void drawHighlight(u8 *);
void drawMinimap(OBJ_ATTR *, u8 *);
void updateMinimap(u8 *);
void nextPlayer();
void nextTurn();
void turnStartState();
void openMapState();
void shipSelectedState();
void shipMovementSelectState();
void shipMovingState();
void turnEndState();
void turnEndMovementState();
void processCamera();
void cameraPanInit(s16, s16, u8);
void processCameraPan();
void cameraBoundsCheck(s16 *, s16 *);
void shipMoveInit(s16, s16, u8);
void processShipMovement();
void checkForOverlap(u8);
void makeShipVisible(u8);
void makeShipHidden(u8);
u8 isShipVisible(u8);
void moveCursor();
void selectShip(u8);
u8 arctan2(s16, s16);
void checkCycleButtons();
void revealMinimap();
void hideMinimap();
void cursorBoundsCheck();

//temp function
void initMap();

//external functions

#endif

