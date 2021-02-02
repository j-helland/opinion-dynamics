#ifndef UTILS
#define UTILS


#include <tuple>
#include <random>

#include "../types.h"
#include "../random.h"
#include "../data_structures/graph.h"


graph::edge_t
sample_edge(const graph::Graph* graph) {
    if (graph->edges.empty()) return std::make_pair(UINT_MAX, UINT_MAX);

    static std::uniform_int_distribution dist( 0, (int) graph->edges.size() - 1 );

    // TODO: O(n) search for edge is inefficient.
    graph::edge_t edge;
    int idx = dist(rng::generator);
    int count = 0;
    for (auto e : graph->edges) {
        if (idx == count) {
            edge = e;
            break;
        }
        count++;
    }
    return std::make_pair( edge.first, edge.second );
}

void init_graph_opinions(graph::Graph* graph) {
    std::bernoulli_distribution dist(0.5);
    for (const auto& [id, _] : graph->nodes) {
        graph::Node* node = core::get_entity<graph::Node>(id);
        node->opinion = dist(rng::generator);
    }
}

bool is_consensus_reached(graph::Graph* graph) {
    // Get an opinion in the graph.
    bool opinion;
    for (const auto& [id, _] : graph->nodes) {
        opinion = core::get_entity<graph::Node>(id)->opinion;
    }
    // Check that all nodes share this opinion.
    for (const auto& [id, _] : graph->nodes) {
        graph::Node* node = core::get_entity<graph::Node>(id);
        if (node->opinion != opinion) {
            return false;
        }
    }
    return true;
}


#endif