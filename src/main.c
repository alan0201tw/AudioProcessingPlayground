//
//  main.c
//  sndfile_tutorial
//
//  Created by Matthew Hosack on 11/28/14.
//  Copyright (c) 2014 Matthew Hosack. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#ifndef M_PI
#define M_PI (3.14159265358979323846f)
#endif

// audio processing and audio file I/O
#include "sndfile.h"
#include "portaudio.h"
// fourier transform library
#include "kiss_fft.h"
// rendering dependencies
#include "render_util.h"

#include "util.h"

#define FRAMES_PER_BUFFER (512)
#define WINDOW_WIDTH (256)
#define WINDOW_HEIGHT (512)

int graph[FRAMES_PER_BUFFER / 2];

static int callback(const void *input, void *output,
                    unsigned long frameCount,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags,
                    void *userData );

int main(int argc, const char * argv[])
{
    PaStream *stream;
    PaError error;
    callback_data_s data;
        
    // const char* fileName = "audio_clips/sample.wav";
    const char* fileName = "audio_clips/sample_music.wav";

    fprintf(stderr, "Using file name = %s\n", fileName);

    /* Open the soundfile */
    data.file = sf_open(fileName, SFM_READ, &data.info);
    if (sf_error(data.file) != SF_ERR_NO_ERROR)
    {
        fprintf(stderr, "%s\n", sf_strerror(data.file));
        fprintf(stderr, "File: %s\n", fileName);
        return 1;
    }
    
    /* init portaudio */
    error = Pa_Initialize();
    if(error != paNoError)
    {
        fprintf(stderr, "Problem initializing\n");
        return 1;
    }
    
    /* Open PaStream with values read from the file */
    error = Pa_OpenDefaultStream(&stream
                                 ,0                     /* no input */
                                 ,data.info.channels         /* stereo out */
                                 ,paFloat32             /* floating point */
                                 ,data.info.samplerate
                                 ,FRAMES_PER_BUFFER
                                 ,callback
                                 ,&data);        /* our sndfile data struct */
    if(error != paNoError)
    {
        fprintf(stderr, "Problem opening Default Stream\n");
        return 1;
    }
    
    /* Start the stream */
    error = Pa_StartStream(stream);
    if(error != paNoError)
    {
        fprintf(stderr, "Problem opening starting Stream\n");
        return 1;
    }

    // setup OpenGL context
    GLFWwindow* window = gfx_init(WINDOW_WIDTH, WINDOW_HEIGHT);
    while (!glfwWindowShouldClose(window) && Pa_IsStreamActive(stream))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();

        Pa_Sleep(100);

        float ortho_size_x = (float)WINDOW_WIDTH;
        float ortho_size_y = (float)WINDOW_HEIGHT;
    
        // gluOrtho2D sets up a two-dimensional orthographic viewing region.
        // This is equivalent to calling glOrtho with near = -1 and far = 1 .
        glOrtho(
            0, ortho_size_x,
            0, ortho_size_y,
            -1, 1);

        glBegin(GL_QUADS);
        {
            const float ratio = 2.0f * ((float)WINDOW_WIDTH / (float)FRAMES_PER_BUFFER);

            for(size_t i = 0; i < FRAMES_PER_BUFFER / 2; ++i)
            {
                float absVal = (float)abs(graph[i]);
                
                glColor3f(
                    (float)i / (FRAMES_PER_BUFFER / 2), 
                    absVal / 150, 
                    1.0f);

                glVertex2f( (float)ratio*i, 0.0f );
                glVertex2f( (float)ratio*(i+1), 0.0f );
                glVertex2f( (float)ratio*(i+1), abs(graph[i]) * 3.0f );
                glVertex2f( (float)ratio*i, abs(graph[i]) * 3.0f );
            }
        }
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    /* Close the soundfile */
    sf_close(data.file);

    /*  Shut down portaudio */
    error = Pa_CloseStream(stream);
    if(error != paNoError)
    {
        fprintf(stderr, "Problem closing stream\n");
        return 1;
    }
    
    error = Pa_Terminate();
    if(error != paNoError)
    {
        fprintf(stderr, "Problem terminating\n");
        return 1;
    }

    return 0;
}

static int callback(
    const void                     *input
    ,void                           *output
    ,unsigned long                   frame_count
    ,const PaStreamCallbackTimeInfo *timeInfo
    ,PaStreamCallbackFlags           statusFlags
    ,void                           *userData
    )
{
    float           *out;
    callback_data_s *p_data = (callback_data_s*)userData;
    sf_count_t       num_read;

    out = (float*)output;
    p_data = (callback_data_s*)userData;

    /* clear output buffer */
    memset(out, 0, sizeof(float) * frame_count * p_data->info.channels);

    /* read directly into output buffer */
    num_read = sf_read_float(p_data->file, out, frame_count * p_data->info.channels);

    // increase frequency
    float tmp[2048];
    memcpy(tmp, out, sizeof(float) * frame_count * p_data->info.channels);

    // PaUt_tune_frequency(tmp, frame_count, 1u, out);

    // perform fft

    kiss_fft_cpx complex_input[frame_count];
    kiss_fft_cpx complex_output[frame_count];

    for(size_t i = 0, j = 0; i < frame_count; ++i, ++j)
    {
        float multiplier = 0.5f * 
            (1.0f - cos(2.0f * M_PI * j / (frame_count-1)));

        complex_input[j].r = multiplier * tmp[i];
        complex_input[j].i = 0.0f;
    }

    FFT(complex_input, frame_count, complex_output);

    for(size_t i = 0; i < frame_count / 2; i++ )
    {
		float val = 
            sqrt((complex_output[i].r * complex_output[i].r) 
            + (complex_output[i].i * complex_output[i].i));
        
        graph[i] = (val > 0) ? (log(val) * 10) : 0;

        // printf("%d \n", graph[i]);
    }
    // printf("____________\n");

    // end of - perform fft

    /*  If we couldn't read a full frameCount of samples we've reached EOF */
    if (num_read < frame_count)
    {
        return paComplete;
    }
    
    return paContinue;
}