#include <stdio.h>
#include <assert.h>

#include "../types.h"
#include "../data_structures/graph.h"
#include "../models/voter_model.h"

#define TEST_SIZE (5)
#define TEST_SIMULATION_STEPS (100)

void print_network(const graph::Graph* graph) {
    printf("Opinions:\n");
    for (uint i = 0; i < graph->nodes.size(); ++i) {
        printf("\tNode %i, opinion %i\n", i, graph->nodes[i]->properties->opinion);
    }
}

int main(void) {
    auto graph = graph::make(TEST_SIZE);  // undirected graph
    init_graph_opinions(graph);  // uniform-random opinions
    // add edges
    for (uint n = 0; n < graph->nodes.size(); ++n) {
        for (uint k = 0; k < graph->nodes.size(); ++k) {
            if (n == k) continue;
            graph::add_edge(graph, n, k);
        }
    }
    print_network(graph);

    // make sure that a single step works properly
    printf("\nChecking single step of voter model...\n");
    auto pair = sample_nodes(graph);
    printf("\tOpinion target: %i | Opinion neighbor: %i\n", 
        pair.first->properties->opinion,
        pair.second->properties->opinion);
    step_dynamics(pair);
    printf("\tOpinion target: %i | Opinion neighbor: %i\n\n", 
        pair.first->properties->opinion,
        pair.second->properties->opinion);
    assert( pair.first->properties->opinion == pair.second->properties->opinion );

    // run simulation to consensus
    printf("Running simulation for %i steps...\n", TEST_SIMULATION_STEPS);
    for (uint step = 0; step < TEST_SIMULATION_STEPS; ++step) {
        step_dynamics(sample_nodes(graph));

        if (is_consensus_reached(graph)) {
            printf("Consensus reached in %i steps!\n", step + 1);
            break;
        }
    }
    if (! is_consensus_reached(graph)) {
        printf("Consensus was not reached in %i steps.\n", TEST_SIMULATION_STEPS);
    }
    print_network(graph);

    return 0;
}
