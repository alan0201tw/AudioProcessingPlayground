#pragma once

#include "portaudio.h"
#include "sndfile.hh"

typedef struct callback_data_s
{
    SNDFILE* file;
    SF_INFO info;
} callback_data_s;