#ifndef AUDIO_H

// printf and fprintf
#include <stdlib.h>

// vector
#include <vector>

// OpenAL Headers
#include "al.h"
#include "alc.h"
#include "alext.h"

// libsnd Headers
#include "sndfile.h"

namespace audio {
    // OpenAL device and context
    ALCdevice* pALCdevice;
    ALCcontext* pALCcontext;

    // sound effect buffers
    std::vector<ALuint> buffers;

    // initialize OpenAL
    void init(void);
    // add sound effect to buffers
    ALuint add_sound(const char* filename);
    // remove sound from buffers (TODO)
    // void remove_sound(const ALuint& buffer); 
    // play sound
    void play_sound(const ALuint buffer);
    // destroy OpenAL
    void destroy(void);

    // initialize OpenAL
    void init(void) {
        //  create device, context, and get name of device
        pALCdevice = alcOpenDevice(nullptr);
        if (!pALCdevice) {
            fprintf(stderr, "(OpenAL) Failed to get sound device, exiting.\n");
            exit(EXIT_FAILURE);
        }
        pALCcontext = alcCreateContext(pALCdevice, nullptr);
        if (!pALCcontext) {
            fprintf(stderr, "(OpenAL) Failed to create audio context, exiting.\n");
            exit(EXIT_FAILURE);
        }
        if (!alcMakeContextCurrent(pALCcontext)) {
            fprintf(stderr, "(OpenAL) Failed to make context current, exiting.\n");
            exit(EXIT_FAILURE);
        }
        const ALCchar* name = nullptr;
        if (!alcIsExtensionPresent(pALCdevice, "ALC_ENUMERATE_ALL_EXT"))
            name = alcGetString(pALCdevice, ALC_ALL_DEVICES_SPECIFIER);
        if (!name || alcGetError(pALCdevice) != AL_NO_ERROR)
            name = alcGetString(pALCdevice, ALC_DEVICE_SPECIFIER);
        printf("(OpenAL) Opened %s\n", name);
    }

    // Add sound effect to buffers
    ALuint add_sound(const char* filename) {
        // load sound  
        ALenum err, format;
        ALuint buffer;
        SNDFILE* sndfile;
        SF_INFO sfinfo;
        short* membuf;
        sf_count_t num_frames;
        ALsizei num_bytes;
        //  open audio file and check that it's usable
        sndfile = sf_open(filename, SFM_READ, &sfinfo);
        if(!sndfile) {
            fprintf(stderr, "(libsnd) Could not open audio at %s\n", filename);
            exit(EXIT_FAILURE);
        }
        if (sfinfo.frames < 1 || sfinfo.frames > (sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
        {
            fprintf(stderr, "(libsnd) Bad sample count (%i) in %s\n", (int)sfinfo.frames, filename);
            sf_close(sndfile);
            exit(EXIT_FAILURE);
        }
        //  get sound format
        format = AL_NONE;
        if (sfinfo.channels == 1)
            format = AL_FORMAT_MONO16;
        else if (sfinfo.channels == 2)
            format = AL_FORMAT_STEREO16;
        else if (sfinfo.channels == 3) {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
                format = AL_FORMAT_BFORMAT2D_16;
        }
        else if (sfinfo.channels == 4) {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
                format = AL_FORMAT_BFORMAT3D_16;
        }
        if (!format) {
            fprintf(stderr, "(libsend) Unsupported channel count (%i) in %s\n", (int)sfinfo.channels, filename);
            sf_close(sndfile);
            exit(EXIT_FAILURE);
        }
        //  decode audio file into buffer
        membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));
        num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
        if (num_frames < 1) {
            free(membuf);
            sf_close(sndfile);
            fprintf(stderr, "Failed to read samples in %s\n", filename);
        }
        num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);
        //  buffer audio data into a new buffer object, then free the data and close the file
        buffer = 0;
        alGenBuffers(1, &buffer);
        alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);
        free(membuf);
        sf_close(sndfile);
        //  check what broke
        err = alGetError();
        if (err != AL_NO_ERROR) {
            fprintf(stderr, "(OpenAL) Error: %s\n", alGetString(err));
            if (buffer && alIsBuffer(buffer))
                alDeleteBuffers(1, &buffer);
            exit(EXIT_FAILURE);
        }

        // add to list of known sound effects
        buffers.push_back(buffer);
        return buffer;
    }

    // play sound
    void play_sound(const ALuint buffer) {
        ALuint pSource;
        // set up speaker
        ALuint source;
        float pitch = 1.f;
        float gain = 1.f;
        float position[3] = {0.f, 0.f, 0.f};
        float velocity[3] = {0.f, 0.f, 0.f};
        ALuint pBuffer = 0;
        alGenSources(1, &pSource);
        alSourcef(pSource, AL_PITCH, pitch);
        alSourcef(pSource, AL_GAIN, gain);
        alSource3f(pSource, AL_POSITION, position[0], position[1], position[2]);
        alSource3f(pSource, AL_VELOCITY, velocity[0], velocity[1], velocity[2]);
        alSourcei(pSource, AL_BUFFER, pBuffer);
        //  set sound source to buffer
        pBuffer = buffer;
        alSourcei(pSource, AL_BUFFER, (ALint)buffer);

        //  play sound (blocking)
        alSourcePlay(pSource);
        ALint state = AL_PLAYING;
        while (state == AL_PLAYING && alGetError() == AL_NO_ERROR) {
            alGetSourcei(pSource, AL_SOURCE_STATE, &state);
        }

        // destroy source
        alDeleteSources(1, &pSource);
    }

    // Destroy OpenAL Context
    void destroy(void) {
        //  delete buffers
        alDeleteBuffers(buffers.size(), buffers.data());
        //  device config
        if (!alcMakeContextCurrent(nullptr)) {
            fprintf(stderr, "(OpenAL) Failed to set context to nullptr, exiting.\n");
            exit(EXIT_FAILURE);
        }
        alcDestroyContext(pALCcontext);
        if (pALCcontext) {
            fprintf(stderr, "(OpenAL) Failed to unset during close, exiting.\n");
            exit(EXIT_FAILURE);
        }
        if (!alcCloseDevice(pALCdevice)) {
            fprintf(stderr, "(OpenAL) Failed to close sound device, exiting.\n");
            exit(EXIT_FAILURE);
        }
    }
}

#define AUDIO_H
#endif