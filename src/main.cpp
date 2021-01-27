// CSTDLIB Header
#include <cstdlib>
// C++ Filesystem
#include <filesystem>
// C++ I/O Streams
#include <stdio.h>
// assert
#include <assert.h>
//
#include <cmath>
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

// TODO (jæn)
// 1 - Implement another model of your discretion
// 2 - Think about allocation strategies for edges
// 3 - Load/Save Graphs (Serialization)
// 4 - Random graph generation
// 5 - Alternative graph models

// TODO (jllusty)
// 1 - Edge Textures
// 2 - FPS Indicator / Dev Mode
// 3 - Window Resizing Callback
// 4 - Better Controls
// 5 - Graph Editor

#define TEST_SIZE (100)
//#define TEST_SIMULATION_STEPS (100)

// From voter_model_test.cpp
#include "types.h"
#include "data_structures/graph.h"
#include "models/voter_model.h"
#include "random.h"

// graph
graph::Graph* graph1 { nullptr };

int main(void)
{
    // Make a Graph
    graph1 = graph::make(TEST_SIZE);  // undirected graph
    init_graph_opinions(graph1);  // uniform-random opinions
    // add edges
    std::bernoulli_distribution dist(0.05);
    for (uint n = 0; n < graph1->nodes.size(); ++n) {
        for (uint k = 0; k < graph1->nodes.size(); ++k) {
            if (n == k) continue;
            if (dist(rng::generator)) {
                graph::add_edge(graph1, n, k);
            }
        }
    }
    // move 'em around
    // theta
    float pi = 4. * atan(1.f);
    float theta = 0.0f;
    for (uint n = 0; n < graph1->nodes.size(); ++n) {
        theta = (float)n * 2.0f * pi / (float)(graph1->nodes.size());
        float x = 100.f*cos(theta);
        float y = 100.f*sin(theta);
        graph1->nodes[n]->properties->x = x;
        graph1->nodes[n]->properties->y = y;
    }

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
    glfwSetTime(0.0);
    uint currentSecond{ 0 };
    while (!glfwWindowShouldClose(graphics::window))
    {
        /* Timing Calculations */
        // per-frame timing
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // update graph every 1 second of realtime
        if ((uint)(20.f*currentFrame) > currentSecond) {
            currentSecond++;
            step_dynamics(sample_nodes(graph1));
        }

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

    /* Graph */
    // free the graph
    graph::destroy(graph1);

    return EXIT_SUCCESS;
}

void processInput(GLFWwindow *window) {
    const float speed = 25.f * deltaTime;
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