#include "audio_engine_settings.h"
#include "tonc.h"

//type in the priority of this asset. Higher number means higher priority
cu8 _sfxCursorMove_Priority = 0;

//type in the priority of each channel in this asset. Leave unused channels as blank, or set to 0
cu8 _sfxCursorMove_ChannelPriority[MAX_DMA_CHANNELS] = { 200, 200, 0, 0, 0, 0, 0, 0, 0, 0 };