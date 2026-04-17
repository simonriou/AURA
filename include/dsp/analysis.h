#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "aura.h"

typedef struct {
    float enveloppe;        // Current loudness value
    float attack_coef;      // How fast we track rising volume      
    float release_coef;     // How fast we fall when volume drops
} EnvelopeFollower;

typedef struct {
    float prev_env;             // Envelope value from the previous sample
    float threshold;            // How sharp the spike needs to be to trigger
    long long debounce_frames;  // How long to wait before allowing a new trigger
    long long frames_since_hit; // Counter to track the cooldown
} TransientDetector;

typedef struct {
    float threshold;    // 0.1 - 0.15
    int buffer_size;
    int half_size; 
    float* yin_buffer;
} YinDetector;

// Initialize the envelope follower with parameters
void env_init(EnvelopeFollower* env, float attack_ms, float release_ms, float sample_rate);

// Process a single sample and return the current env. level
float env_process_sample(EnvelopeFollower* env, float input);

// Analyze a whole buffer and prints ASCII plot to the console
void env_print_ascii(EnvelopeFollower* env, TransientDetector* td, AudioBuffer* buffer);

// Initialize the transient detector
void trans_init(TransientDetector* td, float threshold, float debounce_ms, float sample_rate);

// Process the current envelope value. Returns
// - 1 for a detected transient
// - 0 else
int trans_process_sample(TransientDetector* td, float current_env);

// Initialze YIN and allocate internal buffer
void yin_init(YinDetector* yin, int buffer_size, float threshold);

// Analyze a block of audio and returns the fundamental freq.
// Returns -1.0f if no pitch was found
float yin_process(YinDetector* yin, const float* audio_block, float sample_rate);

// Frees the internal memory
void yin_free(YinDetector* yin);

#endif