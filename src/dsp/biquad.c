#include <math.h>
#include <string.h>
#include "dsp/biquad.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void biquad_init(BiquadFilter* filter) {
    if (!filter) return;

    filter->b0 = 1.0f; filter->b1 = 0.0f; filter->b2 = 0.0f;
    filter->a1 = 0.0f; filter->a2 = 0.0f;
    
    memset(filter->x1, 0, sizeof(filter->x1));
    memset(filter->x2, 0, sizeof(filter->x2));
    memset(filter->y1, 0, sizeof(filter->y1));
    memset(filter->y2, 0, sizeof(filter->y2));
}

void biquad_calc_lowpass(BiquadFilter* filter, float cutoff_freq, float sample_rate, float q) {
    if (!filter || sample_rate <= 0.0f) return;

    // Angular frequency
    float w0 = 2.0f * (float)M_PI * cutoff_freq / sample_rate;
    float alpha = sinf(w0) / (2.0f * q);
    float cos_w0 = cosf(w0);

    // Un-normalized coefficients
    float b0 = (1.0f - cos_w0) / 2.0f;
    float b1 = 1.0f - cos_w0;
    float b2 = (1.0f - cos_w0) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cos_w0;
    float a2 = 1.0f - alpha;

    filter->b0 = b0 / a0;
    filter->b1 = b1 / a0;
    filter->b2 = b2 / a0;
    filter->a1 = a1 / a0;
    filter->a2 = a2 / a0;
}

void biquad_process(BiquadFilter* filter, AudioBuffer* buffer) {
    if (!filter || !buffer || !buffer->samples) return;

    int chans = buffer->channels;
    if (chans > MAX_CHANNELS) chans = MAX_CHANNELS;

    long long frames = buffer->num_frames;
    float* samples = buffer->samples;

    for (long long i = 0; i < frames; i++) {
        for (int c = 0; c < chans; c++) {
            // Handling interleaved audio
            long long index = (i * buffer->channels) + c;
            float in = samples[index];

            float out = filter->b0 * in
                + filter->b1 * filter->x1[c]
                + filter->b2 * filter->x2[c]
                - filter->a1 * filter->y1[c]
                - filter->a2 * filter->y2[c];
            
            // Shift for the next iteration
            filter->x2[c] = filter->x1[c];
            filter->x1[c] = in;
            filter->y2[c] = filter->y1[c];
            filter->y1[c] = out;

            // Write back to the buffer
            samples[index] = out;
        }
    }
}