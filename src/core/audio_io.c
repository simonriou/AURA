#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include "core/audio_io.h"

AudioBuffer* load_wav(const char* filepath) {
    SF_INFO sfinfo;
    sfinfo.format = 0;

    SNDFILE* file = sf_open(filepath, SFM_READ, &sfinfo);
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filepath);
        fprintf(stderr, "libsndfile error: %s\n", sf_strerror(NULL));
        return NULL;
    }

    // Allocate the struct
    AudioBuffer* buffer = (AudioBuffer*)malloc(sizeof(AudioBuffer));
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed for AudioBuffer.\n");
        sf_close(file);
        return NULL;
    }

    // Fill the struct
    buffer->sample_rate = sfinfo.samplerate;
    buffer->channels = sfinfo.channels;
    buffer->num_frames = sfinfo.frames;

    // Allocate the actual float array for the samples
    buffer->samples = (float*)malloc(buffer->num_frames * buffer->channels * sizeof(float));
    if (!buffer->samples) {
        fprintf(stderr, "Error: Memory allocation failed for audio samples.\n");
        free(buffer);
        sf_close(file);
        return NULL;
    }

    // Read the audio into the array
    sf_readf_float(file, buffer->samples, buffer->num_frames);
    sf_close(file);

    return buffer;
}

int save_wav(const AudioBuffer* buffer, const char* filepath) {
    if (!buffer || !buffer->samples) return -1;

    SF_INFO sfinfo;
    sfinfo.samplerate = buffer->sample_rate;
    sfinfo.channels = buffer->channels;
    sfinfo.frames = buffer->num_frames;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    SNDFILE* file = sf_open(filepath, SFM_WRITE, &sfinfo);
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s' for writing.\n", filepath);
        return -1;
    }

    // Write the float array back to disk
    sf_writef_float(file, buffer->samples, buffer->num_frames);
    sf_close(file);

    return 0;
}

void free_buffer(AudioBuffer* buffer) {
    if (buffer) {
        if (buffer->samples) {
            free(buffer->samples);
        }
        free(buffer);
    }
}