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

// Window Resize Callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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
// 1 - Edge Textures [X]
// 2 - FPS Indicator / Dev Mode [X]
// 3 - Window Resizing Callback [X]
// 4 - Better Controls [X]
// 5 - Graph Editor

// TODO (someone)
// 1 - Draw nice graphs by treating edges as springs and letting it reach stability.
// 2 - Use FreeFont for rendering TrueType Fonts (ideally, JetBrains Mono) in dev mode
// 3 - FPS Counter is exponentially weighted, so if there are significant changes it slowly
//     will converge to that average, hence, if the sample is considerably different than the
//     average (say, 99%?) the frame counter and previous measurement (old_fps) should reset to 0.
//     Hard reset on any graphics settings changing (window resizing, etc).
// 4 - Before first merge with main, need to change included library to git submodules.

// Runtime Diagnostics / Devmode
// NOTE (jllusty): This should turn into a diagnostic struct of info to extern to the renderer.
bool devmode{ false };
float fps{ 0.f };
// GLFWKey Callback for Devmode Toggle (and other toggles)
void devmode_toggle(GLFWwindow* window, int key, int scancode, int action, int mods);

// Graph
#define TEST_SIZE (8)
//#define TEST_SIMULATION_STEPS (100)

// From voter_model_test.cpp
#include "types.h"
#include "data_structures/graph.h"
#include "models/voter_model.h"
#include "random.h"

// graph
graph::Graph* graph1 { nullptr };
// updating
bool simulating{ true };
// selected node
bool nodeSelected{ false };
uint selectedNode{ 0 };
// current position of mouse in world coords
glm::vec4 cPos;

