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

// Camera
#include "camera.h"
extern Camera camera;

// Node Positions and Colors
//extern glm::vec3 nodePositions[];
//extern glm::vec3 nodeColors[];

#include "types.h"
#include "data_structures/graph.h"
#include "models/voter_model.h"
extern graph::Graph* graph1;

namespace graphics {
    // GLFW window
    GLFWwindow* window{ nullptr };

    // Window
    GLsizei scr_width{ 640 };
    GLsizei scr_height{ 480 };

    // ShaderProgram
    GLuint shaderProgram;

    // Buffer Objects
    GLuint VBO, VAO, EBO;
    
    // Textures
    GLuint texture1;    // Node
    GLuint texture2;    // Edge

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
        window = glfwCreateWindow(scr_width, scr_height, "Opinion Dynamics", NULL, NULL);
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
    }
    // Cleanup
    void cleanup(void) {
        glDeleteProgram(shaderProgram);

        /* GLFW: clean-up */
        glfwTerminate();
    }

    // Create Shader Program
    void create_shader(const char* vertexShaderFilename, const char* fragShaderFilename) {
        // allocate & load vertex source
        GLchar* pVertexSource = read_glsl("../../src/media/shaders/vertex.glsl");
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
        GLchar* pFragSource = read_glsl("../../src/media/shaders/frag.glsl");
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
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragShader);
        glLinkProgram(shaderProgram);
        // check if linking was successful
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            fprintf(stderr, "(OpenGL) Error, linking failed:\n%s\n", infoLog);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragShader);
        // use shader
        glUseProgram(shaderProgram);
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
            // positions                           // colors          // texture coords
            node_x+node_r,  node_y+node_r, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // top right
            node_x+node_r,  node_y-node_r, 0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // bottom right
            node_x-node_r,  node_y-node_r, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // bottom left
            node_x-node_r,  node_y+node_r, 0.0f,   1.0f, 1.0f, 0.0f,  0.0f, 1.0f   // top left
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
        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
    }

    // Display Scene
    void render(void) {
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures to texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        // activate shader
        glUseProgram(shaderProgram);
        // transformation matrices
        glm::mat4 view  = glm::mat4(1.0f);
        glm::mat4 proj  = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(camera.x, camera.y, -32.0f + camera.z));
        //proj = glm::ortho(0.0f, (GLfloat)graphics::scr_width, 0.0f, (GLfloat)graphics::scr_height, 0.1f, 100.0f);
        proj = glm::perspective(glm::radians(45.0f), (float)scr_width/(float)scr_height, 0.1f, 100.f);
        // get their uniform locations
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(shaderProgram, "proj");
        // pass to shaders
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        GLuint selectLoc = glGetUniformLocation(shaderProgram, "selection");

        // bind vertices of unit quad (+/-1,+/-1)
        glBindVertexArray(VAO);

        // Render wires
        GLint selection = 2;
        glUniform1i(selectLoc, selection);
        for(const auto& elem: graph1->edges) {
            auto node1 = graph1->nodes[elem.first];
            auto node2 = graph1->nodes[elem.second];
            float x1 = node1->properties->x;
            float y1 = node1->properties->y;
            float x2 = node2->properties->x;
            float y2 = node2->properties->y;
            float dist = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
            // set model matrix
            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 edgePosition{ (x1+x2)/2.f, (y1+y2)/2.f, 0.f };
            float theta = atan((y2-y1)/(x2-x1));
            model = glm::translate(model, edgePosition);
            model = glm::rotate(model, theta, glm::vec3(0.f,0.f,1.f));
            model = glm::scale(model, glm::vec3(dist/2.f, 1.f, 1.f));
            GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // draw edge
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // Render Nodes
        selection = 1;
        glUniform1i(selectLoc, selection);
        for (uint n = 0; n < graph1->nodes.size(); ++n) {
            // set model matrix
            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 nodePosition{ graph1->nodes[n]->properties->x, graph1->nodes[n]->properties->y, 0.0f };
            model = glm::translate(model, nodePosition);
            GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // set color
            GLuint colorLoc = glGetUniformLocation(shaderProgram, "nodeColor");
            glm::vec3 green{ 0.0f, 1.0f, 0.0f };
            glm::vec3 red{ 1.0f, 0.0f, 0.0f };
            glm::vec3 nodeColor = graph1->nodes[n]->properties->opinion?green:red;
            glUniform3fv(colorLoc, 1, glm::value_ptr(nodeColor));
            // draw node
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

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