#ifndef VOTER_MODEL
#define VOTER_MODEL


#include <stdlib.h>
#include <random>
#include <tuple>

#include "../types.h"
#include "../random.h"  // rng::
#include "../data_structures/graph.h"  // graph::

static std::bernoulli_distribution bernoulli_dist(0.5);

// forward declarations
void init_graph_opinions(graph::Graph* graph);
std::pair<graph::Node*, graph::Node*> sample_nodes(const graph::Graph* graph);
void step_dynamics(std::pair<graph::Node*, graph::Node*>& node_pair);
bool is_consensus_reached(graph::Graph* graph);

// Implementations
void init_graph_opinions(graph::Graph* graph) {
    for (uint i = 0; i < graph->num_nodes; ++i) {
        graph->nodes[i]->properties->opinion = bernoulli_dist(rng::generator);
    }
}

std::pair<graph::Node*, graph::Node*>
sample_nodes(const graph::Graph* graph) {
    if (! graph->num_edges) {
        throw "Graph has no edges!";
    }

    // randomly select a node and neighbor
    auto node = graph->nodes[rand() % graph->num_nodes];
    while (! node->num_adjacent) {
        node = graph->nodes[rand() % graph->num_nodes];
    }
    auto neighbor = graph->nodes[ node->adjacent[rand() % node->num_adjacent] ];

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

    for (uint i = 0; i < graph->num_nodes; ++i) {
        if (graph->nodes[i]->properties->opinion != opinion) {
            return false;
        }
    }
    return true;
}


#endif
