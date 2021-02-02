#ifndef SZNAJD 
#define SZNAJD 


#include <stdlib.h>
#include <random>
#include <tuple>
#include <vector>

#include "../../types.h"
#include "../../random.h"  // rng::
#include "../../data_structures/graph.h"  // graph::
#include "../../core/entity_manager.h"  // core::

void step_sznajd_dynamics(graph::Graph* graph, graph::edge_t& edge) {
    graph::Node* n1 = core::get_entity<graph::Node>(edge.first);
    graph::Node* n2 = core::get_entity<graph::Node>(edge.second);
    if ( n1 == nullptr || n2 == nullptr ) return;
    // if (edge.first == nullptr || edge.second == nullptr) return;

    bool opinion1 = n1->opinion;
    bool opinion2 = n2->opinion;

    if (opinion1 == opinion2) {
        // All neighbors take this opinion.
        // for (uint n = 0; n < edge.first->num_adjacent; ++n) {
        for (auto id : graph->nodes.at(edge.first)) {
            auto neighbor = core::get_entity<graph::Node>(id);
            neighbor->opinion = opinion1;
        }
        // for (uint n = 0; n < edge.second->num_adjacent; ++n) {
        for (auto id : graph->nodes.at(edge.second)) {
            auto neighbor = core::get_entity<graph::Node>(id);
            neighbor->opinion = opinion1;
            // graph->nodes[ edge.second->adjacent[n] ]->properties->opinion = opinion1;
        }
    } else {
        // Neighbors take corresponding opinions.
        for (auto id : graph->nodes.at(edge.first)) {
            if (id == edge.second) continue;
            auto neighbor = core::get_entity<graph::Node>(id);
            neighbor->opinion = opinion1;
        // for (uint n = 0; n < edge.first->num_adjacent; ++n) {
        //     if (graph->nodes[n] == edge.second) continue;
        //     graph->nodes[ edge.first->adjacent[n] ]->properties->opinion = opinion1;
        }
        for (auto id : graph->nodes.at(edge.second)) {
            if (id == edge.first) continue;
            auto neighbor = core::get_entity<graph::Node>(id);
            neighbor->opinion = opinion2;
        // for (uint n = 0; n < edge.second->num_adjacent; ++n) {
        //     if (graph->nodes[n] == edge.first) continue;
        //     graph->nodes[ edge.second->adjacent[n] ]->properties->opinion = opinion2;
        }
    }
}

#endif