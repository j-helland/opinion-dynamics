#ifndef VOTER_MODEL
#define VOTER_MODEL


#include <stdlib.h>
#include <random>
#include <tuple>

#include "../../random.h"  // rng::
#include "../../data_structures/graph.h"  // graph::

// Sample a pair of nodes by randomly sampling an edge.
void
step_voter_dynamics(graph::Graph* graph, graph::edge_ptr_t& edge) {
    if (edge.first == nullptr || edge.second == nullptr) return;

    bool opinion1 = edge.first->properties->opinion;
    bool opinion2 = edge.second->properties->opinion;

    // if the opinions differ, then target changes its opinion to that of its selected neighbor
    if (opinion1 != opinion2) {
        edge.first->properties->opinion = opinion2;
    }
}


#endif
