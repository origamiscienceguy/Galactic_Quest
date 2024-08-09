#include "audio_engine_settings.h"
#include "tonc.h"

//type in the priority of this asset. Higher number means higher priority
cu8 _sfxShipExplodeL_Priority = 0;

//type in the priority of each channel in this asset. Leave unused channels as blank, or set to 0
cu8 _sfxShipExplodeL_ChannelPriority[MAX_DMA_CHANNELS] = { 
	80,		// Channel 1
	80,		// Channel 2
	0,		// Channel 3
	0,		// Channel 4
	0,		// Channel 5
	0,		// Channel 6
	0,		// Channel 7
	0,		// Channel 8
	0,		// Channel 9
	0		// Channel 10
 };