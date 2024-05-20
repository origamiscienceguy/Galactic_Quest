#include "audio_engine_internal.h"

//global variables
s8 soundBuffer1[MAX_SAMPLES_IN_ONE_FRAME * 2] ALIGN(4) EWRAM_DATA;
s8 soundBuffer2[MAX_SAMPLES_IN_ONE_FRAME * 2] ALIGN(4) EWRAM_DATA;
s8 audioError;
u8 audioTimer;
ChannelData channelsMixData[MAX_DMA_CHANNELS] EWRAM_DATA;
CurrentSongSettings currentSongs[MAX_SONGS_IN_QUEUE] EWRAM_DATA;
u8 audioProgress;
u8 graphXPos = 0;


void audioInitialize(){
	//reset the timer registers
	REG_TM0CNT = 0;
	REG_TM0D = 0x10000 - CYCLES_PER_SAMPLE;
	REG_TM1CNT = 0;
	REG_TM1D = 0x10000 - MAX_SAMPLES_IN_ONE_FRAME + 16;
	REG_TM1CNT = TM_CASCADE | TM_IRQ | TM_ENABLE;

	//reset some of the sound registers
	REG_SNDSTAT = SSTAT_ENABLE;
	REG_SNDDSCNT = SDS_DMG100 | SDS_A100 | SDS_B100 | SDS_AR | SDS_ATMR0 | SDS_ARESET | SDS_BL | SDS_BTMR0 | SDS_BRESET | SDS_DMG100;
	REG_SNDDMGCNT = SDMG_LSQR1 | SDMG_LSQR2 | SDMG_LWAVE | SDMG_LNOISE | SDMG_RSQR1 | SDMG_RSQR2 | SDMG_RWAVE | SDMG_RNOISE | SDMG_LVOL(7) | SDMG_RVOL(7);
	REG_SNDBIAS = 0x200;
	
	//fill the sound FIFO with 0
	REG_FIFO_A = 0;
	REG_FIFO_A = 0;
	REG_FIFO_A = 0;
	REG_FIFO_A = 0;
	REG_FIFO_B = 0;
	REG_FIFO_B = 0;
	REG_FIFO_B = 0;
	REG_FIFO_B = 0;
	
	//reset the DMA registers
	REG_DMA1CNT = 0;
	REG_DMA1SAD = (u32)soundBuffer1;
	REG_DMA1DAD = (u32)&REG_FIFO_A;
	REG_DMA1CNT = DMA_COUNT(4) | DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;
	REG_DMA2CNT = 0;
	REG_DMA2DAD = (u32)&REG_FIFO_B;
	REG_DMA2SAD = (u32)soundBuffer1 + (MAX_SAMPLES_IN_ONE_FRAME >> 2);
	REG_DMA2CNT = DMA_COUNT(4) | DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;
	
	//set the correct global variables
	audioError = SYNC_PERIOD_FRAMES - 1; //127
	audioTimer = 0;
	
	//clear soundBuffer1 and 2, both L and R;
	for(u32 i = 0; i < (MAX_SAMPLES_IN_ONE_FRAME >> 1); i++){
		((u32 *)soundBuffer1)[i] = 0;
		((u32 *)soundBuffer2)[i] = 0;
	}
	
	//memorySet(soundBuffer1R, (MAX_SAMPLES_IN_ONE_FRAME >> 4), 0);
	//memorySet(soundBuffer1L, (MAX_SAMPLES_IN_ONE_FRAME >> 4), 0);
	
	//initialize all channels
	for(u32 i = 0; i < MAX_DMA_CHANNELS; i++){
		channelsMixData[i].samplePtr = sampleList[0];
		channelsMixData[i].sampleIndex = 0;
		channelsMixData[i].state = 0;
		channelsMixData[i].leftVolume = 0x0;
		channelsMixData[i].rightVolume = 0x0;
		channelsMixData[i].pitch = 0x0;
	}
	
	//initialize all songs
	for(u32 i = 0; i < MAX_SONGS_IN_QUEUE; i++){
		currentSongs[i].enabled = 0;
	}
	
	//enable vcount interrupts so that the audio iterrupt can be synced to this point
	REG_DISPSTAT = DSTAT_VBL_IRQ | DSTAT_VCT_IRQ | DSTAT_VCT(158);
	
	playNewSong(0);
}

//starts a new song playing, based on the id into the audio_list song list.
u8 playNewSong(u16 songID){
	
	u8 songIndex = 0;
	//find an open songID slot
	while(currentSongs[songIndex].enabled != 0){
		songIndex++;
		//if there are no available song slots, return error
		if(songID == MAX_SONGS_IN_QUEUE){
			return 0xff;
		}
	}
	
	CurrentSongSettings *songPointer = &currentSongs[songIndex];
	
	songPointer->song = songList[songID];
	songPointer->songIndex = songID;
	songPointer->rowNum = 0;
	songPointer->patternOffset = 0;
	songPointer->orderIndex = 0;
	songPointer->enabled = 1;
	songPointer->currentTickSpeed = songList[songID]->initTickSpeed;
	songPointer->tickCounter = 0;
	songPointer->currentTempo = songList[songID]->initTempo;
	songPointer->leftoverSamples = 0;
	songPointer->globalVolume = songList[songID]->initGlobalVol;
	songPointer->globalEffects.A = 0xff;
	songPointer->globalEffects.B = 0xff;
	songPointer->globalEffects.C = 0xff;
	songPointer->globalEffects.SE = 0xff;
	//psgBufferPreviousWriteIndex = 0;
	
	for(u32 channel = 0; channel < MAX_DMA_CHANNELS; channel++){
		songPointer->channelSettings[channel].noteState = NO_NOTE;
		songPointer->channelSettings[channel].effectMemory.SBx = 0xff;
		songPointer->channelSettings[channel].channelVolume = songList[songID]->initChannelVol[channel];
		songPointer->channelSettings[channel].instrumentPointer = &songList[0]->instruments[0]; //pointer to the instrument struct that is currently playing
		songPointer->channelSettings[channel].samplePointer = sampleList[0]; //pointer to the sample that is currently playing
		songPointer->channelSettings[channel].pitchModifier = 0x1000;
		songPointer->channelSettings[channel].channelPan = songList[songID]->initChannelPan[channel];
		songPointer->channelSettings[channel].vibrato.waveformType = 0;
		songPointer->channelSettings[channel].tremolo.waveformType = 0;
		songPointer->channelSettings[channel].panbrello.waveformType = 0;
		songPointer->channelSettings[channel].autoVibrato.waveformType = 0;
	}
	return songIndex;
}

void processAudio(){
	//mark the audio as started
	audioProgress = 1;
	
	//enable interrupts
	REG_IME = 1;
	
	
	REG_TM3D = 0;
	REG_TM3CNT = TM_FREQ_256 | TM_ENABLE;
	
	s8 *audioBufferPtr;
	u32 samplesNeeded;
	
	//start recording for the performance graph
	u32 timerLength;
	vu16 *VramPtr = ((vu16 *)0x06000000) + (240 * 159) + graphXPos;
	if(graphXPos < 239){
		graphXPos++;
	}
	else{
		graphXPos = 0;
	}
	
	//set the audio pointers based on the parity of the current frame
	if (audioTimer & 1) {
	    audioBufferPtr = soundBuffer1;
	}
	else{
	    audioBufferPtr = soundBuffer2;
	}
	
	//figure out how many samples we need this frame
	if(audioError < 0){
		samplesNeeded = MAX_SAMPLES_IN_ONE_FRAME;
	}
	else{
		samplesNeeded = MAX_SAMPLES_IN_ONE_FRAME - 16;
	}
	
	u32 areSongsPlaying = 0;
	//check if there is at least one song playing right now
	for(u32 songIndex = 0; songIndex < MAX_SONGS_IN_QUEUE; songIndex++){
		if(currentSongs[songIndex].enabled == 1){
			areSongsPlaying = 1;
		}
	}
	
	//if at least one song is playing
	if(areSongsPlaying){
		//repeat until all samples needed this frame are processed
		while(samplesNeeded != 0){
			//check which channel has the fewest leftoverSamples
			u32 samplesThisBatch = 0x10000;
			for(u32 songIndex = 0; songIndex < MAX_SONGS_IN_QUEUE; songIndex++){
				//if this song has no leftover samples, process a tick
				if(currentSongs[songIndex].leftoverSamples == 0){
					processSongTick(&currentSongs[songIndex]);
					currentSongs[songIndex].leftoverSamples = tempoTable[currentSongs[songIndex].currentTempo - 32];
				}
				if(currentSongs[songIndex].leftoverSamples < samplesThisBatch){
					samplesThisBatch = currentSongs[songIndex].leftoverSamples;
				}
			}
			//check if the value is bigger than the maximum batch size
			if(samplesThisBatch > MAX_SAMPLES_AT_ONCE){
				samplesThisBatch = MAX_SAMPLES_AT_ONCE;
			}
			//check if the value will finish this frame
			if(samplesThisBatch > samplesNeeded){
				samplesThisBatch = samplesNeeded;
			}
			//mix this many samples using the current settings
			mixAudio(channelsMixData, audioBufferPtr, samplesThisBatch, MAX_DMA_CHANNELS);
			
			samplesNeeded -= samplesThisBatch;
			//update the leftover samples of every channel
			for(u32 songIndex = 0; songIndex < MAX_SONGS_IN_QUEUE; songIndex++){
				currentSongs[songIndex].leftoverSamples -= samplesThisBatch;
			}
		}
	}
	
	//draw the performance graph
	REG_TM3CNT = 0;
	timerLength = REG_TM3D;
	timerLength = (timerLength * 160 * 60) >> 16;
	
	
	for(u32 i = 0; i < 160; i++){
		VramPtr[-i * 240] = 0x0;	//clear the column
	}
	for(u32 i = 0; i < timerLength; i++){
		VramPtr[-i * 240] = 0x7fff; //plot white bars 
	}
	
	//disable interrupts
	REG_IME = 0;
	
	//mark the audio as finished
	audioProgress = 2;
}

