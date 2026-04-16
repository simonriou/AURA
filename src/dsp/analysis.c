#include <stdio.h>
#include <math.h>
#include "dsp/analysis.h"

void env_init(EnvelopeFollower* env, float attack_ms, float release_ms, float sample_rate) {
    if (!env || sample_rate <= 0.0f) return;

    env->enveloppe = 0.0f;

    // coef = exp(-1 / (time_s * sr))
    env->attack_coef = expf(-1.0f / ((attack_ms * 0.001f) * sample_rate));
    env->release_coef = expf(-1.0f / ((release_ms * 0.001f) * sample_rate));
}

float env_process_sample(EnvelopeFollower* env, float input) {
    // We take the | |
    float rect = fabsf(input);

    // Smoothing wrt attack and release coefs
    if (rect > env->enveloppe) {
        // We are rising
        env->enveloppe = env->attack_coef * env->enveloppe + (1.0f - env->attack_coef) * rect;
    } else {
        // We're falling
        env->enveloppe = env->release_coef * env->enveloppe + (1.0f - env->release_coef) * rect;
    }

    return env->enveloppe;
}

void env_print_ascii(EnvelopeFollower* env, AudioBuffer* buffer) {
    if (!env || !buffer || !buffer->samples) return;

    printf("\n--- Envelope Plotting ---\n");

    long long frames = buffer->num_frames;
    int channels = buffer->channels;

    long long print_interval = 2000;

    for (long long i = 0; i < frames; i++) {
        // If stereo, we'll only analyse the left channel (for plotting)
        float input = buffer->samples[i * channels];
        float current_env = env_process_sample(env, input);

        if (i % print_interval == 0) {
            int bars = (int)(current_env * 50.0f);
            if (bars > 50) bars = 50;

            printf("|");
            for (int b = 0; b < bars; b++) printf("█");
            printf("\n");
        }
    }
    printf("\n");
}