int main(void)
{
    cPos = glm::vec4(0.f, 0.f, 0.f, 0.f);
    // Make a Graph
    graph1 = graph::make(TEST_SIZE);  // undirected graph
    init_graph_opinions(graph1);  // uniform-random opinions
    // add edges
    //std::bernoulli_distribution dist(0.05);
    for (uint n = 0; n < graph1->nodes.size(); ++n) {
        for (uint k = 0; k < graph1->nodes.size(); ++k) {
            if (n == k) continue;
     //       if (dist(rng::generator)) {
                graph::add_edge(graph1, n, k);
      //      }
        }
    }
    // move 'em around
    // theta
    float pi = 4. * atan(1.f);
    float theta = 0.0f;
    for (uint n = 0; n < graph1->nodes.size(); ++n) {
        theta = (float)n * 2.0f * pi / (float)(graph1->nodes.size());
        float x = 10.f*cos(theta);
        float y = 10.f*sin(theta);
        graph1->nodes[n]->properties->x = x;
        graph1->nodes[n]->properties->y = y;
    }

    /* Initialize Graphics */
    graphics::init();
    // Set Callbacks 
    glfwSetFramebufferSizeCallback(graphics::window, framebuffer_size_callback);
    glfwSetKeyCallback(graphics::window, devmode_toggle);   // set devmode toggle
    // Create Shader Programs
    //  Graph Shader
    graphics::create_shader("../../src/media/shaders/graph/vertex.glsl", 
                            "../../src/media/shaders/graph/frag.glsl", 
                            &graphics::shaderGraph);
    //  Text Shader
    graphics::create_shader("../../src/media/shaders/font/vertex.glsl", 
                            "../../src/media/shaders/font/frag.glsl", 
                            &graphics::shaderText);
    // Load Vertex Data (for Quad)
    graphics::load_buffers();
    // Load Texture Data
    graphics::load_texture("../../res/node.png", &graphics::textureNode);               // Node
    graphics::load_texture("../../res/line.png", &graphics::textureEdge);               // Edge
    graphics::load_texture("../../res/font/MS_Gothic.png", &graphics::textureFont);     // Font
    // Set Texture Data (defines texture samplers in shader program)
    // NOTE (jllusty): As the program swaps between shaders and active / bound textures,
    //                 I don't think this can be done before the render loop (unsure).
    // graphics::set_textures();
    
    /* Initialize OpenAL */
    audio::init();
    // add a sound
    ALuint sound1 = audio::add_sound("../../res/sound.ogg");
    // add a source
    ALuint* pSource1 = audio::create_source();

    /* Loop until the user closes the window */
    glfwSetTime(0.0);
    uint currentSecond{ 0 };
    long frames = 0;
    float oldfps = 0;
    while (!glfwWindowShouldClose(graphics::window))
    {
        /* Timing Calculations */
        // per-frame timing
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // measure FPS (Exponential Moving Average w/ alpha = 1 / Num Frames)
        // NOTE (jllusty): This starts / resets every time devmode is toggled *on*
        if(devmode) {
            frames++;
            float alpha = 1.f/(float)frames;
            fps = ((((float)frames-1.f))*(oldfps) + 1.f/deltaTime)/(float)frames;
            oldfps = fps;
        } else { frames = 0; fps = 0.0f; oldfps = 0.0f; }

        // update graph every 1 second of realtime
        if ((uint)(1.f*currentFrame) > currentSecond) {
            currentSecond++;
            if (simulating) step_dynamics(sample_nodes(graph1));
        }

        /* Process Input */
        processInput(graphics::window);
        //audio::set_source(pSource1, (float)mouse.x/640.f);
        // if space is pressed, play sound
        //if (glfwGetKey(graphics::window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        //    audio::play_sound(pSource1, sound1);
       //}

        /* Render Graph */
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

// continuous input
void processInput(GLFWwindow *window) {
    const float speed = 20.f * deltaTime;
    // pan
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.pos.y += speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.pos.x -= speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.pos.y -= speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.pos.x += speed;
    // zoom
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.pos.z -= speed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.pos.z += speed;

    // check if we touchy-touched a node with a mouse clicky-click
    // update cursor position anyway
    glfwGetCursorPos(graphics::window, &mouse.x, &mouse.y);
    // check if we clicked the the left mouse button
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        // are we already touchy-touching a node?
        if (!nodeSelected) {
            // if not, are we touchy-touching one now?
            bool selected{ false };
            // make view and projection matrices
            glm::mat4 view  = glm::mat4(1.0f);
            glm::mat4 proj  = glm::mat4(1.0f);
            view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
            float aspect = (float)graphics::scr_width/(float)graphics::scr_height;
            float zoom = camera.pos.z;
            proj = glm::ortho(-zoom*aspect, zoom*aspect, -zoom, zoom, 0.1f, 100.0f);
            for (uint n = 0; n < graph1->nodes.size(); ++n) {
                // get position of node in view coords
                glm::mat4 model = glm::mat4(1.0f);
                glm::vec3 nodePosition{ graph1->nodes[n]->properties->x, graph1->nodes[n]->properties->y, 0.0f };
                model = glm::translate(model, nodePosition);
                // origin
                glm::vec4 oPos = proj * view * model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0);
                    oPos = oPos / oPos.w;
                // edge of texture
                glm::vec4 ePos = proj * view * model * glm::vec4(1.0f, 0.0f, 0.0f, 1.0);
                    ePos = ePos / ePos.w;
                // radius
                float r = glm::distance(oPos, ePos);
                // mouse position in normalized screen coordinates
                glm::vec2 mPos{ 2.f*mouse.x/graphics::scr_width-1.f, -2.f*mouse.y/graphics::scr_height+1.f };
                // get distance from node origin to mouse position
                float dMouse = glm::distance(glm::vec2(oPos.x, oPos.y), mPos);
                if (dMouse < r) { selected = true; selectedNode = n; nodeSelected = true; break; }
            }
            if (!selected) { nodeSelected = false; }
        }
        // if we touchy-touched a node and the user is still holding down the mouse,
        // drag that little 'en along
        else {
            // mouse position in normalized "screen" coordinates
            glm::vec4 mPos{ 2.f*mouse.x/graphics::scr_width-1.f, -2.f*mouse.y/graphics::scr_height+1.f, 0.0f, 1.0f };
            // get camera transform, use it to invert the mouse position into world coordinates
            glm::mat4 view  = glm::mat4(1.0f);
            glm::mat4 proj  = glm::mat4(1.0f);
            view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
            float aspect = (float)graphics::scr_width/(float)graphics::scr_height;
            float zoom = camera.pos.z;
            proj = glm::ortho(-zoom*aspect, zoom*aspect, -zoom, zoom, 0.1f, 100.0f);
            glm::mat4 model = glm::mat4(1.0f);
            //proj = glm::perspective(glm::radians(45.0f), (float)graphics::scr_width/(float)graphics::scr_height, 0.1f, 100.f);
            glm::vec3 nodePosition{ graph1->nodes[selectedNode]->properties->x, graph1->nodes[selectedNode]->properties->y, 0.0f };
            model = glm::translate(model, nodePosition);
            glm::mat4 invCamera = glm::inverse(proj * view);
            cPos = invCamera * mPos;
                cPos = cPos / cPos.w;
            auto newNode = graph1->nodes[selectedNode];
            newNode->properties->x = cPos.x;
            newNode->properties->y = cPos.y;
        }
    }
    // we aren't touchy-touching anything
    else { nodeSelected = false; }
}

// devmode toggle, also other toggles
void devmode_toggle(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // enable devmode output
    if(key == GLFW_KEY_F1 && action == GLFW_PRESS)
        devmode = !devmode;
    // pause / play simulation
    if(key == GLFW_KEY_P && action == GLFW_PRESS)
        simulating = !simulating;
}

// window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Update Viewport
    glViewport(0, 0, width, height);
    // Update Screen Sizes
    graphics::scr_width = width;
    graphics::scr_height = height;
}