void processSongTick(CurrentSongSettings *songPointer){	
	//if the song just started
	if(songPointer->rowNum == 0){
		nextRow(&songPointer->song->patterns[songPointer->song->orders[0]], &songPointer->patternOffset, songPointer);
		songPointer->rowNum = 1;
	}
	//if there is a delay
	else if(songPointer->delayTicks > 0){
		songPointer->delayTicks--;
	}
	//if we are moving on to a new row
	else if(songPointer->tickCounter == (songPointer->currentTickSpeed - 1)){
		PatternData *patternPtr;
		u16 numRows;
		
		//reset the tick counter
		songPointer->tickCounter = 0;
		
		//store the last used settings
		for(u8 channel = 0; channel < MAX_DMA_CHANNELS; channel++){
			CurrentChannelSettings *channelPointer = &songPointer->channelSettings[channel];
			channelPointer->previousBasicSettings.volume = channelPointer->currentBasicSettings.volume;
			channelPointer->previousBasicSettings.note = channelPointer->currentBasicSettings.note;
			channelPointer->previousBasicSettings.instrument = channelPointer->currentBasicSettings.instrument;
			
			if(channelPointer->vibrato.state == PLAY_VIBRATO){
				channelPointer->vibrato.state = PROCESSED_VIBRATO;
			}
			if(channelPointer->tremolo.state == PLAY_VIBRATO){
				channelPointer->tremolo.state = PROCESSED_VIBRATO;
			}
			if(channelPointer->panbrello.state == PLAY_VIBRATO){
				channelPointer->panbrello.state = PROCESSED_VIBRATO;
			}
			if(channelPointer->autoVibrato.state == PLAY_VIBRATO){
				channelPointer->autoVibrato.state = PROCESSED_VIBRATO;
			}
			
			if(channelPointer->periodicEffectState == ARPEGGIO_1){
				channelPointer->currentPitch = (channelPointer->currentPitch * arpeggioTable[15 - ((channelPointer->currentBasicSettings.effectValue & 0xf0) >> 4)]) >> 12;
			}
			else if(channelPointer->periodicEffectState == ARPEGGIO_2){
				channelPointer->currentPitch = (channelPointer->currentPitch * arpeggioTable[15 - (channelPointer->currentBasicSettings.effectValue & 0xf)]) >> 12;
			}
			channelPointer->periodicEffectState = NO_PERIODIC_EFFECT;
		}
		
		//load the pattern data we are currently working through
		patternPtr = &songPointer->song->patterns[songPointer->song->orders[songPointer->orderIndex]];
		numRows = patternPtr->rowsNum;
		
		//check if there is a position jump command
		if((songPointer->globalEffects.B != 0xff) || (songPointer->globalEffects.C != 0xff)){
			positionJump(songPointer);
			patternPtr = &songPointer->song->patterns[songPointer->song->orders[songPointer->orderIndex]];
			songPointer->patternOffset = 0;
			for(u32 i = 1; i < songPointer->rowNum; i++){
				nextRow(patternPtr, &songPointer->patternOffset, songPointer);
			}
		}
		
		//check if we have reached the last row number of this pattern
		else if(songPointer->rowNum == numRows){
			//increment the order ID
			songPointer->orderIndex++;
			//check if the song is over
			if((songPointer->orderIndex >= (songPointer->song->ordersNum)) || (songPointer->song->orders[songPointer->orderIndex] == 0xff)){
				//play the next song
				u16 songIndex = songPointer->songIndex;
				if(songIndex == (numSongs - 1)){
					playNewSong(0);
				}
				else{
					playNewSong(songIndex + 1);
				}
				return;
			}
			songPointer->rowNum = 1;
			songPointer->patternOffset = 0;
			//load the pattern data for the next pattern
			patternPtr = &songPointer->song->patterns[songPointer->song->orders[songPointer->orderIndex]];
		}
		//if we have not yet reached the last row of this pattern
		else{
			//increment the row Number
			songPointer->rowNum++;
		}
		//reset the global settings
		songPointer->globalEffects.SE = 0xff;
		songPointer->globalEffects.A = 0xff;
		//get the data for this row from the pattern table
		nextRow(patternPtr, &songPointer->patternOffset, songPointer);
		
	}
	//if we are not yet moving on to a new row
	else{
		//decrement the tick counter
		songPointer->tickCounter++;
	}
	
	for(u8 channel = 0; channel < MAX_DMA_CHANNELS; channel++){
		CurrentChannelSettings *channelPointer = &songPointer->channelSettings[channel];
		//process any effects
		processEffects(channelPointer, songPointer);
	}
	
	//now we go through each of the 8 sampled channels, and process all of their data.
	for(u8 channel = 0; channel < MAX_DMA_CHANNELS; channel++){
		processSampledChannel(&songPointer->channelSettings[channel], &channelsMixData[channel], songPointer);
	}
}

void processSampledChannel(CurrentChannelSettings *channelPointer, ChannelData *channelMixBuffer, CurrentSongSettings *songPointer){
	//setup some local variables, and set them to default values
	u8 finalVolume = 128;
	u16 finalPitch = 0;
	s8 finalPanning = 0;
	u8 envalopeVolume = 64;
	s8 envalopePitch = 0;
	s8 envalopePanning = 0;
	s8 vibrato = 0;
	s8 tremolo = 0;
	s8 panbrello = 0;
	s8 autoVibrato = 0;
	u16 offset = 0;
	
	//process any volume commands
	processVolume(channelPointer);
	
	//process any new note data in the pattern
	if((channelPointer->pitchState == NEW_PITCH) && (channelPointer->triggerState != DELAY_TRIGGER)){
		processNote(channelPointer, songPointer);
	}
	
	if(channelPointer->triggerState != NO_TRIGGER){
		processTrigger(channelPointer, songPointer);
	}

	//if the note is currently fading out
	if(channelPointer->noteState == FADEOUT_NOTE){
		//if the fade component reaches zero
		if(channelPointer->noteFadeComponent <= (channelPointer->instrumentPointer->fadeOut)){
			channelPointer->noteState = NO_NOTE;
		}
		//otherwise, reduce the fade component
		else{
			channelPointer->noteFadeComponent -= (channelPointer->instrumentPointer->fadeOut);
		}
	}
	
	if(channelPointer->noteState != NO_NOTE){
		//process the three envalopes
		if((channelPointer->volumeEnvalope.enabled & 2) || ((channelPointer->volumeEnvalope.enabled & 1) && !(channelPointer->volumeEnvalope.enabled & 4))){
			envalopeVolume = processEnvalope(&channelPointer->volumeEnvalope, &channelPointer->instrumentPointer->volEnvalope, channelPointer->noteState);
			//if the end of the volume envalope is reached, switch to fadeout
			if((channelPointer->instrumentPointer->volEnvalope.nodeCount - 1) == (channelPointer->volumeEnvalope.currentNodeIndex)){
				channelPointer->noteState = FADEOUT_NOTE;
			}	
		}
		else if(channelPointer->noteState == PLAY_NOTE){
			channelPointer->noteState = FADEOUT_NOTE;
		}
		if((channelPointer->pitchEnvalope.enabled & 2) || ((channelPointer->pitchEnvalope.enabled & 1) && !(channelPointer->pitchEnvalope.enabled & 4))){
			envalopePitch = processEnvalope(&channelPointer->pitchEnvalope, &channelPointer->instrumentPointer->pitchEnvalope, channelPointer->noteState);
			channelPointer->pitchModifier = (channelPointer->pitchModifier * portamentoTable[256 + (envalopePitch << 3)]) >> 12;
		}
		if((channelPointer->panningEnvalope.enabled & 2) || ((channelPointer->panningEnvalope.enabled & 1) && !(channelPointer->panningEnvalope.enabled & 4))){
			envalopePanning = processEnvalope(&channelPointer->panningEnvalope, &channelPointer->instrumentPointer->panEnvalope, channelPointer->noteState);
		}
	}
	
	//process the 4 vibratos
	vibrato = processVibrato(&channelPointer->vibrato);
	tremolo = processVibrato(&channelPointer->tremolo);
	panbrello = processVibrato(&channelPointer->panbrello);
	autoVibrato = processVibrato(&channelPointer->autoVibrato);
	
	//calculate the final volume
	finalVolume = (((envalopeVolume * channelPointer->noteFadeComponent * channelPointer->currentVolume * songPointer->globalVolume >> 6) *
				channelPointer->channelVolume >> 6) * channelPointer->samplePointer->globalVolume >> 6) * channelPointer->instrumentPointer->globalVolume >> 23;
	//if the tremor has turned off the note right now
	if(finalVolume + tremolo > 0x80){
		finalVolume = 0x80;
	}
	else if(finalVolume + tremolo < 0){
		finalVolume = 0;
	}
	else{
		finalVolume += tremolo;
	}
	if(channelPointer->periodicEffectState == TREMOR_OFF){
		finalVolume = 0;
	}
	
	//calculate the final pitch
	finalPitch = (channelPointer->currentPitch * channelPointer->pitchModifier) >> 12;
	channelPointer->currentPitch = finalPitch;
	finalPitch = (finalPitch * vibratoTable[128 + vibrato]) >> 12;
	finalPitch = (finalPitch * vibratoTable[128 + autoVibrato]) >> 12;
	channelPointer->pitchModifier = 0x1000;
	
	//calculate the final pan
	//if we are using envalope pan
	if(channelPointer->currentPanning & 0x80){
		finalPanning = envalopePanning;
		channelPointer->currentPanning = envalopePanning + 32;
	}
	else{
		finalPanning = channelPointer->currentPanning - 32;
	}
	if(finalPanning + panbrello < -32){
		finalPanning = -32;
	}
	else if(finalPanning + panbrello > 32){
		finalPanning = 32;
	}
	else{
		finalPanning += panbrello;
	}
	
	//if there is an active sample offset
	if(channelPointer->offset & 0x8000){
		offset = channelPointer->offset & 0xfff;
		channelPointer->offset &= 0xf00;
	}
	
	applySettings(channelMixBuffer, channelPointer->samplePointer, finalPitch, finalVolume, finalPanning, &channelPointer->noteState, offset);
}

