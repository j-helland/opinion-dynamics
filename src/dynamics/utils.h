#ifndef UTILS
#define UTILS


#include <tuple>
#include <random>

#include "../types.h"
#include "../random.h"
#include "../data_structures/graph.h"


graph::edge_ptr_t
sample_edge(const graph::Graph* graph) {
    if (graph->edges.empty()) return std::make_pair(nullptr, nullptr);

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
    return std::make_pair( graph->nodes[edge.first], graph->nodes[edge.second] );
}

void 
init_graph_opinions(graph::Graph* graph) {
    std::bernoulli_distribution dist(0.5);

    for (uint i = 0; i < graph->nodes.size(); ++i) {
        graph->nodes[i]->properties->opinion = dist(rng::generator);
    }
}

bool
is_consensus_reached(graph::Graph* graph) {
    bool opinion = graph->nodes[0]->properties->opinion;

    for (uint i = 0; i < graph->nodes.size(); ++i) {
        if (graph->nodes[i]->properties->opinion != opinion) {
            return false;
        }
    }
    return true;
}


#endif