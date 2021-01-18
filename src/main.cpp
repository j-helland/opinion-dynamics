// CSTDLIB Header
#include <cstdlib>
// C++ Filesystem
#include <filesystem>
// C++ I/O Streams
#include <iostream>
// C++ Strings
#include <string>
// GLFW Header
#include <GLFW/glfw3.h>
// OpenAL Headers
#include "al.h"
#include "alc.h"
#include "alext.h"
// libsnd Headers
#include "sndfile.h"

int main(void)
{
    /* OpenAL */
    //  create device, context, and get name of device
    auto pALCDevice = alcOpenDevice(nullptr);
    if (!pALCDevice) {
        std::cout << "(OpenAL) Failed to get sound device, exiting.\n";
        return EXIT_FAILURE;
    }
    auto pALCContext = alcCreateContext(pALCDevice, nullptr);
    if (!pALCContext) {
        std::cout << "(OpenAL) Failed to create audio context, exiting.\n";
        return EXIT_FAILURE;
    }
    if (!alcMakeContextCurrent(pALCContext)) {
        std::cout << "(OpenAL) Failed to make context current, exiting.\n";
        return EXIT_FAILURE;
    }
    const ALCchar* name = nullptr;
    if (!alcIsExtensionPresent(pALCDevice, "ALC_ENUMERATE_ALL_EXT"))
        name = alcGetString(pALCDevice, ALC_ALL_DEVICES_SPECIFIER);
    if (!name || alcGetError(pALCDevice) != AL_NO_ERROR)
        name = alcGetString(pALCDevice, ALC_DEVICE_SPECIFIER);
    std::cout << "(OpenAL) Opened " << name << "\n";
    // load sound  
    ALenum err, format;
    ALuint buffer;
    SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;
	//  open audio file and check that it's usable
    std::string cpath = std::filesystem::current_path().string();
    std::cout << "(debug) Current path is '" << cpath << "'\n";
	sndfile = sf_open("..\\..\\res\\sound.ogg", SFM_READ, &sfinfo);
	if(!sndfile) {
		std::cout << "(libsnd) Could not open audio at sound.ogg\n";
		return EXIT_FAILURE;
	}
    if (sfinfo.frames < 1 || sfinfo.frames > (sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
    {
        std::cout << "(libsnd) Bad sample count in sound.ogg (" << sfinfo.frames << ")\n";
        sf_close(sndfile);
        return EXIT_FAILURE;
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
        std::cout << "Unsupported channel count: " << sfinfo.channels << "\n";
        sf_close(sndfile);
        return EXIT_FAILURE;
    }
    //  decode audio file into buffer
    membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));
    num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
    if (num_frames < 1) {
        free(membuf);
        sf_close(sndfile);
        std::cout << "Failed to read samples in sound.ogg\n";
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
        std::cout << "(OpenAL) Error: " << alGetString(err) << "\n";
        if (buffer && alIsBuffer(buffer))
            alDeleteBuffers(1, &buffer);
        return EXIT_FAILURE;
    }
    //  set up a sound source
    ALuint pSource;
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
    alSourcei(pSource, AL_BUFFER, (ALint)pBuffer);

    /* GLFW window instance*/
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return EXIT_FAILURE;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Opinion Dynamics", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glClearColor(0.4f, 0.3f, 0.4f, 0.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // if space is pressed, play sound
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            //  play sound
            alSourcePlay(pSource);
            ALint state = AL_PLAYING;
            while (state == AL_PLAYING && alGetError() == AL_NO_ERROR) {
                alGetSourcei(pSource, AL_SOURCE_STATE, &state);
            }
        }
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    /* GLFW clean-up */
    glfwTerminate();

    /* OpenAL clean-up */
    //  delete buffer
    alDeleteBuffers(1, &buffer);
    //  device config
    if (!alcMakeContextCurrent(nullptr)) {
        std::cout << "(OpenAL) Failed to set context to nullptr, exiting.\n";
        return EXIT_FAILURE;
    }
    alcDestroyContext(pALCContext);
    if (pALCContext) {
        std::cout << "(OpenAL) Failed to unset during close, exiting.\n";
        return EXIT_FAILURE;
    }
    if (!alcCloseDevice(pALCDevice)) {
		std::cout << "(OpenAL) Failed to close sound device, exiting.\n";
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}