//volume from 0 to 128
//panning from -32 to 32

//take the settings dictated by the IT song, and translate them into settings for the mixer
void applySettings(ChannelData *channelMixData, AudioSample *samplePtr, u32 pitch, u8 volume, s8 panning, enum NoteState *state, u16 offset){
	s32 temp;
	//set the new pitch
	channelMixData->pitch = pitch;
	
	//set the right volume
	temp = -((volume * panning) >> 5) + volume;
	if(temp == 256){
		temp = 255;
	}
	channelMixData->rightVolume = (u8)temp;
	
	//set the left volume
	temp = ((volume * panning) >> 5) + volume;
	if(temp == 256){
		temp = 255;
	}
	channelMixData->leftVolume = (u8)temp;
	
	u8 flags = samplePtr->sampleType;
	//set the correct state of the sample according to the state of the channel
	switch(*state){
	case NO_NOTE:
		channelMixData->state = 0;
		break;
	case TRIGGER_TICK_NOTE:
		//set the starting index
		channelMixData->sampleIndex = offset << 8;
		//set the sample pointer
		channelMixData->samplePtr = samplePtr;
		
		//set the correct starting state
		//if the sample has a sustain loop
		if(flags & 0x2){
			//if the sustain loop is a ping pong loop
			if(flags & 0x8){
				channelMixData->state = 8;
			}
			//if the sustain loop is an overflow loop
			else{
				channelMixData->state = 7;
			}
		}
		//if the sample does not have a sustain loop, but does have a normal loop
		else if(flags & 0x1){
			//if the normal loop is a ping pong loop
			if(flags & 0x4){
				channelMixData->state = 4;
			}
			//if the normal loop is an overflow loop
			else{
				channelMixData->state = 3;
			}
		}
		//if the sample does not have any loops
		else{
			channelMixData->state = 1;
		}
		*state = SUSTAIN_NOTE;
		break;
	case SUSTAIN_NOTE:
		//no changes to state necessary
		break;
	case PLAY_NOTE:
		//same state management as FADEOUT
	case FADEOUT_NOTE:
		//if the sample is sustain-looping
		if(channelMixData->state >= 7){
			//if the sustain loop is currently moving right
			if(channelMixData->state != 9){
				//if the sample has a loop
				if(flags & 0x1){
					//if the loop is a ping-pong loop
					if(flags & 0x4){
						channelMixData->state = 4;
					}
					//if the loop is an overflow loop
					else{
						channelMixData->state = 3;
					}
				}
				//if the sample has no loop
				else{
					channelMixData->state = 1;
				}
			}
			//if the sustain loop is a ping-pong loop, currently moving left
			else{
				//if the sample has a loop
				if(flags & 0x1){
					//if the loop is a ping-pong loop
					if(flags & 0x4){
						channelMixData->state = 5;
					}
					//if the loop is an overflow loop
					else{
						channelMixData->state = 6;
					}
				}
				//if the sample has no loop
				else{
					channelMixData->state = 2;
				}
			}
		}
		break;
	}
}

void nextRow(PatternData *patternPtr, u16 *patternOffsetPtr, CurrentSongSettings *songPointer){
	u8 channelMask;
	ChannelBasicSettings *previousSettings;
	ChannelBasicSettings *currentSettings;
	u8 isChannelModified[MAX_DMA_CHANNELS] = {0};
	u16 patternOffset = *patternOffsetPtr;

	//load the channel mask
	channelMask = patternPtr->packedPatternData[patternOffset];
	patternOffset++;
	
	//complete this loop until the channelMask becomes 0
	while(channelMask != 0){
		u8 channel;
		u8 maskVariable;
		
		//determine the channel being modified
		channel = (channelMask & 0x3f) - 1;
		previousSettings = &songPointer->channelSettings[channel].previousBasicSettings;
		currentSettings = &songPointer->channelSettings[channel].currentBasicSettings;
		isChannelModified[channel] = 1;
		
		//load a new maskVariable if channelMask says to
		if(channelMask & 0x80){
			maskVariable = patternPtr->packedPatternData[patternOffset];
			patternOffset++;
			songPointer->channelSettings[channel].maskVariable = maskVariable;
		}
		//otherwise reuse the last maskVariable
		else{
			maskVariable = songPointer->channelSettings[channel].maskVariable;
		}
		
		//check if a new note needs to be loaded
		if(maskVariable & 0x1){
			//load the new note
			currentSettings->note = patternPtr->packedPatternData[patternOffset];
			patternOffset++;
			songPointer->channelSettings[channel].pitchState = NEW_PITCH;
		}
		//if the last note gets reused
		else if(maskVariable & 0x10){
			//load the last note used
			currentSettings->note = previousSettings->note;
			songPointer->channelSettings[channel].pitchState = NEW_PITCH;
		}
		
		//check if a new instrument needs to be loaded
		if(maskVariable & 0x2){
			currentSettings->instrument = patternPtr->packedPatternData[patternOffset];
			patternOffset++;
			songPointer->channelSettings[channel].triggerState = TRIGGER;
		}
		//if the last instrument gets reused
		else if(maskVariable & 0x20){
			currentSettings->instrument = previousSettings->instrument;
			songPointer->channelSettings[channel].triggerState = TRIGGER;
		}
		
		//check if a new volume needs to be loaded
		if(maskVariable & 0x4){
			currentSettings->volume = patternPtr->packedPatternData[patternOffset];
			patternOffset++;
			songPointer->channelSettings[channel].volumeState = TRIGGER_TICK_VOLUME;
		}
		//if the last volume gets reused
		else if(maskVariable & 0x40){
			currentSettings->volume = previousSettings->volume;
			songPointer->channelSettings[channel].volumeState = TRIGGER_TICK_VOLUME;
		}
		else{
			songPointer->channelSettings[channel].volumeState = NO_VOLUME;
		}
		
		//check if a new effect needs to be loaded
		if(maskVariable & 0x8){
			currentSettings->effect = patternPtr->packedPatternData[patternOffset];
			patternOffset++;
			currentSettings->effectValue = patternPtr->packedPatternData[patternOffset];
			patternOffset++;
			previousSettings->effectValue = currentSettings->effectValue;
			previousSettings->effect = currentSettings->effect;
			songPointer->channelSettings[channel].effectState = TRIGGER_TICK_EFFECT;
		}
		//if the last effect gets reused
		else if(maskVariable & 0x80){
			currentSettings->effect = previousSettings->effect;
			currentSettings->effectValue = previousSettings->effectValue;
			songPointer->channelSettings[channel].effectState = TRIGGER_TICK_EFFECT;
		}
		else{
			songPointer->channelSettings[channel].effectState = NO_EFFECT;
		}
		
		//load the next channelmask
		channelMask = patternPtr->packedPatternData[patternOffset];
		patternOffset++;
	}
	
	//handle all the unmodified channels
	for(u32 channel = 0; channel < MAX_DMA_CHANNELS; channel++){
		if(isChannelModified[channel]){
			continue;
		}
		else{
			songPointer->channelSettings[channel].effectState = NO_EFFECT;
			songPointer->channelSettings[channel].volumeState = NO_VOLUME;
		}
	}
	*patternOffsetPtr = patternOffset;
}

