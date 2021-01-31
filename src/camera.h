#ifndef CAMERA_H

#include <glm/glm.hpp>

struct Camera {
    glm::vec3 pos{ 0.0f, 0.0f, 20.0f };
    glm::vec3 front{ 0.0f, 0.0f, -1.0f };
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
};

#define CAMERA_H
#endif