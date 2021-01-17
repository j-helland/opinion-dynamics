#include <stdio.h>
#include <assert.h>

#include "types.h"
#include "data_structures/graph.h"

#define TEST_SIZE (5)

static void match_dest(graph::Graph graph, uint source, uint dest, void* data) {
    assert( data && dest == *((uint*) data));
}

int main(void) {
    graph::Graph graph = graph::make(TEST_SIZE);
    assert( graph->num_nodes == TEST_SIZE );

    uint n, k;

    // check that all nodes exist
    for (n = 0; n < TEST_SIZE; ++n) {
        assert( graph::has_node(graph, n) == 1 );
    }

    // check that it's empty
    printf("Empty check has_edge");
    for (n = 0; n < TEST_SIZE; ++n) {
        for (k = 0; k < TEST_SIZE; ++k) {
            assert( graph::has_edge(graph, n, k) == 0 );
        }
    }

    // another empty check
    printf("Empty check degree, foreach match_dest");
    for (n = 0; n < TEST_SIZE; ++n) {
        assert( graph::degree(graph, n) == 0, "degree must be zero" );
        graph::foreach(graph, n, match_dest, 0);
    }

    // make sure no edges somehow exist
    assert( graph->num_edges == 0 );

    // add self-edges to all nodes
    for (n = 0; n < TEST_SIZE; ++n) {
        graph::add_edge(graph, n, n);
    }
    assert( graph->num_edges == TEST_SIZE );
    for (n = 0; n < TEST_SIZE; ++n) {
        for (k = 0; k < TEST_SIZE; ++k) {
            assert( graph::has_edge(graph, n, k) == (n == k) );
        }
    }
    for (n = 0; n < TEST_SIZE; ++n) {
        assert( graph::degree(graph, n) == 1 );
        graph::foreach(graph, n, match_dest, &n);
    }

    // complete the graph
    for (n = 0; n < TEST_SIZE; ++n) {
        for (k = 0; k < TEST_SIZE; ++k) {
            graph::add_edge(graph, n, k);
        }
    }
    assert( graph->num_edges == TEST_SIZE * TEST_SIZE );
    for (n = 0; n < TEST_SIZE; ++n) {
        assert( graph::degree(graph, n) == TEST_SIZE );
        for (k = 0; k < TEST_SIZE; ++k) {
            assert( graph::has_edge(graph, n, k) == 1 );
        }
    }

    // free the graph
    graph::destroy(graph);

    return 0;
}