void processEffects(CurrentChannelSettings *channelPointer, CurrentSongSettings *songPointer){
	u8 command = channelPointer->currentBasicSettings.effect;
	u8 commandAmount = channelPointer->currentBasicSettings.effectValue;
	//if there was no effect command to process
	if(channelPointer->effectState == NO_EFFECT){
		return;
	}
	//if there is a new effect command this tick
	else if(channelPointer->effectState == TRIGGER_TICK_EFFECT){
		switch(command){
		//if it is an Axx "set speed" command
		case 1:
			if((commandAmount != 0) && (songPointer->globalEffects.A == 0xff)){
				songPointer->currentTickSpeed = commandAmount;
			}
			channelPointer->effectState = NO_EFFECT;
			return;
		//if it is a Bxx "position jump" command
		case 2:
			if(songPointer->globalEffects.B == 0xff){
				songPointer->globalEffects.B = commandAmount;
			}
			channelPointer->effectState = NO_EFFECT;
			return;
		//if it is a Cxx "pattern break" command
		case 3:
			if(songPointer->globalEffects.C == 0xff){
				songPointer->globalEffects.C = commandAmount;
			}
			channelPointer->effectState = NO_EFFECT;
			return;
		//if it is a Dxx "volume slide" command
		case 4:
			//if it is a "fine volume slide" command
			if(slideCheck(&channelPointer->effectMemory.D, commandAmount)){
				volumeSlide(&channelPointer->currentVolume, &channelPointer->effectMemory.D, 64, commandAmount);
				channelPointer->effectState = NO_EFFECT;
			}
			//if it is a regular "volume slide" command
			else{
				channelPointer->effectState = EVERY_TICK_EFFECT;
			}
			return;
		//if it is a Exx "portamento down" command
		case 5:
			if(commandAmount == 0){
				commandAmount = channelPointer->effectMemory.EeFf;
			}
			else{
				channelPointer->effectMemory.EeFf = commandAmount;
			}
			//if it is a "fine portamento down" or "extra fine portamento down" command
			if(portamentoCheck(commandAmount)){
				portamentoDown(channelPointer, commandAmount);
				channelPointer->effectState = NO_EFFECT;
			}
			//if it is a regular "portamento down" command
			else{
				channelPointer->effectState = EVERY_TICK_EFFECT;
			}
			return;
		//if it is a Fxx "portamento up" command
		case 6:
			if(commandAmount == 0){
				commandAmount = channelPointer->effectMemory.EeFf;
			}
			else{
				channelPointer->effectMemory.EeFf = commandAmount;
			}
			//if it is a "fine portamento up" or "extra fine portamento up" command
			if(portamentoCheck(commandAmount)){
				portamentoUp(channelPointer, commandAmount);
				channelPointer->effectState = NO_EFFECT;
			}
			//if it is a regular "portamento down" command
			else{
				channelPointer->effectState = EVERY_TICK_EFFECT;
			}
			return;
		//if it is a Gxx "tone portamento" command
		case 7:
			channelPointer->triggerState = CANCEL_TRIGGER;
			channelPointer->effectState = EVERY_TICK_EFFECT;
			return;
		//if it is a Hxx "vibrato" command
		case 8:
			initVibrato(&channelPointer->vibrato, &channelPointer->effectMemory.HUh, commandAmount, 0, 0);
			channelPointer->effectState = NO_EFFECT;
			return;
		//if it is a Ixx "tremor" command
		case 9:
			if(commandAmount == 0){
				commandAmount = channelPointer->effectMemory.I;
			}
			else{
				channelPointer->effectMemory.I = commandAmount;
			}
			channelPointer->effectState = EVERY_TICK_EFFECT;
			channelPointer->periodicEffectState = TREMOR_ON;
			channelPointer->periodicEffectTimer = ((commandAmount & 0xf0) >> 4) - 1;
			return;
		//if it is a Jxx "arpeggio" command
		case 10:
			if(commandAmount == 0){
				commandAmount = channelPointer->effectMemory.J;
			}
			else{
				channelPointer->effectMemory.J = commandAmount;
			}
			channelPointer->periodicEffectState = ARPEGGIO_0;
			channelPointer->effectState = EVERY_TICK_EFFECT;
			return;
		//if it is a Kxx "volume slide + vibrato" command
		case 11:
			//if it is a "fine volume slide" command
			if(slideCheck(&channelPointer->effectMemory.D, commandAmount)){
				volumeSlide(&channelPointer->currentVolume, &channelPointer->effectMemory.D, 64, commandAmount);
				channelPointer->effectState = NO_EFFECT;
			}
			//if it is a regular "volume slide" command
			else{
				channelPointer->effectState = EVERY_TICK_EFFECT;
			}
			initVibrato(&channelPointer->vibrato, &channelPointer->effectMemory.HUh, 0, 0, 0);
			return;
		//if it is a Lxx "volume slide + tone portamento" command
		case 12:
			//if it is a "fine volume slide" command
			if(slideCheck(&channelPointer->effectMemory.D, commandAmount)){
				volumeSlide(&channelPointer->currentVolume, &channelPointer->effectMemory.D, 64, commandAmount);
			}
			channelPointer->effectState = EVERY_TICK_EFFECT;
			return;
		//if it is a Mxx "set channel volume" command
		case 13:
			//bounds check
			if(commandAmount > 0x40){
				commandAmount = 0x40;
			}
			channelPointer->channelVolume = commandAmount;
			channelPointer->effectState = NO_EFFECT;
			return;
		//if it is a Nxx "channel volume slide" command
		case 14:
			//if it is a "fine channel volume slide" command
			if(slideCheck(&channelPointer->effectMemory.N, commandAmount)){
				volumeSlide(&channelPointer->channelVolume, &channelPointer->effectMemory.N, 64, commandAmount);
				channelPointer->effectState = NO_EFFECT;
			}
			//if it is a regular "channel volume slide" command
			else{
				channelPointer->effectState = EVERY_TICK_EFFECT;
			}
			return;
		//if it is a Oxx "sample offset" command
		case 15:
			channelPointer->offset |= (0x8000 | commandAmount);
			channelPointer->effectState = NO_EFFECT;
			return;
		//if it is a Pxx "panning slide" command
		case 16:
			//if it is a "fine panning slide" command
			if(slideCheck(&channelPointer->effectMemory.P, commandAmount)){
				panningSlide(&channelPointer->currentPanning, &channelPointer->effectMemory.P, commandAmount);
				channelPointer->effectState = NO_EFFECT;
			}
			//if it is a regular "panning slide" command
			else{
				channelPointer->effectState = EVERY_TICK_EFFECT;
			}
			return;
		//if it is a Qxx "retrigger" command
		case 17:
			if(commandAmount == 0){
				commandAmount = channelPointer->effectMemory.Q;
			}
			else{
				channelPointer->effectMemory.Q = commandAmount;
			}
			channelPointer->periodicEffectTimer = (commandAmount & 0xf) - 1;
			channelPointer->effectState = EVERY_TICK_EFFECT;
			return;
		//if it is a Rxx "Tremolo" command
		case 18:
			initVibrato(&channelPointer->tremolo, &channelPointer->effectMemory.R, commandAmount, 0, 0);
			channelPointer->effectState = NO_EFFECT;
			return;
		//if it is one of the many Sxx commands
		case 19:
			command = (commandAmount & 0xf0) >> 4;
			commandAmount = commandAmount & 0xf;
			switch(command){
			//if it is a S1x "glissando control" command
			case 1:
				
				return;
			//if it is a S3x "set vibrato waveform" command
			case 3:
				channelPointer->vibrato.waveformType = commandAmount;
				channelPointer->effectState = NO_EFFECT;
				return;
			//if it is a S4x "set tremolo waveform" command
			case 4:
				channelPointer->tremolo.waveformType = commandAmount;
				channelPointer->effectState = NO_EFFECT;
				return;
			//if it is a S5x "set panbrello waveform" command
			case 5:
				channelPointer->panbrello.waveformType = commandAmount;
				channelPointer->effectState = NO_EFFECT;
				return;
			//if it is a S6x "fine pattern delay" command
			case 6:
				songPointer->delayTicks += commandAmount & 0xf;
				channelPointer->effectState = NO_EFFECT;
				return;
			//if it is one of the many S7x commands
			case 7:
				command = commandAmount;
				switch(command){
				//if it is a S77 "volume envelope off" command
				case 7:
					channelPointer->volumeEnvalope.enabled = (channelPointer->volumeEnvalope.enabled & 1) | 0x84;
					channelPointer->effectState = EVERY_TICK_EFFECT;
					return;
				//if it is a S78 "volume envelope on" command
				case 8:
					channelPointer->volumeEnvalope.enabled = (channelPointer->volumeEnvalope.enabled & 1) | 0x82;
					channelPointer->effectState = EVERY_TICK_EFFECT;
					return;
				//if it is a S79 "panning envelope off" command
				case 9:
					channelPointer->panningEnvalope.enabled = (channelPointer->panningEnvalope.enabled & 1) | 0x84;
					//mark the panning envelope as not in use
					channelPointer->currentPanning &= 0x7f;
					channelPointer->effectState = EVERY_TICK_EFFECT;
					return;
				//if it is a S7A "panning envelope on" command
				case 10:
					channelPointer->panningEnvalope.enabled = (channelPointer->panningEnvalope.enabled & 1) | 0x82;
					channelPointer->effectState = EVERY_TICK_EFFECT;
					return;
				//if it is a S7B "pitch envelope off" command
				case 11:
					channelPointer->pitchEnvalope.enabled = (channelPointer->pitchEnvalope.enabled & 1) | 0x84;
					channelPointer->effectState = EVERY_TICK_EFFECT;
					return;
				//if it is a S7C "pitch envelope on" command
				case 12:
					channelPointer->panningEnvalope.enabled = (channelPointer->panningEnvalope.enabled & 1) | 0x82;
					channelPointer->effectState = EVERY_TICK_EFFECT;
					return;
				default:
					channelPointer->effectState = NO_EFFECT;
					return;
				}
				return;
			//if it is a S8x "set panning" command
			case 8:
				if(commandAmount == 0xf){
					commandAmount = 64;
				}
				else{
					commandAmount <<= 2;
				}
				channelPointer->channelPan = commandAmount | 0x80;
				return;
			//if it is a SAx "high offset" command
			case 10:
				channelPointer->offset = commandAmount << 8;
				channelPointer->effectState = NO_EFFECT;
				return;
			//if it is a SBx "pattern loop" command
			case 11:;
				//if we are setting a loop point and are not currently in a loop
				if((commandAmount == 0) && (channelPointer->effectMemory.SBx == 0xff)){
					channelPointer->effectMemory.SB0 = songPointer->rowNum;
				}
				//if we are starting a new loop
				else if((commandAmount != 0) && (channelPointer->effectMemory.SBx == 0xff)){
					channelPointer->effectMemory.SBx = commandAmount;
				}
				//if we are ending a loop
				if((commandAmount != 0) && (channelPointer->effectMemory.SBx == 0)){
					channelPointer->effectMemory.SBx = 0xff;
				}
				//if we are performing one loop
				else if(commandAmount != 0){
					channelPointer->effectMemory.SBx--;
					songPointer->globalEffects.B = songPointer->orderIndex;
					songPointer->globalEffects.C = channelPointer->effectMemory.SB0;
				}
				channelPointer->effectState = NO_EFFECT;
				return;
			//if it is a SCx "note cut" command
			case 12: 
				if(commandAmount == 0){
					commandAmount = 1;
				}
				channelPointer->periodicEffectTimer = commandAmount - 1;
				channelPointer->effectState = EVERY_TICK_EFFECT;
				return;
			//if it is a SDx "note delay" command
			case 13:
				if(commandAmount == 0){
					commandAmount = 1;
				}
				channelPointer->periodicEffectTimer = commandAmount - 1;
				channelPointer->triggerState = DELAY_TRIGGER;
				channelPointer->effectState = EVERY_TICK_EFFECT;
				return;
			//if it is a SEx "pattern delay" command
			case 14:
				if(songPointer->globalEffects.SE == 0xff){
					songPointer->globalEffects.SE = commandAmount;
					songPointer->delayTicks += songPointer->currentTickSpeed * (commandAmount);
					channelPointer->effectState = NO_EFFECT;
				}
				return;
			default:
				channelPointer->effectState = NO_EFFECT;
				return;
			}
			return;
		//if it is a Txx "tempo control" command
		case 20:
			if(commandAmount == 0){
				commandAmount = channelPointer->effectMemory.T;
			}
			else{
				channelPointer->effectMemory.T = commandAmount;
			}
			//if it is a "set tempo command"
			if(commandAmount >= 0x20){
				songPointer->currentTempo = commandAmount;
				channelPointer->effectState = NO_EFFECT;
			}
			//if it is a "increase tempo" or "decrease tempo" command
			else{
				channelPointer->effectState = EVERY_TICK_EFFECT;
			}
			return;
		//if it is a Uxx "fine vibrato" command
		case 21:
			initVibrato(&channelPointer->vibrato, &channelPointer->effectMemory.HUh, commandAmount, 0, 1);
			channelPointer->effectState = NO_EFFECT;
			return;
		//if it is a Vxx "set global volume" command
		case 22:
			//bounds check
			if(commandAmount > 0x80){
				commandAmount = 0x80;
			}
			songPointer->globalVolume = commandAmount;
			channelPointer->effectState = NO_EFFECT;
			return;
		//if it is a Wxx "global volume slide" command
		case 23:
			//if it is a "fine global volume slide" command
			if(slideCheck(&channelPointer->effectMemory.W, commandAmount)){
				volumeSlide(&songPointer->globalVolume, &channelPointer->effectMemory.W, 128, commandAmount);
				channelPointer->effectState = NO_EFFECT;
			}
			//if it is a regular "global volume slide" command
			else{
				channelPointer->effectState = EVERY_TICK_EFFECT;
			}
			return;
		//if it is a Xxx "set panning" command
		case 24:
			if(commandAmount == 0xff){
				commandAmount = 64;
			}
			else{
				commandAmount >>= 2;
			}
			channelPointer->channelPan = commandAmount | 0x80;
			channelPointer->effectState = NO_EFFECT;
			return;
		//if it is a Yxx "panbrello" command
		case 25:
			initVibrato(&channelPointer->panbrello, &channelPointer->effectMemory.Y, commandAmount, 0, 2);
			channelPointer->effectState = NO_EFFECT;
			return;
		default:
			channelPointer->effectState = NO_EFFECT;
			return;
		}
	}
	else if(channelPointer->effectState == EVERY_TICK_EFFECT){
		switch(command){
		//if it is a Dxx "volume slide" command
		case 4:
			volumeSlide(&channelPointer->currentVolume, &channelPointer->effectMemory.D, 64, commandAmount);
			return;
		//if it is a Exx "portamento down" command
		case 5:
			if(commandAmount == 0){
				commandAmount = channelPointer->effectMemory.EeFf;
			}
			portamentoDown(channelPointer, commandAmount);
			return;
		//if it is a Fxx "portamento up" command
		case 6:
			if(commandAmount == 0){
				commandAmount = channelPointer->effectMemory.EeFf;
			}
			portamentoUp(channelPointer, commandAmount);
			return;
		//if it is a Gxx "tone portamento" command
		case 7:
			tonePortamento(channelPointer, commandAmount);
			return;
		//if it is a Ixx "tremor" command
		case 9:
			if(channelPointer->periodicEffectTimer == 0){
				if(channelPointer->periodicEffectState == TREMOR_ON){
					channelPointer->periodicEffectState = TREMOR_OFF;
					channelPointer->periodicEffectTimer = (commandAmount & 0xf) - 1;
				}
				else{
					channelPointer->periodicEffectState = TREMOR_ON;
					channelPointer->periodicEffectTimer = ((commandAmount & 0xf0) >> 4) - 1;
				}
			}
			else{
				channelPointer->periodicEffectTimer--;
			}
			return;
		//if it is a Jxx "arpeggio" command
		case 10:
			arpeggio(channelPointer, commandAmount);
			
			return;
		//if it is a Kxx "volume slide + vibrato" command
		case 11:
			volumeSlide(&channelPointer->currentVolume, &channelPointer->effectMemory.D, 64, commandAmount);
			return;
		//if it is a Lxx "volume slide + tone portamento" command
		case 12:
			if(slideCheck(&channelPointer->effectMemory.D, commandAmount) == 0){
				volumeSlide(&channelPointer->currentVolume, &channelPointer->effectMemory.D, 64, commandAmount);
			}
			commandAmount = channelPointer->effectMemory.Gg;
			tonePortamento(channelPointer, commandAmount);
			return;
		//if it is a Nxx "channel volume slide" command
		case 14:
			volumeSlide(&channelPointer->channelVolume, &channelPointer->effectMemory.N, 64, commandAmount);
			return;
		//if it is a Pxx "panning slide" command
		case 16:
			panningSlide(&channelPointer->currentPanning, &channelPointer->effectMemory.P, commandAmount);
			return;
		//if it is a Qxx "retrigger" command
		case 17:
			if(channelPointer->periodicEffectTimer == 0){
				retrigger(channelPointer, commandAmount);
			}
			else{
				channelPointer->periodicEffectTimer--;
			}
			return;
		//if it is one of the many Sxx commands
		case 19:
			command = (commandAmount & 0xf0) >> 4;
			switch(command){
			//if it is a SCx "note cut" command
			case 12: 
				if(channelPointer->periodicEffectTimer == 0){
					channelPointer->noteState = NO_NOTE;
					channelPointer->effectState = NO_EFFECT;
				}
				else{
					channelPointer->periodicEffectTimer--;
				}
				return;
			//if it is a SDx "note delay" command
			case 13:
				if(channelPointer->periodicEffectTimer == 0){
					channelPointer->triggerState = NO_TRIGGER;
					channelPointer->effectState = NO_EFFECT;
				}
				else{
					channelPointer->periodicEffectTimer--;
				}
				return;
			default:
				channelPointer->effectState = NO_EFFECT;
				return;
			}
			return;
		//if it is a Txx "tempo control" command
		case 20:
			if(commandAmount == 0){
				commandAmount = channelPointer->effectMemory.T;
			}
			//if it is a "increase tempo" command
			if(commandAmount >= 0x10){
				increaseTempo(commandAmount & 0xf, songPointer);
			}
			else{
				decreaseTempo(commandAmount & 0xf, songPointer);
			}
			return;
		//if it is a Wxx "global volume slide" command
		case 23:
			volumeSlide(&songPointer->globalVolume, &channelPointer->effectMemory.W, 128, commandAmount);
			return;
		default:
			channelPointer->effectState = NO_EFFECT;
			return;
		}
	}
}

