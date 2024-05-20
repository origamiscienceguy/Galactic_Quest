#include "audio_engine_settings.h"

//processAudio must be ran once per frame, and must finish before the timer 1 interrupt fires.
extern void processAudio();

//will enable a vcount interrupt just once. Two frames after this function is invoked, it is safe to use Vcount interrupts again.
//Timer 0, Timer 1, Timer 2, DMA 0, DMA 1, and all the sound registers must not be touched from now until the engine is disabled.
extern void audioInitialize();

//will add a new song to the queue if it's priority is higher than another songs in the queue.
//Return value is the index into the current song array.
extern u8 playNewSong(u16);