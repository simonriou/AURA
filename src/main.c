#include <stdio.h>
#include <stdlib.h>
#include "aura.h"
#include "core/audio_io.h"

int main(int argc, char* argv[]) {

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

    printf("Writing to %s\n", output_file);
    if (save_wav(buffer, output_file) == 0) {
        printf("Write successfull.\n");
    }

    free_buffer(buffer);

    return 0;
}