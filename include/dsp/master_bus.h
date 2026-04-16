#ifndef MASTER_BUS_H
#define MASTER_BUS_H

#include "aura.h"

// Applies global gain (in dB) to the entire audio buffer
void apply_global_gain(AudioBuffer* buffer, float gain_db);

// Clamps all audio samples to [-1.0, 1.0]
void apply_hard_clip(AudioBuffer* buffer);

#endif