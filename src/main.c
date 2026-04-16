#include <stdio.h>
#include <stdlib.h>
#include "aura.h"

#include "core/audio_io.h"
#include "dsp/master_bus.h"
#include "dsp/biquad.h"

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

    // --- DSP PROCESSING ---

    float gain_change = 0.0f;
    printf("Processing: Applying %.1f dB global gain. . .\n", gain_change);
    apply_global_gain(buffer, gain_change);

    float cutoff = 400; float q = 0.707;
    printf("Processing: Applying Low-Pass Filter at %.1f Hz with Q = %.1f", cutoff, q);
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