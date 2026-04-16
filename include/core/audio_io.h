#ifndef AUDIO_IO_H
#define AUDIO_IO_H

#include "aura.h"

// Load a WAV/AIFF file into an AudioBuffer. Returns
// - NULL on failure
// - The buffer on success
AudioBuffer* load_wav(const char* filepath);

// Saves an AudioBuffer to disk. Returns
// - non-zero on failure
// 0 on success
int save_wav(const AudioBuffer* buffer, const char* filepath);

// Safely frees the mem allocated for the audio buffer
void free_buffer(AudioBuffer* buffer);

#endif