#include <math.h>
#include "dsp/master_bus.h"

void apply_global_gain(AudioBuffer* buffer, float gain_db) {
    if (!buffer || !buffer->samples) return;

    // Linear = 10^(dB/20)
    float linear_gain = powf(10.0f, gain_db / 20.0f);

    long long total_samples = buffer->num_frames * buffer->channels;
    for (long long i = 0; i < total_samples; i++) {
        buffer->samples[i] *= linear_gain;
    }
}