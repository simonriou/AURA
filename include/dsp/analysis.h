#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "aura.h"

typedef struct {
    float enveloppe;        // Current loudness value
    float attack_coef;      // How fast we track rising volume      
    float release_coef;     // How fast we fall when volume drops
} EnvelopeFollower;

// Initialize the envelope follower with parameters
void env_init(EnvelopeFollower* env, float attack_ms, float release_ms, float sample_rate);

// Process a single sample and return the current env. level
float env_process_sample(EnvelopeFollower* env, float input);

// Analyze a whole buffer and prints ASCII plot to the console
void env_print_ascii(EnvelopeFollower* env, AudioBuffer* buffer);

#endif