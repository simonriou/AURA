#ifndef BIQUAD_H
#define BIQUAD_H

#include "aura.h"

#define MAX_CHANNELS 2

typedef struct {
    // Standard difference equation is
    // y[n] = b0 x[n] + b1 x[n-1] + b2 x[n-2] - a1 y[n-1] - a2 y[n-2]

    float b0, b1, b2, a1, a2;   // Filter coefficients

    // History for up to 2 channels
    float x1[MAX_CHANNELS];
    float x2[MAX_CHANNELS];
    float y1[MAX_CHANNELS];
    float y2[MAX_CHANNELS];
} BiquadFilter;

// Initialise the filter
void biquad_init(BiquadFilter* filter);

// Calculates the coefficients for a LPF
// cutoff_freq: frequency to start rolling off
// q: resonance at the cutoff
void biquad_calc_lowpass(BiquadFilter* filter, float cutoff_freq, float sample_rate, float q);

// Filter the buffer
void biquad_process(BiquadFilter* filter, AudioBuffer* buffer);

#endif