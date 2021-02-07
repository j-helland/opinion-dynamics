#ifndef GRAPH_DESERIALIZATION_TEST
#define GRAPH_DESERIALIZATION_TEST


#include "../types.h"
#include "../data_structures/graph.h"
#include "../core/entity_manager.h"

#ifndef TEST_SIZE
    #define TEST_SIZE (64)
#endif

int graph_deserialization_test(void) {
    auto* graph = new graph::Graph;
    graph::load_graph(graph, "../../data/graph.json");

    graph::destroy(graph);
    core::clear_all_entities();

    return 0;
}


#endif