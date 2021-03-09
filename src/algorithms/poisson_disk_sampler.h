#ifndef POISSON_DISK_SAMPLER_H
#define POISSON_DISK_SAMPLER_H


#include <vector>
#include <unordered_set>
#include <math.h>

#include "../types.h"
#include "../random.h"
#include "../data_structures/matrix.h"

template<typename T>
struct vec2 {
    T x, y;
};

namespace {

    template<typename T>
    class RandomQueue {
    public:
        RandomQueue(void) {}
        RandomQueue(uint n) { queue.reserve(n); }

        void insert(const T& elem) {
            elements.push_back(elem);
        }

        T pop(void) {
            static std::uniform_int_distribution dist(0, (int) elements.size() - 1);

            // Get random element.
            int idx = dist(rng::generator); 
            T elem = elements[idx];

            // Remove element.
            std::swap(elements[idx], elements.back());
            elements.pop_back();

            return elem;
        }

        bool empty(void) { return elements.empty(); }
    
    private:
        std::vector<T> elements;
    };

    // Map a 2d point to its containing grid cell.
    vec2<int> get_grid_coord(const vec2<float>& point, const float cell_size) {
        return vec2<int> { 
            (int)(point.x / cell_size),
            (int)(point.y / cell_size) };
    }

    // Sample a random point in an annulus around the current point x0, with bias towards the inner
    // radius to more tightly pack points.
    vec2<float> sample_point(const vec2<float>& p, const float min_dist) {
        static std::uniform_real_distribution dist(0.0f, 1.0f);
        static float pi = std::atanf(1) * 4;

        float r1 = dist(rng::generator);
        float r2 = dist(rng::generator);

        float radius = min_dist * (r1 + r2);
        float angle = 2.0f * pi * r2;

        return vec2<float>{ 
            p.x + radius * std::cosf(angle), 
            p.y + radius * std::sinf(angle) };
    }

    std::vector<int> get_neighborhood(const Matrix<int>& grid, const vec2<int>& coord) {
        int w1 = std::max<int>(coord.x - 2, 0);
        int w2 = std::min<int>(coord.x + 2, grid.cols - 1);
        int h1 = std::max<int>(coord.y - 2, 0);
        int h2 = std::min<int>(coord.y + 2, grid.rows - 1);

        std::vector<int> neighborhood;
        for (size_t h = h1; h <= h2; ++h) 
            for (size_t w = w1; w <= w2; ++w) 
                if (grid[h][w] > -1) 
                    // Only add cell if it actually contains a point.
                    neighborhood.push_back(grid[h][w]);

        return neighborhood;
    }

    float distance(vec2<float> u, vec2<float> v) {
        float dx = u.x - v.x;
        float dy = u.y - v.y;
        return std::sqrt(dx*dx + dy*dy);
    }

    bool in_neighborhood(
        const vec2<float>& query_point, 
        const float min_dist, 
        const std::vector<vec2<float>>& points, 
        const Matrix<int>& grid, 
        const float cell_size) 
    {
        vec2<int> coord = get_grid_coord(query_point, cell_size);

        // Check cells around point.
        std::vector<int> neighborhood = get_neighborhood(grid, coord);
        for (const auto& neighbor_idx : neighborhood) {
            if (distance(query_point, points[neighbor_idx]) < min_dist)
                return true;
        }
        return false;
    }

    bool in_grid(
        const vec2<float>& query_point,
        const Matrix<int>& grid,
        const float cell_size)
    {
        vec2<int> coord = get_grid_coord(query_point, cell_size);
        if (coord.x < 0 || coord.y < 0 || coord.x >= grid.cols || coord.y >= grid.rows) {
            return false;
        }
        return true;
    }

} // namespace

void print_point(const vec2<float>& point) {
    std::cout << point.x << " " << point.y << std::endl;
}

#include <iostream>
std::vector<vec2<float>> generate_poisson_disk(
    const uint height,
    const uint width,
    const float min_dist,
    const uint max_candidate_points = 30,
    const uint max_samples = 100)
{
    const float cell_size = min_dist / std::sqrtf(2.0f);
    const uint grid_height = (uint) std::ceil((float) height / cell_size);
    const uint grid_width = (uint) std::ceil((float) width / cell_size);

    // Fill grid cells with -1 to indicate unoccupied.
    Matrix<int> grid(grid_height, grid_width);
    for (size_t i = 0; i < grid.rows; ++i)
        for (size_t k = 0; k < grid.cols; ++k)
            grid[i][k] = -1;

    RandomQueue<vec2<float>> process_queue;  // Like queue, but pop op grabs random element rather than front.
    std::vector<vec2<float>> sample_points;

    // Generate a random first point inside the grid.
    std::uniform_real_distribution dist_h(0.0f, (float) height);
    std::uniform_real_distribution dist_w(0.0f, (float) width);
    vec2<float> p1{ 
        dist_h(rng::generator),
        dist_w(rng::generator) };
    assert( in_grid(p1, grid, cell_size) );
    
    process_queue.insert(p1);
    vec2<int> grid_idx = get_grid_coord(p1, cell_size);
    grid[grid_idx.y][grid_idx.x] = sample_points.size();
    sample_points.push_back(p1);

    while ( !process_queue.empty() && sample_points.size() < max_samples ) {
        vec2<float> point = process_queue.pop();
        for (int i = 0; i < max_candidate_points; ++i) {
            vec2<float> new_point = sample_point(point, min_dist);
            if ( in_grid(new_point, grid, cell_size) && 
                 ! in_neighborhood(new_point, min_dist, sample_points, grid, cell_size) ) {
                grid_idx = get_grid_coord(new_point, cell_size);

                // @FIXME Why the fuck does this happen?
                if ( grid[grid_idx.y][grid_idx.x] != -1 ) continue;

                grid[grid_idx.y][grid_idx.x] = sample_points.size();
                sample_points.push_back(new_point);
                process_queue.insert(new_point);
            }
        }
    }

    // for (size_t i = 0; i < sample_points.size(); ++i) {
    //     // std::cout << i << ": ";
    //     // print_point(sample_points[i]);
    //     for (size_t k = i + 1; k < sample_points.size(); ++k) {
    //         float d = distance(sample_points[i], sample_points[k]);
    //         assert( d > min_dist );
    //         // if (d < min_dist) {
    //         //     std::cout << i << ", " << k << ": " << d << std::endl;
    //         // }
    //     }
    // }
    // std::cout << sample_points.size() << std::endl;

    sample_points.shrink_to_fit();
    return sample_points;
}


#endif
