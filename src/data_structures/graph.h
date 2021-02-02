/*
An adjacency list implementation of a directed graph data structure.
Goal is just to get something quick and dirty working here, can optimize / refactor later.

TODO: graph is almost certainly not memory-efficient. Try to allocate contiguously wrt to spatial adjacency?
*/
#ifndef GRAPH
#define GRAPH


#include <stdlib.h>
#include <assert.h>
#include <unordered_set>
#include <tuple>
#include <vector>

#include "../types.h"
#include "../core/entity_manager.h"

namespace graph {
    //// Types
    typedef struct graph Graph;
    typedef struct node Node;
    typedef struct properties Properties;
    typedef std::pair<core::id_t, core::id_t> edge_t;

    // Graph node with adjacency list.
    struct node {
        float x, y;
        bool opinion;
    };

    // Basic graph struct with list of node structs.
    struct edge_hash {
        inline std::size_t operator()(const edge_t& p) const {
            return std::hash<core::id_t>()(p.first) ^ std::hash<core::id_t>()(p.second);
        }
    };
    struct graph {
        std::unordered_map<core::id_t, std::unordered_set<core::id_t>> nodes;  // nodes w/ adjacency lists
        std::unordered_set<edge_t, edge_hash> edges;
    };

    // Allocate memory for a node and its properties, then register the node in the global entities pool and return its generated id.
    core::id_t create_node(void) {
        Node* node = (Node*) malloc(sizeof(Node));
        assert(node);

        core::id_t id = core::register_entity(node);
        return id;
    }

    // Create a graph with num_nodes vertices, no edges.
    // Graph must be heap-allocated via malloc.
    Graph* make(Graph* graph, uint num_nodes) {
        assert(graph);

        graph->nodes.reserve(num_nodes);
        for (uint i = 0; i < num_nodes; ++i) {
            auto id = create_node();
            graph->nodes[id] = std::unordered_set<core::id_t>{};
            Node* node = core::get_entity<Node>(id);
            assert( node );

            node->x = node->y = 0.f;
            node->opinion = 0;
        }
        return graph;
    }

    void destroy_node(core::id_t id) {
        Node* node = core::get_entity<Node>(id);
        assert( node );
        free(node);
    }

    // Free the heap-allocated memory for a graph struct.
    // NOTE: This will call `delete graph` -- you should only try this with a heap-allocated graph struct.
    void destroy(Graph* graph) {
        std::vector<core::id_t> nodes_to_delete;
        for (const auto& [id, _] : graph->nodes) {
            nodes_to_delete.push_back(id);
        }
        for (auto id : nodes_to_delete) {
            destroy_node(id);
            graph->nodes.erase(id);
        }
        delete graph;
    }
        
    bool has_node(const Graph* graph, const core::id_t node_id) {
        return ( graph->nodes.find(node_id) != graph->nodes.end() );
    }
    
    // Get count of adjacent nodes to a query node that exists in the graph.
    int degree(const Graph* graph, const core::id_t node_id) {
        assert( has_node(graph, node_id) );
        return graph->nodes.at(node_id).size();
    }

    // Return 1 if edge (source, dest) exists, 0 otherwise.
    // O(n log n) on first call due to possible sorting, but subsequent calls without the addition of edges will be faster.
    // int has_edge(const Graph* graph, uint source, uint dest) {
    bool has_edge(const Graph* graph, const core::id_t source, const core::id_t dest) {
        assert( has_node(graph, source) );
        assert( has_node(graph, dest) );
        return ( graph->nodes.at(source).find(dest) != graph->nodes.at(source).end() );
    }

    // Add an edge to an existing graph.
    void add_edge(Graph* graph, core::id_t u, core::id_t v) {
        assert( has_node(graph, u) );
        assert( has_node(graph, v) );

        // add the new edge
        graph->nodes[u].insert(v);
        graph->edges.insert(std::make_pair(u, v));
    }

} // end namespace


#endif