void processVolume(CurrentChannelSettings *channelPointer){
	u8 command = channelPointer->currentBasicSettings.volume;
	u8 commandAmount;
	//if there was no volume command to process
	if(channelPointer->volumeState == NO_VOLUME){
		return;
	}
	//if there was a new volume command this tick
	else if(channelPointer->volumeState == TRIGGER_TICK_VOLUME){
		//if it is a "set volume" command
		if(command <= 64){
			channelPointer->currentVolume = command;
			channelPointer->volumeState = PROCESSED_VOLUME;
			return;
		}
		//if it is a "set panning" command
		else if((command <= 192) && (command >= 128)){
			channelPointer->currentPanning = (command - 192) | 0x80;
			channelPointer->volumeState = NO_VOLUME;
			return;
		}
		//if it is a "fine volume up" command
		else if(command <= 74){
			commandAmount = command - 65;
			channelPointer->volumeState = NO_VOLUME;
			volumeSlideUp(channelPointer, commandAmount);
			return;
		}
		//if it is a "fine volume down command"
		else if(command <= 84){
			commandAmount = command - 75;
			channelPointer->volumeState = NO_VOLUME;
			volumeSlideDown(channelPointer, commandAmount);
			return;
		}
		//if it is a "volume slide up" command
		else if(command <= 94){
			channelPointer->volumeState = EVERY_TICK_VOLUME;
			return;
		}
		//if it is a "volume slide down" command
		else if(command <= 104){
			channelPointer->volumeState = EVERY_TICK_VOLUME;
			return;
		}
		//if it is a "portamento down" command
		else if(command <= 114){
			commandAmount = (command - 105) << 2;
			if(commandAmount == 0){
				commandAmount = channelPointer->effectMemory.EeFf;
			}
			else{
				channelPointer->effectMemory.EeFf = commandAmount;
			}
			//if the function returns 1, this effect is done
			if(portamentoCheck(commandAmount)){
				channelPointer->volumeState = NO_VOLUME;
				portamentoDown(channelPointer, commandAmount);
			}
			else{
				channelPointer->volumeState = EVERY_TICK_VOLUME;
			}
			return;
		}
		//if it is a "portamento up" command
		else if(command <= 124){
			commandAmount = (command - 115) << 2;
			if(commandAmount == 0){
				commandAmount = channelPointer->effectMemory.EeFf;
			}
			else{
				channelPointer->effectMemory.EeFf = commandAmount;
			}
			//if the function returns 1, this effect is done
			if(portamentoCheck(commandAmount)){
				channelPointer->volumeState = NO_VOLUME;
				portamentoUp(channelPointer, commandAmount);
			}
			else{
				channelPointer->volumeState = EVERY_TICK_VOLUME;
			}
			return;
		}
		//if it is an unsopported command
		else if(command <= 127){
			channelPointer->volumeState = NO_VOLUME;
			return;
		}
		//if it is a "tone portamento" command
		else if(command <= 202){
			channelPointer->triggerState = CANCEL_TRIGGER;
			channelPointer->volumeState = EVERY_TICK_VOLUME;
			return;
		}
		//if it is a "vibrato" command
		else if(command <= 212){
			channelPointer->volumeState = NO_VOLUME;
			commandAmount = command - 203;
			if(commandAmount != 0){
				commandAmount = (channelPointer->effectMemory.HUh & 0xf0) | commandAmount;
			}
			initVibrato(&channelPointer->vibrato, &channelPointer->effectMemory.HUh, commandAmount, 0, 0);
			return;
		}
		//if it is an unsupported command
		else{
			channelPointer->volumeState = NO_VOLUME;
			return;
		}
	}
	//if an old volume command is still running
	else if(channelPointer->volumeState == EVERY_TICK_VOLUME){
		//if it is a "volume slide up" command
		if(command <= 94){
			commandAmount = command - 85;
			volumeSlideUp(channelPointer, commandAmount);
			return;
		}
		//if it is a "volume slide down" command
		else if(command <= 104){
			commandAmount = command - 95;
			volumeSlideDown(channelPointer, commandAmount);
			return;
		}
		//if it is a "portamento down" command
		else if(command <= 114){
			commandAmount = (command - 105) << 2;
			portamentoDown(channelPointer, commandAmount);
			return;
		}
		//if it is a "portamento up" command
		else if(command <= 124){
			commandAmount = (command - 115) << 2;
			portamentoUp(channelPointer, commandAmount);
			return;
		}
		//if it is a "tone portamento" command
		else{
			commandAmount = tonePortamentoLUT[command - 193];
			tonePortamento(channelPointer, commandAmount);
			return;
		}
	}
}

