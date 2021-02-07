#ifndef SZNAJD_MODEL_TEST
#define SZNAJD_MODEL_TEST


#include <stdio.h>
#include <assert.h>

#include "../types.h"
#include "../data_structures/graph.h"
#include "../dynamics/models/sznajd.h"
#include "../dynamics/utils.h"

#ifndef TEST_SIZE
    #define TEST_SIZE (64)
#endif
#define TEST_SIMULATION_STEPS (100)

int sznajd_model_test(void) {
    auto* graph = new graph::Graph;
    graph::make(graph, TEST_SIZE);
    init_graph_opinions(graph);  // uniform-random opinions
    for (const auto& [id1, _] : graph->nodes) {
        for (const auto& [id2, _] : graph->nodes) {
            if (id1 == id2) continue;
            graph::add_edge(graph, id1, id2);
        }
    }

    // make sure that a single step works properly
    auto edge = sample_edge(graph);
    step_sznajd_dynamics(graph, edge);
    // auto n1 = core::get_entity<graph::Node>(edge.first);
    // auto n2 = core::get_entity<graph::Node>(edge.second);
    // assert( n1->opinion == n2->opinion );

    graph::destroy(graph);
    core::clear_all_entities();

    return 0;
}


#endif