#include <stdio.h>
#include <stdlib.h>
#include "aura.h"

#include "core/audio_io.h"
#include "dsp/master_bus.h"
#include "dsp/biquad.h"
#include "dsp/analysis.h"

int compare_floats(const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

int main(int argc, char* argv[]) {

    // --- CONFIG & INPUT ---

    if (argc < 3) {
        printf("Usage: %s <input.wav> <output.wav>\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];

    printf("--- A.U.R.A. Audio Engine ---\n");
    printf("Loading: %s\n", input_file);

    AudioBuffer* buffer = load_wav(input_file);
    if (!buffer) {
        return 1;
    }

    printf("File sucessfully loaded!\n");

    // --- ANALYSIS ---

    printf("Analyzing: Running envelope follower. . .\n");

    EnvelopeFollower env;
    env_init(&env, 5.0f, 100.0f, buffer->sample_rate);

    TransientDetector td;
    trans_init(&td, 0.0025f, 50.0f, buffer->sample_rate);

    env_print_ascii(&env, &td, buffer);

    printf("Analyzing: Running YIN pitch detection. . .\n");

    YinDetector yin;
    int window_size = 2048;
    yin_init(&yin, window_size, 0.25);

    int max_windows = 9;
    int hop_size = 512;
    float collected_pitches[9];
    int valid_pitch_count = 0;

    long long start_frame = buffer->sample_rate * 0.05f;

    for (int w = 0; w < max_windows; w++) {
        long long current_frame = start_frame + (w * hop_size);

        if (current_frame + window_size < buffer->num_frames) {

            float mono_block[2048];
            for (int i = 0; i < window_size; i++) {
                mono_block[i] = buffer->samples[(current_frame + i) * buffer->channels];
            }

            float pitch = yin_process(&yin, mono_block, buffer->sample_rate);

            if (pitch > 0.0f) {
                collected_pitches[valid_pitch_count] = pitch;
                valid_pitch_count++;
            }
        }
    }

    if (valid_pitch_count >= (max_windows / 3)) {
        qsort(collected_pitches, valid_pitch_count, sizeof(float), compare_floats);
        float final_macro_pitch = collected_pitches[valid_pitch_count / 2];

        printf("Pitch result: %.2f Hz (%d/%d windows agreed)\n", final_macro_pitch, valid_pitch_count, max_windows);
    } else {
        printf("Pitch result: NO PITCH FOUND\n");
    }

    yin_free(&yin);

    // --- DSP PROCESSING ---

    float gain_change = 0.0f;
    printf("Processing: Applying %.1f dB global gain. . .\n", gain_change);
    apply_global_gain(buffer, gain_change);

    float cutoff = 400; float q = 0.707;
    printf("Processing: Applying Low-Pass Filter at %.1f Hz with Q = %.1f\n", cutoff, q);
    BiquadFilter lpf;
    biquad_init(&lpf);
    biquad_calc_lowpass(&lpf, cutoff, buffer->sample_rate, q);
    biquad_process(&lpf, buffer);

    printf("Processing: Applying Hard Clipper. . .\n");
    apply_hard_clip(buffer);

    // --- OUTPUT ---

    printf("Writing to %s\n", output_file);
    if (save_wav(buffer, output_file) == 0) {
        printf("Write successfull.\n");
    }

    free_buffer(buffer);

    return 0;
}