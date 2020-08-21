#pragma once

#include <stdlib.h>
#include <assert.h>

#include "portaudio.h"
#include "sndfile.h"
#include "kiss_fft.h"

typedef struct callback_data_s
{
    SNDFILE* file;
    SF_INFO info;
} callback_data_s;

// the size of the "input" and "output" buffer should be 2*frame_count to take dual channel into account
void PaUt_tune_frequency(float* input, size_t frame_count, size_t frequency, float* output)
{
    // Input buffer should not point to the same address as output buffer, this operation is not inplace!
    assert(input != output);

    for(size_t i = 0, l = 0; i < frame_count; ++i)
    {
        output[2*i] = input[2*l];
        output[2*i+1] = input[2*l+1];

        l += frequency;
        if(l > frame_count)
            l %= frame_count;
    }
}

void FFT(const kiss_fft_cpx* input, size_t size, kiss_fft_cpx* output)
{
    kiss_fft_cfg cfg = kiss_fft_alloc(size, 0, 0, 0);

    assert(cfg != NULL);

    kiss_fft( cfg , input , output );
    kiss_fft_free(cfg);
}