#ifndef AURA_H
#define AURA_H

#include <stdint.h>

typedef struct {
    float* samples;         // Audio data (interleaved if stereo)
    int sample_rate;        //
    int channels;           //
    int64_t num_frames;     // Number of sample frames per channel
} AudioBuffer;

#endif