void processTrigger(CurrentChannelSettings *channelPointer, CurrentSongSettings *songPointer){
	//if a note just got triggered, record the new instrument and sample
	if(channelPointer->triggerState == TRIGGER){
		channelPointer->instrumentPointer = &songPointer->song->instruments[channelPointer->currentBasicSettings.instrument - 1];
		channelPointer->samplePointer = sampleList[songPointer->song->samples[channelPointer->instrumentPointer->keyboardSample[channelPointer->currentBasicSettings.note]]];
		channelPointer->noteFadeComponent = 1024;
		channelPointer->currentPitch = channelPointer->notePitch;
		channelPointer->noteState = TRIGGER_TICK_NOTE;
		channelPointer->triggerState = NO_TRIGGER;
		if(channelPointer->volumeState != PROCESSED_VOLUME){
			channelPointer->currentVolume = channelPointer->samplePointer->defaultVolume;
		}
		else{
			channelPointer->volumeState = NO_VOLUME;
		}
		
		//reset the envelopes
		if(channelPointer->volumeEnvalope.enabled & 0x80){
			channelPointer->volumeEnvalope.enabled = (channelPointer->volumeEnvalope.enabled & 0x7e) | (channelPointer->instrumentPointer->volEnvalope.flags & 1);
		}
		else{
			channelPointer->volumeEnvalope.enabled = channelPointer->instrumentPointer->volEnvalope.flags & 1;
		}
		if(channelPointer->pitchEnvalope.enabled & 0x80){
			channelPointer->pitchEnvalope.enabled = (channelPointer->pitchEnvalope.enabled & 0x7e) | (channelPointer->instrumentPointer->pitchEnvalope.flags & 1);
		}
		else{
			channelPointer->pitchEnvalope.enabled = channelPointer->instrumentPointer->pitchEnvalope.flags & 1;
		}
		if(channelPointer->panningEnvalope.enabled & 0x80){
			channelPointer->panningEnvalope.enabled = (channelPointer->panningEnvalope.enabled & 0x7e) | (channelPointer->instrumentPointer->panEnvalope.flags & 1);
		}
		else{
			channelPointer->panningEnvalope.enabled = channelPointer->instrumentPointer->panEnvalope.flags & 1;
		}
		
		//figure out the panning
		//if there was a pan effect on this row
		if(channelPointer->channelPan & 0x80){
			channelPointer->currentPanning = channelPointer->channelPan & 0x7f;
			channelPointer->channelPan &= 0x7f;
		}
		//if the sample has a default pan
		else if(channelPointer->samplePointer->defaultPan & 0x80){
			channelPointer->currentPanning = channelPointer->samplePointer->defaultPan & 0x7f;
		}
		//if the instrument has a default pan
		else if((channelPointer->instrumentPointer->defaultPan & 0x80) == 0){
			channelPointer->currentPanning = channelPointer->instrumentPointer->defaultPan & 0x7f;
		}
		//if the instrument has an enabled panning envelope. 
		else if((channelPointer->panningEnvalope.enabled & 2) || ((channelPointer->panningEnvalope.enabled & 1) && !(channelPointer->panningEnvalope.enabled & 4))){
			//mark the currentPan as using the panning envelope
			channelPointer->currentPanning = 0x80 | channelPointer->channelPan;
		}
		//if none of the above are true, use the channel pan
		else{
			channelPointer->currentPanning = channelPointer->channelPan;
		}
		
		//handle auto-vibrato
		u8 wave = channelPointer->samplePointer->vibratoWave;
		if(wave == 4){
			channelPointer->autoVibrato.waveformType = 8;
		}
		channelPointer->autoVibrato.state = NO_VIBRATO;
		channelPointer->autoVibrato.sweepProgress = 0;
		
		if((channelPointer->samplePointer->vibratoDepth != 0) && (channelPointer->samplePointer->vibratoSpeed != 0)){
			channelPointer->autoVibrato.state = TRIGGER_VIBRATO;
			channelPointer->autoVibrato.depth = channelPointer->samplePointer->vibratoDepth >> 1;
			channelPointer->autoVibrato.speed = channelPointer->samplePointer->vibratoSpeed;
			channelPointer->autoVibrato.sweep = channelPointer->samplePointer->vibratoSweep;
		}
	}
	else if(channelPointer->triggerState == CANCEL_TRIGGER){
		channelPointer->triggerState = NO_TRIGGER;
	}
}

void processNote(CurrentChannelSettings *channelPointer, CurrentSongSettings *songPointer){
	//if it is a regular note
	if(channelPointer->currentBasicSettings.note <= 119){
		channelPointer->instrumentPointer = &songPointer->song->instruments[channelPointer->currentBasicSettings.instrument - 1];
		channelPointer->samplePointer = sampleList[songPointer->song->samples[channelPointer->instrumentPointer->keyboardSample[channelPointer->currentBasicSettings.note]]];
		channelPointer->notePitch = (channelPointer->samplePointer->middleCPitch * pitchTable[channelPointer->currentBasicSettings.note]) >> 14;
		if(channelPointer->triggerState != CANCEL_TRIGGER){
			channelPointer->triggerState = TRIGGER;
		}
	}
	//if it is a note cut command
	else if(channelPointer->currentBasicSettings.note == 254){
		channelPointer->noteState = NO_NOTE;
	}
	//if it is a note end
	else if(channelPointer->currentBasicSettings.note == 255){
		channelPointer->noteState = PLAY_NOTE;
	}
	//if it is a note fadeout
	else{
		channelPointer->noteState = FADEOUT_NOTE;
	}
	channelPointer->previousBasicSettings.instrument = channelPointer->currentBasicSettings.instrument;
	channelPointer->previousBasicSettings.note = channelPointer->currentBasicSettings.note;
	channelPointer->pitchState = NO_PITCH;
}

