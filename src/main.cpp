// CSTDLIB Header
#include <cstdlib>
// C++ I/O Streams
#include <iostream>
// GLFW Header
#include <GLFW/glfw3.h>
// OpenAL Headers
#include "al.h"
#include "alc.h"
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
	sndfile = sf_open("..\\sound.ogg", SFM_READ, &sfinfo);
	if(!sndfile) {
		std::cout << "(libsnd) Could not open audio at sound.ogg\n";
		return EXIT_FAILURE;
	}

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