#ifndef GRAPHICS_H

// OpenGL
//  glad
#include <glad/glad.h>
//  GLFW Header
#include <GLFW/glfw3.h>
//  GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// STB Image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Current Time
extern float currentTime;

// Camera
#include "camera.h"
extern Camera camera;

// FPS reading
extern bool devmode;
extern bool testmode;
extern float fps;

// Updates per second
// extern unsigned int ups;
//// Globals
#include "../globals.h"
// uint g_dynamics_updates_per_second;

// simulation status
extern bool simulating;

// Node Positions and Colors
//extern glm::vec3 nodePositions[];
//extern glm::vec3 nodeColors[];

#include "types.h"
#include "data_structures/graph.h"
#include "core/entity_manager.h"
// #include "models/voter_model.h"
extern graph::Graph* graph1;

// selected nodes
extern bool nodeSelected;
extern core::id_t selectedNodeID;

#include <stack>
#include <unordered_map>

namespace graphics {
    // GLFW window
    GLFWwindow* window{ nullptr };

    // Window
    GLsizei g_scr_width;
    GLsizei g_scr_height;

    // ShaderProgram
    GLuint shaderGraph;    // Shader Program for Drawing Graph
    GLuint shaderText;     // Shader Program for Drawing On-Screen Text
    GLuint shaderTest;

    // Buffer Objects
    GLuint VBO, VAO, EBO;
    
    // Textures
    GLuint textureNode;    // Node
    GLuint textureEdge;    // Edge
    GLuint textureFont;    // Font Bitmap

    // Edge and Node transition structures
    // NOTE: (jllusty) I have no idea what I am doing.jpg
    struct live_edge {
        core::id_t start_node;
        core::id_t end_node;
        float start_time;
        float end_time;
    };
    // NOTE: (jllusty) Uses graph::edge_hash, and may change.
    std::unordered_map<graph::edge_t, live_edge*, graph::edge_hash> liveEdges {};
    struct live_node {
        bool opinion;
        float start_time;
        float end_time;
    };
    std::unordered_map<core::id_t, live_node*> liveNodes {};
    void make_transition(graph::edge_t edge, core::id_t node_to, core::id_t node_from, bool old_opinion, double start_time, double end_time) {
        liveEdges[edge] = new live_edge;
            liveEdges[edge]->start_node = node_from;
            liveEdges[edge]->end_node = node_to;
            liveEdges[edge]->start_time = start_time;
            liveEdges[edge]->end_time = end_time;
        liveNodes[node_to] = new live_node;
            liveNodes[node_to]->opinion = old_opinion;
            liveNodes[node_to]->start_time = start_time;
            liveNodes[node_to]->end_time = end_time;
    }
    void clear_transitions(void) {
        auto it1 = liveEdges.begin();
        while(it1 != liveEdges.end()) {
            if (it1->second) {
                if(it1->second->end_time < currentTime) {
                    delete it1->second;
                    it1 = liveEdges.erase(it1);
                }
                else it1++;
            }
            else it1++;
        }
        auto it2 = liveNodes.begin();
        while(it2 != liveNodes.end()) {
            if (it2->second) {
                if(it2->second->end_time < currentTime) {
                    delete it2->second;
                    it2 = liveNodes.erase(it2);
                }
                else it2++;
            }
            else it2++;
        }
    }

    // Init Graphics (Start GLFW, Load OpenGL with GLAD)
    void init(void);
    // Cleanup
    void cleanup(void);
    // Create Shader Program
    void create_shader(const char* vertexShaderFilename, const char* fragShaderFilename);
    // Load buffer objects for GL_QUAD
    void load_buffers(void);
    // Cleanup buffer objects
    void destroy_buffers(void);
    // Load Texture
    void load_texture(const char* filename);
    // Display
    void render(void);

    // Auxillary Function for Loading Shaders
    GLchar* read_glsl(const char* filename);

    // Auxillary Function for Getting Coordinates of a Character (Text Bitmap)
    glm::vec2 get_character_coords(char c);

