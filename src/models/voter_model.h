#ifndef VOTER_MODEL
#define VOTER_MODEL


#include <stdlib.h>
#include <random>
#include <tuple>

#include "../types.h"
#include "../random.h"  // rng::
#include "../data_structures/graph.h"  // graph::

// forward declarations
void init_graph_opinions(graph::Graph* graph);
std::pair<graph::Node*, graph::Node*> sample_nodes(const graph::Graph* graph, float prob);
void step_dynamics(std::pair<graph::Node*, graph::Node*>& node_pair);
bool is_consensus_reached(graph::Graph* graph);

// Implementations
void init_graph_opinions(graph::Graph* graph) {
    std::bernoulli_distribution dist(0.5);

    for (uint i = 0; i < graph->nodes.size(); ++i) {
        graph->nodes[i]->properties->opinion = dist(rng::generator);
    }
}

// Sample a pair of nodes by randomly sampling an edge.
std::pair<graph::Node*, graph::Node*>
sample_nodes(const graph::Graph* graph, float prob = 0.5) {
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
step_dynamics(std::pair<graph::Node*, graph::Node*>& node_pair) {
    if (node_pair.first == nullptr || node_pair.second == nullptr) return;

    bool opinion1 = node_pair.first->properties->opinion;
    bool opinion2 = node_pair.second->properties->opinion;

    // if the opinions differ, then target changes its opinion to that of its selected neighbor
    if (opinion1 != opinion2) {
        node_pair.first->properties->opinion = opinion2;
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
