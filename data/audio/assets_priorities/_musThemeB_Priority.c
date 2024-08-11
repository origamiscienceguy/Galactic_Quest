#include "audio_engine_settings.h"
#include "tonc.h"

//type in the priority of this asset. Higher number means higher priority
cu8 _musThemeB_Priority = 0;

//type in the priority of each channel in this asset. Leave unused channels as blank, or set to 0
cu8 _musThemeB_ChannelPriority[MAX_DMA_CHANNELS] = { 
	100,		// Channel 1
	99,			// Channel 2
	98,			// Channel 3
	97,			// Channel 4
	96,			// Channel 5
	95,			// Channel 6
	94,			// Channel 7
	93,			// Channel 8
	51,			// Channel 9
	50			// Channel 10
 };