#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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

void env_print_ascii(EnvelopeFollower* env, TransientDetector* td, AudioBuffer* buffer) {
    if (!env || !td || !buffer || !buffer->samples) return;

    printf("\n--- Envelope & Transient Plotting ---\n");

    long long frames = buffer->num_frames;
    int channels = buffer->channels;
    long long print_interval = 2000;

    for (long long i = 0; i < frames; i++) {
        // If stereo, we'll only analyse the left channel (for plotting)
        float input = buffer->samples[i * channels];
        float current_env = env_process_sample(env, input);

        (void)trans_process_sample(td, current_env);

        if (i % print_interval == 0) {
            int bars = (int)(current_env * 50.0f);
            if (bars > 50) bars = 50;

            if (td->frames_since_hit < print_interval) {
                printf("[ IMPACT ]");
            } else {
                printf("          ");
            }

            printf("|");
            for (int b = 0; b < bars; b++) printf("█");
            printf("\n");
        }
    }
    printf("\n");
}

void trans_init(TransientDetector* td, float threshold, float debounce_ms, float sample_rate) {
    if (!td || sample_rate <= 0.0f) return;

    td->prev_env = 0.0f;
    td->threshold = threshold;

    // ms to sample frames
    td->debounce_frames = (long long)((debounce_ms * 0.001f) * sample_rate);
    td->frames_since_hit = td->debounce_frames;
}

int trans_process_sample(TransientDetector* td, float current_env) {
    int is_hit = 0;

    float delta = current_env - td->prev_env;

    // Is the jump > threshold? && are we ready to trigger?
    if (delta > td->threshold && td->frames_since_hit >= td->debounce_frames) {
        is_hit = 1;
        td->frames_since_hit = 0;
    } else {
        td->frames_since_hit++;
    }

    // Save current envelope
    td->prev_env = current_env;

    return is_hit;
}

void yin_init(YinDetector* yin, int buffer_size, float threshold) {
    if (!yin || buffer_size <= 0) return;

    yin->threshold = threshold;
    yin->buffer_size = buffer_size;
    yin->half_size = buffer_size / 2;

    yin->yin_buffer = (float*)calloc(yin->half_size, sizeof(float));
}

void yin_free(YinDetector* yin) {
    if (yin && yin->yin_buffer) {
        free(yin->yin_buffer);
        yin->yin_buffer = NULL;
    }
}

float yin_process(YinDetector* yin, const float* audio_block, float sample_rate) {
    if (!yin || !yin->yin_buffer || !audio_block) return -1.0f;

    int tau; int half_size = yin->half_size;

    // Difference function
    for (tau = 0; tau < half_size; tau++) {
        yin->yin_buffer[tau] = 0.0f;
    }

    for (tau = 1; tau < half_size; tau++) {
        for (int i = 0; i < half_size; i++) {
            float delta = audio_block[i] - audio_block[i + tau];
            yin->yin_buffer[tau] += delta * delta;
        }
    }

    // Cumulative Mean Normalized Difference Func (CMNDF)
    yin->yin_buffer[0] = 1.0f;
    float running_sum = 0.0f;
    for (tau = 1; tau < half_size; tau++) {
        running_sum += yin->yin_buffer[tau];
        yin->yin_buffer[tau] *= tau / running_sum;
    }

    // Absolute thresholding
    int tau_estimate = -1;
    for (tau = 2; tau < half_size; tau++) {
        if (yin->yin_buffer[tau] < yin->threshold) {
            // We found a valley below the threshold
            // We keep looking for the minimum
            while (tau + 1 < half_size && yin->yin_buffer[tau + 1] < yin->yin_buffer[tau]) {
                tau++;
            }
            tau_estimate = tau;
            break;
        }
    }

    if (tau_estimate == -1) {
        return -1.0f;
    }

    // Parabolic interpolation for sub-sample accuracy
    float better_tau = (float)tau_estimate;
    if (tau_estimate > 0 && tau_estimate < half_size - 1) {
        float s0 = yin->yin_buffer[tau_estimate - 1];
        float s1 = yin->yin_buffer[tau_estimate];
        float s2 = yin->yin_buffer[tau_estimate + 1];

        float bottom_offset = (s2 - s0) / (2.0f * (2.0f * s1 - s2 - s0));
        better_tau += bottom_offset;
    }

    return sample_rate / better_tau;
}