#ifndef scenesh
#define scenesh

//includes
#include "tonc.h"

//constants

//enums
enum SceneState{
	INITIALIZE, INTRO, NORMAL, PAUSE, OUTRO, END
};

enum SceneIndex{
	GAMEPLAY
};

//structs
typedef const struct Scene{
	void (*const initialize)(); //pointer to this scene's init function
	void (*const intro)(); //pointer to this scene's intro function
	void (*const normal)(); //pointer to this scene's normal function
	void (*const pause)(); //pointer to this scene's pause function
	void (*const outro)(); //pointer to this scene's outro function
	void (*const end)(); //pointer to this scene's end function
}Scene;

typedef struct SceneStatus{
	enum SceneState state;
	Scene *scenePointer;
}SceneStatus;

typedef struct VideoData{
	u16 size; //how many words large this character data buffer is
	void *position; //where in VRAM this data should be sent to
	void *buffer; //pointer to the actual data
}VideoData;

//globals
extern const Scene *sceneList[];
extern SceneStatus currentScene;
extern VideoData tilemapData[4] EWRAM_DATA;
extern VideoData characterData[4] EWRAM_DATA;
extern VideoData paletteData[16] EWRAM_DATA;

//local functions

//external functions

#endif

