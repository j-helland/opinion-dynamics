/*
An adjacency list implementation of a directed graph data structure.
Goal is just to get something quick and dirty working here, can optimize / refactor later.

TODO: graph is almost certainly not memory-efficient. Try to allocate contiguously wrt to spatial adjacency?
*/
#include <stdlib.h>
#include <assert.h>

#include "../types.h"

namespace graph {
    //// Types
    typedef struct graph* Graph;
    typedef struct node* Node;
    typedef struct properties* Properties;

    //// forward declarations
    // structs
    struct properties;
    struct node;
    struct graph;

    // functions
    Graph make(uint, bool);
    void destroy(Graph);
    bool has_node(const Graph, uint);
    int degree(const Graph, uint);
    static int intcmp(const void*, const void*);
    int has_edge(const Graph, uint, uint);
    void add_edge(Graph, uint, uint);
    void foreach(Graph graph, uint source, void (*f) (Graph graph, uint source, uint dest, void* data), void* data);


    //// Implementations
    struct properties {
        float x, y;
    };

    // Graph node with adjacency list.
    struct node {
        uint num_adjacent;
        uint num_slots;  // number of array slots
        Properties properties;
        char is_sorted;  // true if list is sorted
                         // NOTE: all types except char require alignment since char is 1 byte (cf. http://www.catb.org/esr/structure-packing/)
        uint adjacent[1];  // adjacency list 
    };

    // Basic graph struct with list of node structs.
    struct graph {
        uint num_nodes;
        uint num_edges;
        char is_undirected;

        Node nodes[1];
    };

    // Create a graph with num_nodes vertices, no edges.
    // Graph is heap-allocated via malloc.
    Graph 
    make(uint num_nodes, bool undirected = false) {
        // this ain't fuckin C, homie -- explicit cast that void*
        Graph graph = (Graph) malloc(sizeof(struct graph) + sizeof(Node) * (num_nodes - 1));
        assert(graph);

        graph->num_nodes = num_nodes;
        graph->num_edges = 0;

        for (uint i = 0; i < num_nodes; ++i) {
            graph->nodes[i] = (Node) malloc(sizeof(struct node));
            assert(graph->nodes[i]);

            // initialize all spatial locations as 0.f
            // TODO: allow uninitialized?
            graph->nodes[i]->properties = (Properties) malloc(sizeof(struct properties));
            assert(graph->nodes[i]->properties);
            graph->nodes[i]->properties->x 
                = graph->nodes[i]->properties->y 
                = 0.f;

            graph->nodes[i]->num_adjacent = 0;
            graph->nodes[i]->num_slots = 1;  
            graph->nodes[i]->is_sorted = 1;  // we initialize the adjacency lists in sorted order trivially
        }

        graph->is_undirected = (char) undirected;

        return graph;
    }

    // Free the heap-allocated memory for a graph struct.
    void 
    destroy(Graph graph) {
        for (uint i = 0; i < graph->num_nodes; ++i) {
            free(graph->nodes[i]->properties);
            free(graph->nodes[i]);
        }
        free(graph);
    }
        
    bool 
    has_node(const Graph graph, uint node) {
        return (node < graph->num_nodes);
    }
    
    // Get count of adjacent nodes to a query node that exists in the graph.
    int 
    degree(const Graph graph, uint node) {
        assert( has_node(graph, node) == 1 );
        return graph->nodes[node]->num_adjacent;
    }

    #define BSEARCH_THRESHOLD (10)

    static int 
    intcmp(const void* a, const void* b) {
        return *((const int*) a) - *((const int*) b);
    }

    // Return 1 if edge (source, dest) exists, 0 otherwise.
    // O(n log n) on first call due to possible sorting, but subsequent calls without the addition of edges will be faster.
    int 
    has_edge(const Graph graph, uint source, uint dest) {
        assert( has_node(graph, source) == 1 );
        assert( has_node(graph, dest) == 1 );

        // If the node has a relatively high degree, do a faster typically O(log n) search.
        // In this case, it's probably worth taking the rare O(n log n) sorting hit to be faster later.
        if (degree(graph, source) >= BSEARCH_THRESHOLD) {
            // sort
            if (! graph->nodes[source]->is_sorted) {
                qsort(graph->nodes[source]->adjacent, 
                    graph->nodes[source]->num_adjacent, 
                    sizeof(int), 
                    intcmp);
            }
            // binary search to find the node
            return 0 != bsearch(
                &dest,
                graph->nodes[source]->adjacent,
                graph->nodes[source]->num_adjacent,
                sizeof(int),
                intcmp);
        }
        // Do a linear search if the adjacency list is small to avoid running a bunch of sorting operations.
        else {
            for (uint i = 0; i < graph->nodes[source]->num_adjacent; ++i) {
                if (graph->nodes[source]->adjacent[i] == dest) {
                    return 1;
                }
            }
            return 0;
        }
    }

    // Add an edge to an existing graph.
    void 
    add_edge(Graph graph, uint u, uint v) {
        assert( has_node(graph, u) == 1 );
        assert( has_node(graph, v) == 1 );

        // early out if edge already exists
        if (has_edge(graph, u, v)) {
            return;
        }

        // grow the list by powers of 2 if we have too many edges
        while (graph->nodes[u]->num_adjacent >= graph->nodes[u]->num_slots) {
            graph->nodes[u]->num_slots *= 2;
            graph->nodes[u] = (Node) realloc(graph->nodes[u], 
                sizeof(struct node) + sizeof(int) * (graph->nodes[u]->num_slots - 1));
        }

        // add the new edge
        graph->nodes[u]->adjacent[ graph->nodes[u]->num_adjacent ] = v;
        graph->nodes[u]->is_sorted = 0;

        // increment counts
        graph->nodes[u]->num_adjacent++;
        graph->num_edges++;

        // in undirected case, call again
        if (graph->is_undirected) {
            add_edge(graph, v, u);
            graph->num_edges--;  // need to decrement b/c of symmetry
        }
    }

    // Invoke a function `f` over all edges (source, dest) with `data` supplied as the final parameter to `f`.
    // NOTE: there is no guaranteed ordering to the edges.
    void
    foreach(
        Graph graph, uint source, 
        void (*f) (Graph graph, uint source, uint dest, void* data), 
        void* data
    ) {
        assert( has_node(graph, source) );
        for (uint i = 0; i < graph->nodes[source]->num_adjacent; ++i) {
            f(graph, source, graph->nodes[source]->adjacent[i], data);
        }
    }

} // end namespace