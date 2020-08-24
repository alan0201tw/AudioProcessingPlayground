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
void PaUt_tune_frequency(float* input, size_t frame_count, size_t frequency, float* output);

void FFT(const kiss_fft_cpx* input, size_t size, kiss_fft_cpx* output);