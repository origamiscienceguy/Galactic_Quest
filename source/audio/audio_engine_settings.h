//the most DMA channels that can be playing at the same time.
//Increasing this value will greatly reduce performance.
#ifndef MAX_DMA_CHANNELS
	#define MAX_DMA_CHANNELS 12
#endif

//the number of songs that can be playing at the same time.
//Adding any further songs will remove a song based on priority.
//EWRAM usage will be slightly increased by increasing this value.
//Performance will be slightly increased as this value increases.
#ifndef MAX_SONGS_IN_QUEUE
	#define MAX_SONGS_IN_QUEUE 6
#endif

//the number of cycles between each DMA value being sent to the DAC.
//Sample rate is (16,777,216 / DMA_CYCLES_PER_SAMPLE)
//512 is the minimum value, and represents the highest sample rate.
//Decreasing this value will greatly reduce performance.
//Approximately (561,792 / DMA_CYCLES_PER_SAMPLE) bytes will be permenantly allocated in EWRAM.
#ifndef DMA_CYCLES_PER_SAMPLE
	#define DMA_CYCLES_PER_SAMPLE 512
#endif

//the maximum number of samples that are allowed to be mixed in one batch.
//reducing this number will reduce the amount of IWRAM stack space used by the mixer.
//but will also slightly reduce performance.
//Approximately (MAX_SAMPLES_MIXED_AT_ONCE * MAX_DMA_CHANNELS * 2)...
//bytes of temporary IWRAM stack space will be used by the mixer.
//!!!*** THIS VALUE MUST BE DIVISIBLE BY 4 ***!!!
#ifndef MAX_SAMPLE_MIXED_AT_ONCE
	#define MAX_SAMPLE_MIXED_AT_ONCE 64
#endif