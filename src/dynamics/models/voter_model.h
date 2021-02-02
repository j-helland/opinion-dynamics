#ifndef VOTER_MODEL
#define VOTER_MODEL


#include <stdlib.h>
#include <random>
#include <tuple>

#include "../../random.h"  // rng::
#include "../../data_structures/graph.h"  // graph::
#include "../../core/entity_manager.h"  // core::

// Sample a pair of nodes by randomly sampling an edge.
void step_voter_dynamics(graph::Graph* graph, graph::edge_t& edge) {
    graph::Node* n1 = core::get_entity<graph::Node>(edge.first);
    graph::Node* n2 = core::get_entity<graph::Node>(edge.second);
    if ( n1 == nullptr || n2 == nullptr ) return;

    bool opinion1 = n1->opinion;
    bool opinion2 = n2->opinion;

    // if the opinions differ, then target changes its opinion to that of its selected neighbor
    if (opinion1 != opinion2) {
        n1->opinion = opinion2;
    }
}


#endif
