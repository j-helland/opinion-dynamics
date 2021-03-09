#ifndef POISSON_DISK_TEST
#define POISSON_DISK_TEST


#include <iostream>
#include <math.h>

#include "../types.h"
#include "../algorithms/poisson_disk_sampler.h"

float dist(const vec2<float>& u, const vec2<float>& v) {
    float dx = u.x - v.x;
    float dy = u.y - v.y;
    return std::sqrtf(dx*dx + dy*dy);
}

int poisson_disk_test(void) {
    const uint height = 100;
    const uint width = 100;
    const float min_dist = 1.0f;

    auto points = generate_poisson_disk(height, width, min_dist);

    for (size_t i = 0; i < points.size(); ++i)
        for (size_t k = i + 1; k < points.size(); ++k)
            assert( dist(points[i], points[k]) > min_dist );

    return 0;
}


#endif