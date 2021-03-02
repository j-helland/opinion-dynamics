#ifndef GRAPH_TEST
#define GRAPH_TEST


#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "../types.h"
#include "../random.h"
#include "../data_structures/graph.h"
#include "../core/entity_manager.h"
#include "../algorithms/traversal.h"

#define TEST_SIZE (8)

void print_edges(graph::Graph* graph) {
    for (const auto& edge : graph->edges) {
        std::cout << "(" << edge.first << ", " << edge.second << ")\n";
    }
}

int graph_test(void) {
    graph::Graph* graph = new graph::Graph;
    graph::make(graph, TEST_SIZE);
    assert( graph->nodes.size() == TEST_SIZE );

    uint n, k;
    n = k = 0;

    // Check that all nodes exist.
    for (const auto & [id, _] : graph->nodes) 
        assert( graph::has_node(graph, id) );

    // Make sure no edges exist.
    assert( graph->edges.empty() );

    for (const auto& [id, _] : graph->nodes) {
        graph::Node* node = core::get_entity<graph::Node>(id);
        assert( node );
        assert( (node->x == 0.f) && (node->y == 0.f) );
    }

    // arrange all nodes in a main diagonal line
    for (const auto& [id, _] : graph->nodes) {
        graph::Node* node = core::get_entity<graph::Node>(id);
        assert( node );
        node->x = node->y = (float) n;
        n++;
    }

    // add self-edges to all nodes
    for (const auto& [id, _] : graph->nodes) {
        graph::add_edge(graph, id, id);
    }

    // // Delete an individual edge. NOTE: will trigger another assertion failure further down. 
    // graph::remove_edge(graph, std::make_pair(0, 0));
    // assert( graph->edges.find(std::make_pair(0, 0)) == graph->edges.end() );
    // assert( graph->nodes[0].find(0) == graph->nodes[0].end() );
    // return 0;

    // // Delete all edges
    // graph::clear_edges(graph);
    // assert( graph->edges.size() == 0 );
    // for ( const auto& [_, adj] : graph->nodes )
    //     assert( adj.size() == 0 );
    // return 0;

    assert( graph->edges.size() == TEST_SIZE );
    for (const auto& [id, _] : graph->nodes) {
        graph::Node* node = core::get_entity<graph::Node>(id);
        assert( graph->nodes[id].size() == 1 );
    }
    for (const auto& [id1, _] : graph->nodes) {
        for (const auto& [id2, _] : graph->nodes) {
            if (id1 == id2) assert( graph::has_edge(graph, id1, id2) );
            // assert( graph::has_edge(graph, id1, id2) == (n == k) );
        }
    }

    // complete the graph
    std::bernoulli_distribution dist(0.5);
    for (const auto& [id1, _] : graph->nodes) {
        for (const auto& [id2, _] : graph->nodes) {
            if (dist(rng::generator)) {
                graph::add_edge(graph, id1, id2);
            }
        }
    }

    // run bfs and accumulate nodes in bfs ordering using dat void* cast (this feels wrong)
    //printf("\nRunning BFS\n");
    // std::vector<uint> ordering;
    // graph::bfs(graph, 0, graph::accumulate_nodes, (void*) &ordering);
    // for (auto n : ordering) {
    //     //printf("node: %i\n", n);
    // }

    // run dfs and accumulate nodes in bfs ordering using dat void* cast (this feels wrong)
    //printf("\nRunning DFS\n");
    // ordering.clear();
    // graph::dfs(graph, 0, graph::accumulate_nodes, (void*) &ordering);
    // for (auto n : ordering) {
    //     //printf("node: %i\n", n);
    // }

    // free the graph
    graph::destroy(graph);
    core::clear_all_entities();

    return 0;
}


#endif