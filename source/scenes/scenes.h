#ifndef scenesh
#define scenesh

//includes
#include "tonc.h"
#include "main.h"

//constants

//enums
enum SceneState{
	INITIALIZE, INTRO, NORMAL, PAUSE, OUTRO, END
};

enum SceneIndex{
	GAMEPLAY, INTRO_SCENE, MAIN_MENU
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
	u32 sceneCounter; //how long this scene has been going for
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
extern VideoData characterData[6] EWRAM_DATA;
extern VideoData paletteData[2] EWRAM_DATA;
extern VideoData IOData[8] EWRAM_DATA;
extern VideoData OAMData EWRAM_DATA;

extern u16 tilemapBuffer0[0x1000] EWRAM_DATA;
extern u16 tilemapBuffer1[0x1000] EWRAM_DATA;
extern u16 tilemapBuffer2[0x1000] EWRAM_DATA;
extern u16 tilemapBuffer3[0x1000] EWRAM_DATA;
extern u8 characterBuffer0[0x4000] EWRAM_DATA;
extern u8 characterBuffer1[0x4000] EWRAM_DATA;
extern u8 characterBuffer2[0x4000] EWRAM_DATA;
extern u8 characterBuffer3[0x4000] EWRAM_DATA;
extern u8 characterBuffer4[0x4000] EWRAM_DATA;
extern u8 characterBuffer5[0x4000] EWRAM_DATA;
extern u16 paletteBufferBg[0x100] EWRAM_DATA;
extern u16 paletteBufferObj[0x100] EWRAM_DATA;
extern OBJ_ATTR objectBuffer[128] EWRAM_DATA;
extern u16 IOBuffer0[0x200] EWRAM_DATA;
extern u16 IOBuffer1[0x200] EWRAM_DATA;
extern u16 IOBuffer2[0x200] EWRAM_DATA;
extern u16 IOBuffer3[0x200] EWRAM_DATA;
extern u16 IOBuffer4[0x200] EWRAM_DATA;
extern u16 IOBuffer5[0x200] EWRAM_DATA;
extern u16 IOBuffer6[0x200] EWRAM_DATA;
extern u16 IOBuffer7[0x200] EWRAM_DATA;

//local functions

//external functions

#endif


