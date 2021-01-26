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

std::pair<graph::Node*, graph::Node*>
sample_nodes(const graph::Graph* graph, float prob = 0.5) {
    static std::binomial_distribution dist( (uint) graph->nodes.size() - 1, prob);

    if (! graph->edges.size()) {
        throw "Graph has no edges!";
    }

    // randomly select a node and neighbor
    auto node = graph->nodes[dist(rng::generator)];
    while (! node->num_adjacent) {
        node = graph->nodes[dist(rng::generator)];
    }
    dist = std::binomial_distribution(node->num_adjacent - 1, prob);  // need to reset the least upper bound
    auto neighbor = graph->nodes[ node->adjacent[dist(rng::generator)] ];

    return std::make_pair(node, neighbor);
}

void
step_dynamics(std::pair<graph::Node*, graph::Node*>& node_pair) {
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
