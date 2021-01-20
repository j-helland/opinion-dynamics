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

# include "media/audio.h"

int main(void)
{
    /* Initialize OpenAL */
    audio::init();
    // add a sound
    auto sound1 = audio::add_sound("../../res/sound.ogg");
    auto sound2 = audio::add_sound("../../res/bell.ogg");

    //  set up a sound source
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
            audio::play_sound(sound1);
        }
        else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            audio::play_sound(sound2);
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
    audio::destroy();

    return EXIT_SUCCESS;
}