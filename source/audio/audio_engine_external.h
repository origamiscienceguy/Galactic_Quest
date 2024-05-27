#include "audio_engine_settings.h"
#include "audio_assets_enum.h"

//processAudio must be ran once per frame, and must finish before the timer 1 interrupt fires.
extern void processAudio();

//will enable a vcount interrupt just once. Two frames after this function is invoked, it is safe to use Vcount interrupts again.
//Timer 0, Timer 1, Timer 2, DMA 0, DMA 1, and all the sound registers must not be touched from now until the engine is disabled.
extern void audioInitialize();

//will add a new song to the queue if it's priority is higher than another songs in the queue.
//Return value is the index into the current song array.
extern u8 playNewAsset(u16);

//will remove an asset from the queue. Can be called at any time by the programmer, or called automatically after an asset reaches the end.
//returns the number of assets still palying after this one is removed
extern u8 endAsset(u8);

//will temporarilly stop playing a song in the queue. It will still maintain it's position in the queue.
extern void pauseAsset(u8);

//will resume a paused asset.
extern void resumeAsset(u8);