s8 processEnvalope(ChannelEnvalopeSettings *envalopeVariables, EnvalopeData *envalopeData, enum NoteState state){
	//if we need to initialize the envalope
	if(state == FADEOUT_NOTE){
		return envalopeVariables->currentYPos;
	}
	else if(state == TRIGGER_TICK_NOTE){
		envalopeVariables->currentNode = &envalopeData->envalopeNodes[0];
		envalopeVariables->nextNode = &envalopeData->envalopeNodes[1];
		envalopeVariables->currentXPos = 0;
		envalopeVariables->currentYPos = envalopeVariables->currentNode->nodeYPosition;
		envalopeVariables->YPosError = 0;
		envalopeVariables->currentNodeIndex = 0;
	}
	//check if the current tick position is at the sustain loop node, and instrument is sustaining
	else if((envalopeVariables->currentXPos == envalopeData->envalopeNodes[envalopeData->sustainEnd].nodeXPosition) && (state == SUSTAIN_NOTE) && (envalopeData->flags & 4)){
		envalopeVariables->currentNode = &envalopeData->envalopeNodes[envalopeData->sustainBegin];
		envalopeVariables->nextNode = &envalopeData->envalopeNodes[envalopeData->sustainBegin + 1];
		envalopeVariables->currentXPos = envalopeVariables->currentNode->nodeXPosition;
		envalopeVariables->currentYPos = envalopeVariables->currentNode->nodeYPosition;
		envalopeVariables->YPosError = 0;
		envalopeVariables->currentNodeIndex = envalopeData->sustainBegin;
	}
	//check if the current tick position is at the loop node
	else if((envalopeVariables->currentXPos == envalopeData->envalopeNodes[envalopeData->loopEnd].nodeXPosition) && (envalopeData->flags & 2)){
		envalopeVariables->currentNode = &envalopeData->envalopeNodes[envalopeData->loopBegin];
		envalopeVariables->nextNode = &envalopeData->envalopeNodes[envalopeData->loopBegin + 1];
		envalopeVariables->currentXPos = envalopeVariables->currentNode->nodeXPosition;
		envalopeVariables->currentYPos = envalopeVariables->currentNode->nodeYPosition;
		envalopeVariables->YPosError = 0;
		envalopeVariables->currentNodeIndex = envalopeData->loopBegin;
	}
	//if the current tick position is at the final node
	else if(envalopeVariables->currentXPos == envalopeData->envalopeNodes[envalopeData->nodeCount - 1].nodeXPosition){
		envalopeVariables->currentYPos = envalopeData->envalopeNodes[envalopeData->nodeCount - 1].nodeYPosition;
		envalopeVariables->currentNodeIndex = envalopeData->nodeCount - 1;
	}
	//if neither, increment the x position
	else{
		envalopeVariables->currentXPos++;
		//check if we have reached the next node
		if(envalopeVariables->currentXPos >= envalopeVariables->nextNode->nodeXPosition){
			envalopeVariables->currentNodeIndex++;
			envalopeVariables->currentNode = envalopeVariables->nextNode;
			envalopeVariables->nextNode = &envalopeData->envalopeNodes[envalopeVariables->currentNodeIndex + 1];
			envalopeVariables->currentYPos = envalopeVariables->currentNode->nodeYPosition;
			envalopeVariables->YPosError = 0;
		}
		//otherwise, interpolate the y position based on the node data
		else{
			s8 deltaY;
			u16 deltaX;
			u8 absDeltaY;
			s8 deltaYSign;
			deltaY = envalopeVariables->nextNode->nodeYPosition - envalopeVariables->currentNode->nodeYPosition;
			deltaX = envalopeVariables->nextNode->nodeXPosition - envalopeVariables->currentNode->nodeXPosition;
			if(deltaY < 0){
				absDeltaY = -deltaY;
				deltaYSign = -1;
			}
			else{
				absDeltaY = deltaY;
				deltaYSign = 1;
			}
			
			//if the slope is between -1 and 1, use DDA algorithm
			if(deltaX > absDeltaY){
				envalopeVariables->YPosError += absDeltaY;
				if(envalopeVariables->YPosError >= deltaX){
					envalopeVariables->currentYPos += deltaYSign;
					envalopeVariables->YPosError -= deltaX;
				}
			}
			//if the slope has a magnitude 1 or greater, use the interpolation formula LUT
			else{
				envalopeVariables->currentYPos = envalopeVariables->currentNode->nodeYPosition + (deltaY * envalopeInverseTable[deltaX] * (envalopeVariables->currentXPos - envalopeVariables->currentNode->nodeXPosition) >> 15);
			}
		}
	}
	return envalopeVariables->currentYPos;
}

void volumeSlideUp(CurrentChannelSettings *channelPointer, u8 commandAmount){
	if(commandAmount == 0){
		commandAmount = channelPointer->effectMemory.abcd;
	}
	else{
		channelPointer->effectMemory.abcd = commandAmount;
	}
	//apply the change
	channelPointer->currentVolume += commandAmount;
	//check bounds
	if(channelPointer->currentVolume >= 64){
		channelPointer->currentVolume = 64;
	}
}

void volumeSlideDown(CurrentChannelSettings *channelPointer, u8 commandAmount){
	if(commandAmount == 0){
		commandAmount = channelPointer->effectMemory.abcd;
	}
	else{
		channelPointer->effectMemory.abcd = commandAmount;
	}
	//check bounds
	if(channelPointer->currentVolume - commandAmount <= 0){
		channelPointer->currentVolume = 0;
	}
	else{
		channelPointer->currentVolume -= commandAmount;
	}
}

void portamentoDown(CurrentChannelSettings *channelPointer, u8 commandAmount){
	//if it is a "fine portamento down" command
	if(commandAmount >= 0xf0){
		commandAmount &= 0xf;
	}
	//if it is an "extra fine portamento down" command
	else if(commandAmount >= 0xe0){
		channelPointer->pitchModifier = (channelPointer->pitchModifier * extraFinePortamentoTable[15 - (commandAmount & 0xf)]) >> 12;
	}
	else{
		channelPointer->pitchModifier = (channelPointer->pitchModifier * portamentoTable[256 - commandAmount]) >> 12;
	}
}

void portamentoUp(CurrentChannelSettings *channelPointer, u8 commandAmount){
	//if it is a "fine portamento down" command
	if(commandAmount >= 0xf0){
		commandAmount &= 0xf;
	}
	//if it is an "extra fine portamento down" command
	else if(commandAmount >= 0xe0){
		channelPointer->pitchModifier = (channelPointer->pitchModifier * extraFinePortamentoTable[15 + (commandAmount & 0xf)]) >> 12;
	}
	else{
		channelPointer->pitchModifier = (channelPointer->pitchModifier * portamentoTable[256 + commandAmount]) >> 12;
	}
}

u8 portamentoCheck(u8 commandAmount){
	//if this effect happens for just one tick
	if(commandAmount >= 0xE0){
		return 1;
	}
	//if it happens for all ticks other than the first
	else{
		return 0;
	}
}

void initVibrato(VibratoSettings *vibratoPointer, u8 *effectMemory, u8 commandAmount, u8 sweep, u8 flags){
	if((commandAmount & 0xf) == 0){
		commandAmount  = (commandAmount & 0xf0) | (*effectMemory & 0xf);
	}
	else{
		*effectMemory = (*effectMemory & 0xf0) | (commandAmount & 0xf);
	}
	
	if((commandAmount & 0xf0) == 0){
		commandAmount  = (commandAmount & 0xf) | (*effectMemory & 0xf0);
	}
	else{
		*effectMemory = (*effectMemory & 0xf) | (commandAmount & 0xf0);
	}
	
	if((flags & 1) == 0){
		vibratoPointer->depth = (commandAmount & 0xf) << 4;
	}
	else{
		vibratoPointer->depth = (commandAmount & 0xf) << 2;
	}
	
	if((flags & 2) == 0){
		vibratoPointer->speed = (commandAmount & 0xf0) >> 2;
	}
	else{
		vibratoPointer->speed = (commandAmount & 0xf0) >> 4;
	}
	vibratoPointer->sweep = sweep;
	if(sweep == 0){
		vibratoPointer->sweepProgress = 256;
	}
	else{
		vibratoPointer->sweepProgress = 0;
	}
	
	if(vibratoPointer->state == NO_VIBRATO){
		vibratoPointer->state = TRIGGER_VIBRATO;
	}
	else if(vibratoPointer->state == PROCESSED_VIBRATO){
		vibratoPointer->state = PLAY_VIBRATO;
	}
}
s8 processVibrato(VibratoSettings *vibratoPointer){
	u8 retriggerWave = 0;
	if(vibratoPointer->state == NO_VIBRATO){
		return 0;
	}
	else if(vibratoPointer->state == PROCESSED_VIBRATO){
		vibratoPointer->state = NO_VIBRATO;
		return 0;
	}
	else if(vibratoPointer->state == TRIGGER_VIBRATO){
		retriggerWave = 1;
	}
	vibratoPointer->state = PLAY_VIBRATO;
	
	cu8 *waveformTable = sinWaveform;
	switch(vibratoPointer->waveformType){
	case 0:
		waveformTable = sinWaveform;
		if(retriggerWave){
			vibratoPointer->phase = 0;
		}
		break;
	case 1:
		waveformTable = rampDownWaveform;
		if(retriggerWave){
			vibratoPointer->phase = 0;
		}
		break;
	case 2:
		waveformTable = squareWaveform;
		if(retriggerWave){
			vibratoPointer->phase = 0;
		}
		break;
	case 3:
		waveformTable = randomWaveform;
		break;
	case 4:
		waveformTable = sinWaveform;
		break;
	case 5:
		waveformTable = rampDownWaveform;
		break;
	case 6:
		waveformTable = squareWaveform;
		break;
	case 7:
		waveformTable = randomWaveform;
		break;
	case 8:
		waveformTable = rampUpWaveform;
		if(retriggerWave){
			vibratoPointer->phase = 0;
		}
		break;
	}
	
	s8 amplitude;
	//get the amplitude from the selected waveform table
	amplitude = waveformTable[vibratoPointer->phase];
	
	if(vibratoPointer->phase + vibratoPointer->speed >= 256){
		vibratoPointer->phase = (vibratoPointer->phase - 256) + vibratoPointer->speed;
	}
	else{
		vibratoPointer->phase += vibratoPointer->speed;
	}
	
	//calculate the current amplitude of the vibrato
	amplitude = (amplitude * vibratoPointer->depth * vibratoPointer->sweepProgress) >> 16;
	
	//update the sweep
	if(vibratoPointer->sweepProgress + vibratoPointer->sweep >= 256){
		vibratoPointer->sweepProgress = 256;
	}
	else{
		vibratoPointer->sweepProgress += vibratoPointer->sweep;
	}
	
	return amplitude;
};
void tonePortamento(CurrentChannelSettings *channelPointer, u8 commandAmount){
	if(commandAmount == 0){
		commandAmount = channelPointer->effectMemory.Gg;
	}
	else{
		channelPointer->effectMemory.Gg = commandAmount;
	}

	//if we are sliding down
	if(channelPointer->currentPitch > channelPointer->notePitch){
		channelPointer->pitchModifier = (channelPointer->pitchModifier * portamentoTable[256 - commandAmount]) >> 12;
		//if we have reached the target note
		if(((channelPointer->currentPitch * channelPointer->pitchModifier) >> 12) <= channelPointer->notePitch){
			channelPointer->currentPitch = channelPointer->notePitch;
			channelPointer->pitchModifier = 0x1000;
		}
	}
	//if we are sliding up
	else{
		channelPointer->pitchModifier = (channelPointer->pitchModifier * portamentoTable[256 + commandAmount]) >> 12;
		//if we have reached the target note
		if(((channelPointer->currentPitch * channelPointer->pitchModifier) >> 12) >= channelPointer->notePitch){
			channelPointer->currentPitch = channelPointer->notePitch;
			channelPointer->pitchModifier = 0x1000;
		}
	}
}

