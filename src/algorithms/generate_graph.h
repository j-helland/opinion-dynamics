#ifndef GENERATE_GRAPH_H
#define GENERATE_GRAPH_H

#include <limits>
#include <math.h>

#include "../core/entity_manager.h"
#include "../data_structures/graph.h"
#include "../data_structures/matrix.h"
#include "../random.h"
#include "../types.h"

namespace alg {

    // namespace {
    //     template<typename T>
    //     typedef struct vec2 {
    //         T x, y;
    //     };
    // }

    void generate_positions(graph::Graph* graph) {
        float radius = 160.f;
        for (const auto& [id, _] : graph->nodes) {
            graph::Node* node = core::get_entity<graph::Node>(id);
            node->x = (500 - (float)(rand() % 1000)) / 1000.f * radius;
            node->y = (500 - (float)(rand() % 1000)) / 1000.f * radius;
        }
    }

    void generate_edges(graph::Graph* graph, const float threshold) {
        std::vector<core::id_t> idx_to_id(graph->nodes.size());
        uint count = 0;
        for (const auto& [id, _] : graph->nodes) {
            idx_to_id[count++] = id;
        }

        // Compute the l2 distance matrix.
        Matrix<float> dists(graph->nodes.size(), graph->nodes.size());
        // float max_dist = std::numeric_limits<float>::infinity();
        float max_dist = 0.f;
        std::bernoulli_distribution edge_distr(0.5f);
        uint inf = 0xFFU << 23;
        for (uint i = 0; i < dists.rows; ++i) {
            graph::Node* n1 = core::get_entity<graph::Node>(idx_to_id[i]);
            dists[i][i] = *(float*) &inf;
            assert( isinf( dists[i][i] ) );
            for (uint k = i + 1; k < dists.cols; ++k) {
                graph::Node* n2 = core::get_entity<graph::Node>(idx_to_id[k]);

                // Compute l2 distance.
                float xd = n1->x - n2->x;
                float yd = n1->y - n2->y;
                float d = sqrtf( xd*xd+ yd*yd );
                dists[i][k] = dists[k][i] = d;

                if ( d > max_dist )
                    max_dist = d;
            }
        }

        // for (size_t i = 0; i < dists.rows; ++i) {
        //     for (size_t k = 0; k < dists.cols; ++k) {
        //         std::cout << dists[i][k] / max_dist << " ";
        //     }
        //     std::cout << std::endl;
        // }
        // std::cout << "MAX_DIST " << max_dist << std::endl;

        for (uint i = 0; i < dists.rows; ++i) {
            for (uint k = i + 1; k < dists.cols; ++k) {
                // For now, just check the threshold here.
                if ( (dists[i][k] / max_dist) < threshold ) {
                    bool b = edge_distr(rng::generator);
                    graph::add_edge(
                        graph, 
                        b ? idx_to_id[i] : idx_to_id[k],
                        !b ? idx_to_id[i] : idx_to_id[k]);
                }
            }
        }
    }

    graph::Graph* generate_graph(const uint num_nodes, const float threshold) {
        graph::Graph* graph = new graph::Graph;
        graph = graph::make(graph, num_nodes);

        generate_positions(graph);
        generate_edges(graph, threshold);
        init_graph_opinions(graph);

        return graph;
    }

}


#endif