    // Init Graphics (Start GLFW, Load OpenGL with GLAD)
    void init(void) {
        /* Initialize GLFW */
        if (!glfwInit()) {
            fprintf(stderr, "(glfw) Failed to initialize\n");
            exit(EXIT_FAILURE);
        }
        // set window hints
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create a windowed mode window and its OpenGL context
        window = glfwCreateWindow(g_scr_width, g_scr_height, "Opinion Dynamics", NULL, NULL);
        if (!window) {
            fprintf(stderr, "(glfw) Failed to create window\n");
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        // Make the window's context current
        glfwMakeContextCurrent(window);

        // glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            fprintf(stderr, "(glad) Failed to initialize.\n");
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        // Enable Blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Enable Depth-Testing
        //glEnable(GL_DEPTH_TEST);
        //glEnable(GL_LESS);

    }
    // Cleanup
    void cleanup(void) {
        glDeleteProgram(shaderGraph);
        glDeleteProgram(shaderText);

        /* GLFW: clean-up */
        glfwTerminate();
    }

    // Create Shader Program
    void create_shader(const char* vertexShaderFilename, const char* fragShaderFilename, GLuint* shaderProgram) {
        // allocate & load vertex source
        GLchar* pVertexSource = read_glsl(vertexShaderFilename);
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &pVertexSource, NULL);
        glCompileShader(vertexShader);
        // check if compilation was successful
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            fprintf(stderr, "(OpenGL) Error, vertex shader compilation failed.\n%s\n", infoLog);
        }
        free(pVertexSource);
        // allocate & load fragment source
        GLchar* pFragSource = read_glsl(fragShaderFilename);
        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &pFragSource, NULL);
        glCompileShader(fragShader);
        // check if compilation was successful
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
            fprintf(stderr, "(OpenGL) Error, fragment shader compilation failed:\n%s\n", infoLog);
        }
        free(pFragSource);
        // create and link shader program
        *shaderProgram = glCreateProgram();
        glAttachShader(*shaderProgram, vertexShader);
        glAttachShader(*shaderProgram, fragShader);
        glLinkProgram(*shaderProgram);
        // check if linking was successful
        glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
            fprintf(stderr, "(OpenGL) Error, linking failed:\n%s\n", infoLog);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragShader);
        // use shader
        // glUseProgram(shaderProgram);
    }

    // Load Buffers with Vertex Data
    void load_buffers(void) {
        /* Setup Vertex Data and Buffers */
        // node position
        float node_x{ 0.0f }, node_y{ 0.0f };
        // node radius
        float node_r{ 1.f };
        // vertex data
        GLfloat vertices[] = {
            // positions         // colors          // texture coords
            1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // top right
            1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // bottom right
           -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // bottom left
           -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,  0.0f, 1.0f   // top left
        };
        // indexing order for GL_QUAD
        GLuint indices[] = {
            0, 1, 3,    // first triangle
            1, 2, 3     // second triangle
        };

        // Create vertex buffer objects
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        // bind VAO first, then bind and set vertex buffers
        glBindVertexArray(VAO);
        // Vertices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Indices (Ordering)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }
    // Destroy buffer objects
    void destroy_buffers(void) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    // Load Texture
    void load_texture(const char* filename, GLuint* texture) {
        /* Load Node Texture */
        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);
        // set texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int img_width, img_height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(filename, &img_width, &img_height, &nrChannels, 0);
        if(data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            fprintf(stderr, "(stb) Could not load %s\n", filename);
        }
        stbi_image_free(data);
    }

    // Define Shader Texture Samplers
    void set_textures(void) {
        // Set Shader Uniforms */
        // glUseProgram(shaderGraph);
        // glUniform1i(glGetUniformLocation(shaderGraph, "textureNode"), 0);
        // glUniform1i(glGetUniformLocation(shaderGraph, "textureEdge"), 1);
        // glUniform1i(glGetUniformLocation(shaderGraph, "textureFont"), 2);
    }

    // Display Scene
    void render(void) {
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/);

        if(testmode) {
            glUseProgram(shaderTest);
            glBindVertexArray(VAO);

            GLuint camLoc = glGetUniformLocation(shaderTest, "cameraPos");
            glUniform3fv(camLoc, 1, glm::value_ptr(camera.pos));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            return;
        }

        // bind textures to texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureNode);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureEdge);
        //glActiveTexture(GL_TEXTURE2);
        //glBindTexture(GL_TEXTURE_2D, textureFont);
        glUseProgram(shaderGraph);
        glUniform1i(glGetUniformLocation(shaderGraph, "textureNode"), 0);
        glUniform1i(glGetUniformLocation(shaderGraph, "textureEdge"), 1);
        //glUniform1i(glGetUniformLocation(shaderGraph, "textureFont"), 2);
        // activate shader
        // transformation matrices
        glm::mat4 view  = glm::mat4(1.0f);
        glm::mat4 proj  = glm::mat4(1.0f);
        view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
        float aspect = (float)g_scr_width/(float)g_scr_height;
        float zoom = camera.pos.z;
        proj = glm::ortho(-zoom*aspect, zoom*aspect, -zoom, zoom, 0.1f, 100.0f);
        //proj = glm::perspective(glm::radians(45.0f), (float)scr_width/(float)scr_height, 0.1f, 100.f);
        // get their uniform locations
        GLuint viewLoc = glGetUniformLocation(shaderGraph, "view");
        GLuint projLoc = glGetUniformLocation(shaderGraph, "proj");
        // pass to shaders
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        GLuint selectLoc = glGetUniformLocation(shaderGraph, "texSelection");

        // bind vertices of unit quad (+/-1,+/-1)
        glBindVertexArray(VAO);

        // Render wires
        // NOTE (jllusty): Draws texture with uv.x = -1 at 
        // elem.first and uv.x = +1 at elem.second
        GLint selection = 2;
        glUniform1i(selectLoc, selection);
        for(const auto& edge : graph1->edges) {
            auto node1 = core::get_entity<graph::Node>(edge.first);
            auto node2 = core::get_entity<graph::Node>(edge.second);
            // auto node1 = graph1->nodes[elem.first];
            // auto node2 = graph1->nodes[elem.second];
            float x1 = node1->x;
            float y1 = node1->y;
            float x2 = node2->x;
            float y2 = node2->y;
            float dist = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
            // set model matrix
            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 edgePosition{ (x1+x2)/2.f, (y1+y2)/2.f, 0.f };
            float theta = atan2((y2-y1),(x2-x1));
            model = glm::translate(model, edgePosition);
            model = glm::rotate(model, theta, glm::vec3(0.f,0.f,1.f));
            model = glm::scale(model, glm::vec3(dist/2.f, 1.f, 1.f));
            GLuint modelLoc = glGetUniformLocation(shaderGraph, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // assimilation pulse
            GLint pulsing;
            GLuint pulsingLoc = glGetUniformLocation(shaderGraph, "pulsing");
            if (liveEdges[edge]) {
                pulsing = 1;
                glUniform1i(pulsingLoc, pulsing);
                // pulse color
                GLuint pulseColorLoc = glGetUniformLocation(shaderGraph, "pulseColor");
                glm::vec3 green{ 0.0f, 1.0f, 0.0f };
                glm::vec3 red{ 1.0f, 0.0f, 0.0f };
                glm::vec3 pulseColor = (core::get_entity<graph::Node>(liveEdges[edge]->start_node)->opinion)?green:red;
                glUniform3fv(pulseColorLoc, 1, glm::value_ptr(pulseColor));
                // compute pulse
                GLuint pulseLoc = glGetUniformLocation(shaderGraph, "pulse");
                float pulse = 
                    1.2f * (
                        g_dynamics_updates_per_second*currentTime 
                        - floor(g_dynamics_updates_per_second*currentTime) 
                    ) - 0.1f;
                if(liveEdges[edge]->start_node != edge.first) pulse = 1.2f - pulse;
                glUniform1f(pulseLoc, pulse);
            }
            else {
                pulsing = 0;
                glUniform1i(pulsingLoc, pulsing);
            }
            // draw edge
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // Render Nodes
        selection = 1;
        glUniform1i(selectLoc, selection);
        // for (uint n = 0; n < graph1->nodes.size(); ++n) {
        for (const auto& [id, _] : graph1->nodes) {
            auto node = core::get_entity<graph::Node>(id);
            // set model matrix
            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 nodePosition{ node->x, node->y, 0.0f };
            model = glm::translate(model, nodePosition);
            GLuint modelLoc = glGetUniformLocation(shaderGraph, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // set color
            GLuint colorLoc = glGetUniformLocation(shaderGraph, "nodeColor");
            glm::vec3 green{ 0.0f, 1.0f, 0.0f };
            glm::vec3 red{ 1.0f, 0.0f, 0.0f };
            glm::vec3 nodeColor{ 0.0f, 0.0f, 0.0f};
            if(liveNodes[id]) {
                nodeColor = (liveNodes[id]->opinion)?green:red;
            }
            else {
                nodeColor = (node->opinion)?green:red;
            } 
            glUniform3fv(colorLoc, 1, glm::value_ptr(nodeColor));
            // highlight if selected node
            GLuint selLoc  = glGetUniformLocation(shaderGraph, "selected");
            int sel{ 0 };
            if (nodeSelected && selectedNodeID == id) {
                sel = 1;
                GLuint highlightLoc = glGetUniformLocation(shaderGraph, "highlight");
                glm::vec3 highlight{ 1.0f, 1.0f, 1.0f };
                glUniform1i(selLoc, sel);
                glUniform3fv(highlightLoc, 1, glm::value_ptr(highlight));
            } else {
                sel = 0;
                glUniform1i(selLoc, sel);
            }
            // draw node
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // Optional: Draw Dev Readout
        if(!devmode) return;
        glUseProgram(shaderText);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureFont);
        glUniform1i(glGetUniformLocation(shaderText, "textureFont"), 2);
        GLuint coordLoc = glGetUniformLocation(shaderText, "charCoords");
        glm::mat4 model = glm::mat4(1.0f);
        GLuint modelLoc = glGetUniformLocation(shaderText, "model");
        // Draw "FPS:_"
        char digits[15];
        int len = sprintf(digits, "FPS: %6.1f", fps);
        glm::vec3 scaling{ 1.f/10.f, 1.f/10.f, 0.f};
        glm::vec3 margin{ -8.f, 8.f, 0.f };
        for(int n = 0; n < len; ++n) {
            glm::vec2 charCoords = get_character_coords(digits[n]);
            glUniform2fv(coordLoc, 1, glm::value_ptr(charCoords));
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, scaling);
            model = glm::translate(model, margin+glm::vec3((float)(n), 0.f, 0.f));
            GLuint modelLoc = glGetUniformLocation(shaderText, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        // Draw "RUN:_"
        margin = glm::vec3(-8.f, 6.f, 0.f);
        char run_status[16];
        if(simulating) len = sprintf(run_status, "RUN: YES");
        else len = sprintf(run_status, "RUN: NO");
        for(int n = 0; n < len; ++n) {
            glm::vec2 charCoords = get_character_coords(run_status[n]);
            glUniform2fv(coordLoc, 1, glm::value_ptr(charCoords));
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, scaling);
            model = glm::translate(model, margin+glm::vec3((float)(n), 0.f, 0.f));
            GLuint modelLoc = glGetUniformLocation(shaderText, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }

    // Auxillary Function for Getting Coordinates of a Character (Text Bitmap)
    glm::vec2 get_character_coords(char c) {
        if(c == '0') return glm::vec2(0.f, 5.f);
        if(c == '1') return glm::vec2(1.f, 5.f);
        if(c == '2') return glm::vec2(2.f, 5.f);
        if(c == '3') return glm::vec2(3.f, 5.f);
        if(c == '4') return glm::vec2(4.f, 5.f);
        if(c == '5') return glm::vec2(5.f, 5.f);
        if(c == '6') return glm::vec2(6.f, 5.f);
        if(c == '7') return glm::vec2(7.f, 5.f);
        if(c == '8') return glm::vec2(0.f, 4.f);
        if(c == '9') return glm::vec2(1.f, 4.f);
        if(c == 'F') return glm::vec2(6.f, 3.f);
        if(c == 'P') return glm::vec2(0.f, 1.f);
        if(c == 'S') return glm::vec2(3.f, 1.f);
        if(c == ':') return glm::vec2(2.f, 4.f);
        if(c == ' ') return glm::vec2(0.f, 7.f);
        if(c == '.') return glm::vec2(6.f, 6.f);
        if(c == 'R') return glm::vec2(2.f, 1.f);
        if(c == 'U') return glm::vec2(5.f, 1.f);
        if(c == 'N') return glm::vec2(6.f, 2.f);
        if(c == 'Y') return glm::vec2(1.f, 0.f);
        if(c == 'E') return glm::vec2(5.f, 3.f);
        if(c == 'S') return glm::vec2(3.f, 1.f);
        if(c == 'N') return glm::vec2(6.f, 2.f);
        if(c == 'O') return glm::vec2(7.f, 2.f);
        return glm::vec2(0.f, 0.f);
    }

    // Auxillary Function for Loading Shaders
    GLchar* read_glsl(const char* filename) {
        GLchar* source = nullptr;
        FILE *fp = fopen(filename, "r");
        if(fp) {
            if (fseek(fp, 0L, SEEK_END) == 0) {
                // get size of file
                long sz = ftell(fp);
                assert(sz != -1);
                source = (GLchar*)malloc(sz*sizeof(GLchar));
                // go to start of file
                fseek(fp, 0L, SEEK_SET);
                // read file into memory
                size_t len = fread(source, sizeof(GLchar), sz, fp);
                if (ferror(fp) != 0) {
                    fprintf(stderr, "(fread) Error reading file '%s\n'", filename);
                }
                else {
                    source[len++] = '\0';
                }
            }
            fclose(fp);
        }
        return source;
    }
}

#define GRAPHICS_H
#endif