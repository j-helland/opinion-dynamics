#ifndef GRAPH_SERIALIZATION_TEST
#define GRAPH_SERIALIZATION_TEST


#include "../types.h"
#include "../data_structures/graph.h"
#include "../core/entity_manager.h"

#ifndef TEST_SIZE
    #define TEST_SIZE (64)
#endif

int graph_serialization_test(void) {
    auto* graph = new graph::Graph;
    graph::make(graph, TEST_SIZE);
    for (const auto& [id1, _] : graph->nodes) {
        for (const auto& [id2, _] : graph->nodes) {
            if (id1 == id2) continue;
            graph::add_edge(graph, id1, id2);
        }
    }

    graph::save_graph(graph, "../../data/graph.json");
    graph::destroy(graph);
    core::clear_all_entities();

    return 0;
}


#endif