void positionJump(CurrentSongSettings *songPointer){

	u8 nextOrder;
	u8 nextRow;
	//first, figure out which order we are jumping to
	if(songPointer->globalEffects.B == 0xff){
		nextOrder = songPointer->orderIndex + 1;
	}
	else{
		nextOrder = songPointer->globalEffects.B;
	}
	//check if the jumped-to order is a valid one. If invalid, jump to 0
	if(nextOrder >= songPointer->song->ordersNum){
		nextOrder = 0;
	}
	//then, figure out which row we are jumping to
	if(songPointer->globalEffects.C == 0xff){
		nextRow = 1;
	}
	//check if the new row number is bigger than the number of rows in the new order. If invalid, set to 0
	else if(songPointer->globalEffects.C >= songPointer->song->patterns[songPointer->song->orders[nextOrder]].rowsNum){
		nextRow = 1;
	}
	else{
		nextRow = songPointer->globalEffects.C + 1;
	}
	//set the new values
	songPointer->orderIndex = nextOrder;
	songPointer->rowNum = nextRow;

	//clear the global effects from the queue
	songPointer->globalEffects.B = 0xff;
	songPointer->globalEffects.C = 0xff;
}


void increaseTempo(u8 commandAmount, CurrentSongSettings *songPointer){
	//do bounds checking
	if((songPointer->currentTempo + commandAmount) > 0xff){
		songPointer->currentTempo = 0xff;
	}
	else{
		songPointer->currentTempo = songPointer->currentTempo + commandAmount;
	}
}
void decreaseTempo(u8 commandAmount, CurrentSongSettings *songPointer){
	//do bounds checking
	if(songPointer->currentTempo - commandAmount < 0x20){
		songPointer->currentTempo = 0x20;
	}
	else{
		songPointer->currentTempo = songPointer->currentTempo - commandAmount;
	}
}
void volumeSlide(u8 *currentVolume, u8 *effectMemory, u8 maxVolume, u8 commandAmount){
	//consult the effect memory
	if(commandAmount == 0){
		commandAmount = *effectMemory;
	}
	else{
		*effectMemory = commandAmount;
	}
	//if it is a "volume slide up" 
	if(((commandAmount & 0xf) == 0x0) || (((commandAmount & 0xf) == 0xf) && ((commandAmount & 0xf0) != 0x0))) {
		commandAmount = (commandAmount & 0xf0) >> 4;
		if(*currentVolume + commandAmount > maxVolume){
			*currentVolume = maxVolume;
		}
		else{
			*currentVolume += commandAmount;
		}
	}
	//if it is a "volume slide down"
	else{
		commandAmount = (commandAmount & 0xf);
		if(*currentVolume - commandAmount < 0){
			*currentVolume = 0;
		}
		else{
			*currentVolume -= commandAmount;
		}
	}
}

u8 slideCheck(u8 *effectMemory, u8 commandAmount){
	//consult the effect memory
	if(commandAmount == 0){
		commandAmount = *effectMemory;
	}
	else{
		*effectMemory = commandAmount;
	}
	//if it is a regular "volume slide" return 0
	if(((commandAmount & 0xf) == 0x0) || ((commandAmount & 0xf0) == 0x0)){
		return 0;
	}
	else{
		return 1;
	}
}
void panningSlide(u8 *currentPanning, u8 *effectMemory, u8 commandAmount){
	//consult the effect memory
	if(commandAmount == 0){
		commandAmount = *effectMemory;
	}
	else{
		*effectMemory = commandAmount;
	}
	//if it is a "panning slide up"
	if(((commandAmount & 0xf) == 0x0) || (((commandAmount & 0xf) == 0xf) && ((commandAmount & 0xf0) != 0x0))) {
		commandAmount = (commandAmount & 0xf0) >> 4;
		if(*currentPanning + commandAmount > 64){
			*currentPanning = 64;
		}
		else{
			*currentPanning += commandAmount;
		}
	}
	//if it is a "panning slide down"
	else{
		commandAmount = (commandAmount & 0xf);
		if(*currentPanning - commandAmount < 0){
			*currentPanning = 0;
		}
		else{
			*currentPanning -= commandAmount;
		}
	}
}
void retrigger(CurrentChannelSettings *channelPointer, u8 commandAmount){
	if(commandAmount == 0){
		commandAmount = channelPointer->effectMemory.Q;
	}
	channelPointer->triggerState = TRIGGER;
	channelPointer->volumeState = PROCESSED_VOLUME;
	channelPointer->periodicEffectTimer = (commandAmount & 0xf) - 1;
	commandAmount = (commandAmount & 0xf0) >> 4;
	switch(commandAmount){
	case 1:
		if((channelPointer->currentVolume - 1) < 0){
			channelPointer->currentVolume = 0;
		}
		else{
			channelPointer->currentVolume -= 1;
		}
		break;
	case 2:
		if((channelPointer->currentVolume - 2) < 0){
			channelPointer->currentVolume = 0;
		}
		else{
			channelPointer->currentVolume -= 2;
		}
		break;
	case 3:
		if((channelPointer->currentVolume - 4) < 0){
			channelPointer->currentVolume = 0;
		}
		else
		{
			channelPointer->currentVolume -= 4;
		}
		break;
	case 4:
		if((channelPointer->currentVolume - 8) < 0){
			channelPointer->currentVolume = 0;
		}
		else{
			channelPointer->currentVolume -= 8;
		}
		break;
	case 5:
		if((channelPointer->currentVolume - 16) < 0){
			channelPointer->currentVolume = 0;
		}
		else{
			channelPointer->currentVolume -= 16;
		}
		break;
	case 6:
		channelPointer->currentVolume = (channelPointer->currentVolume * 171) >> 8;
		break;
	case 7:
		channelPointer->currentVolume >>= 1;
		break;
	case 9:
		if(channelPointer->currentVolume + 1 > 0x80){
			channelPointer->currentVolume = 0x80;
		}
		else{
			channelPointer->currentVolume += 1;
		}
		break;
	case 10:
		if(channelPointer->currentVolume + 2 > 0x80){
			channelPointer->currentVolume = 0x80;
		}
		else{
			channelPointer->currentVolume += 2;
		}
		break;
	case 11:
		if(channelPointer->currentVolume + 4 > 0x80){
			channelPointer->currentVolume = 0x80;
		}
		else{
			channelPointer->currentVolume += 4;
		}
		break;
	case 12:
		if(channelPointer->currentVolume + 8 > 0x80){
			channelPointer->currentVolume = 0x80;
		}
		else{
			channelPointer->currentVolume += 8;
		}
		break;
	case 13:
		if(channelPointer->currentVolume + 16 > 0x80){
			channelPointer->currentVolume = 0x80;
		}
		else{
			channelPointer->currentVolume += 16;
		}
		break;
	case 14:
		if(((channelPointer->currentVolume * 3) >> 1) > 0x80){
			channelPointer->currentVolume = 0x80;
		}
		else{
			channelPointer->currentVolume = ((channelPointer->currentVolume * 3) >> 1);
		}
		break;
	case 15:
		if(channelPointer->currentVolume << 1 > 0x80){
			channelPointer->currentVolume = 0x80;
		}
		else{
			channelPointer->currentVolume <<= 1;
		}
		break;
	}
}
void arpeggio(CurrentChannelSettings *channelPointer, u8 commandAmount){
	if(commandAmount == 0){
		commandAmount = channelPointer->effectMemory.J;
	}
	if(channelPointer->periodicEffectState == ARPEGGIO_0){
		channelPointer->periodicEffectState = ARPEGGIO_1;
		channelPointer->pitchModifier = (channelPointer->pitchModifier * arpeggioTable[15 + ((commandAmount & 0xf0) >> 4)]) >> 12;
	}
	else if(channelPointer->periodicEffectState == ARPEGGIO_1){
		channelPointer->periodicEffectState = ARPEGGIO_2;
		channelPointer->pitchModifier = (channelPointer->pitchModifier * arpeggioTable[15 + (commandAmount & 0xf) - ((commandAmount & 0xf0) >> 4)]) >> 12;
	}
	else{
		channelPointer->periodicEffectState = ARPEGGIO_0;
		channelPointer->pitchModifier = (channelPointer->pitchModifier * arpeggioTable[15 - (commandAmount & 0xf)]) >> 12;
	}
}












//let me scroll down please