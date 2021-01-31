#ifndef SZNAJD 
#define SZNAJD 


#include <stdlib.h>
#include <random>
#include <tuple>
#include <vector>

#include "../../types.h"
#include "../../random.h"  // rng::
#include "../../data_structures/graph.h"  // graph::

void
step_sznajd_dynamics(graph::Graph* graph, graph::edge_ptr_t& edge) {
    if (edge.first == nullptr || edge.second == nullptr) return;

    bool opinion1 = edge.first->properties->opinion;
    bool opinion2 = edge.second->properties->opinion;

    if (opinion1 == opinion2) {
        // All neighbors take this opinion.
        for (uint n = 0; n < edge.first->num_adjacent; ++n) {
            // TODO: this way of getting the neighbor is fucking stupid
            graph->nodes[ edge.first->adjacent[n] ]->properties->opinion = opinion1;
        }
        for (uint n = 0; n < edge.second->num_adjacent; ++n) {
            graph->nodes[ edge.second->adjacent[n] ]->properties->opinion = opinion1;
        }
    } else {
        // Neighbors take corresponding opinions.
        for (uint n = 0; n < edge.first->num_adjacent; ++n) {
            if (graph->nodes[n] == edge.second) continue;
            graph->nodes[ edge.first->adjacent[n] ]->properties->opinion = opinion1;
        }
        for (uint n = 0; n < edge.second->num_adjacent; ++n) {
            if (graph->nodes[n] == edge.first) continue;
            graph->nodes[ edge.second->adjacent[n] ]->properties->opinion = opinion2;
        }
    }
}

#endif