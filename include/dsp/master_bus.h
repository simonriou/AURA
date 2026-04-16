#ifndef MASTER_BUS_H
#define MASTER_BUS_H

#include "aura.h"

// Applies global gain (in dB) to the entire audio buffer
void apply_global_gain(AudioBuffer* buffer, float gain_db);

#endif