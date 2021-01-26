// CSTDLIB Header
#include <cstdlib>
// C++ Filesystem
#include <filesystem>
// C++ I/O Streams
#include <stdio.h>
// assert
#include <assert.h>
// OpenGL
#include "media/graphics.h"
// OpenAL
#include "media/audio.h"
// Camera Object
#include "camera.h"
Camera camera;

// Process Input Handle
void processInput(GLFWwindow* window);

// Frame-by-frame Timing
float deltaTime{ 0.0f };
float lastFrame{ 0.0f };

// Highly abstract Mouse Object
struct Mouse {
    double x;
    double y;
};
Mouse mouse;

// Example Nodes Attributes
// Node Positions
glm::vec3 nodePositions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f)
};
// Node Colors
glm::vec3 nodeColors[] = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
};

int main(void)
{
    /* Initialize Graphics */
    graphics::init();
    // Create Shader Program
    graphics::create_shader("../../src/media/shaders/vertex.glsl", "../../src/media/shaders/frag.glsl");
    // Load Vertex Data (for Quad)
    graphics::load_buffers();
    // Load Texture Data (for Node)
    graphics::load_texture("../../res/node.png");
    
    /* Initialize OpenAL */
    audio::init();
    // add a sound
    ALuint sound1 = audio::add_sound("../../res/sound.ogg");
    // add a source
    ALuint* pSource1 = audio::create_source();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(graphics::window))
    {
        // per-frame timing
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* Process Input */
        processInput(graphics::window);
        // get cursor position
        glfwGetCursorPos(graphics::window, &mouse.x, &mouse.y);
        // use it to set the pitch (lel)
        audio::set_source(pSource1, (float)mouse.x/640.f);
        // if space is pressed, play sound
        if (glfwGetKey(graphics::window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            audio::play_sound(pSource1, sound1);
        }

        /* Render Nodes */
        graphics::render();

        /* Swap front and back buffers */
        glfwSwapBuffers(graphics::window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    /* OpenAL: clean-up */
    audio::destroy_source(pSource1);
    audio::destroy();

    /* Graphics: clean-up */
    graphics::cleanup();

    return EXIT_SUCCESS;
}

void processInput(GLFWwindow *window) {
    const float speed = 2.5f * deltaTime;
    // pan
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.y += speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.x -= speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.y -= speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.x += speed;
    // zoom
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.z -= speed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.z += speed;
}