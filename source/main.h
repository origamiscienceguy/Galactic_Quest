#ifndef mainh
#define mainh

//includes
#include "tonc.h"
#include "scenes.h"
#include "interrupt.h"
#include "audio_engine_external.h"


//constants
//options menu constants
#define MAX_VOLUME 10        // Maximum volume level (0-10)
#define VOLUME_SCALE 256     // Scale to represent 100% (fixed-point 8.8 format)
#define DEFAULT_MASTER_VOLUME 10 // Default SFX volume as a percentage (100% of 10)
#define DEFAULT_BGM_VOLUME 8 // Default BGM volume as a percentage (80% of 10)
#define DEFAULT_SFX_VOLUME 10 // Default SFX volume as a percentage (100% of 10)
#define DEFAULT_GRID_FLAG true

//define constants for Sound Test
#define BGM_SINGLE 999 // Marker for single track

//enums
enum GameLoopState{WORKING, WAITING_FOR_VBLANK};

//structs
typedef struct Inputs{
	u16 current;
	u16 pressed;
	u16 held;
	u16 released;
}Inputs;

typedef struct Options{
	u8 gridOn;
	u8 masterVolume;
	u8 bgmVolume;
	u8 sfxVolume;
	u8 lastPlayedSaveSlot;
	bool firstTimeBoot;
}Options;

typedef struct SoundChannel{
	u8 assetIndex;
	u8 defaultVolume;
}SoundChannel;

#define SOUND_TEST_BGM_COUNT (int)(sizeof(bgmGroups) / sizeof(bgmGroups[0]))
#define SOUND_TEST_SFX_COUNT 15
#define TOTAL_SOUND_COUNT 26 //rip numSounds
#define BGM_COUNT (TOTAL_SOUND_COUNT - SOUND_TEST_SFX_COUNT) // Technically not correct if SFX also has groups, but it works :p
#define SFX_START 11

enum BGMList{
	BGM_OPENING,
	BGM_TITLE,
	BGM_MAINMENU,
	BGM_THEMEA,
	BGM_THEMEB,
	BGM_THEMEB_BATTLE,
	BGM_THEMEC,
	BGM_THEMED,
	BGM_MAX
};

// Precomputed fixed-point volume multipliers for each volume level (0-10)
extern const int volumeTable[MAX_VOLUME + 1];

//globals
extern u32 globalCounter;
extern u8 taskData[64];
extern Inputs inputs;
extern volatile enum GameLoopState gameLoopState;
extern vu8 gameState;
extern Options options;

//local functions
void gameLoop();
void globalInitialize();
void softReset();
__attribute__ ((noreturn)) void gameLoop();
void criticalUpdates();
void sceneManager();
void updateGraphics();

//external functions
extern void updateSoundVolumes(bool leavingOptionsMenu);
extern void playBGM(u8 bgmIndex);
extern void playSFX(u8 assetIndex, int sfxSlot);
extern void stopAllSoundExcept(const u8* exception);
extern void stopAllSound();
extern void stopSFX(u8 soundIndex);
extern void endCurrentBGM();

#endif


