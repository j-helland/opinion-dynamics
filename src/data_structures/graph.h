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
#include <fstream>
#include <iomanip>

#include "nlohmann/json.hpp"

#include "../types.h"
#include "../utils.h"
#include "../core/entity_manager.h"

namespace graph {
    using json = nlohmann::json;

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
    core::id_t create_node(const core::id_t* _id = nullptr) {
        Node* node = (Node*) malloc(sizeof(Node));
        assert(node);

        core::id_t id = core::register_entity(node, _id);
        return id;
    }

    bool add_node(Graph* graph, core::id_t id) {
        if ( graph->nodes.find(id) != graph->nodes.end() ) 
            return false;
        graph->nodes[id] = std::unordered_set<core::id_t>{};
        return true;
    }

    // Create a graph with num_nodes vertices, no edges.
    // Graph must be heap-allocated via malloc.
    Graph* make(Graph* graph, uint num_nodes) {
        assert(graph);

        graph->nodes.reserve(num_nodes);
        for (uint i = 0; i < num_nodes; ++i) {
            // Create and register.
            auto id = create_node();
            assert( add_node(graph, id) );

            // Initialize properties of node.
            Node* node = core::get_entity<Node>(id);
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

    // Return the outgoing edges from a node.
    std::vector<edge_t> get_edges_at_node(const Graph* graph, core::id_t node) {
        assert( has_node(graph, node) );
        std::vector<edge_t> edges( graph->nodes.at(node).size() );
        for ( core::id_t neighbor : graph->nodes.at(node) ) {
            edges.push_back( std::make_pair(node, neighbor) );
        }
        return edges; // NOTE: >=c++11 has automatic move semantics for vector return
    }

    void serialize_graph(const Graph* graph, json& graph_serialized) {
        // Add the nodes
        for (const auto& [id, _] : graph->nodes) {
            auto node = core::get_entity<Node>(id);
            // TODO: This is a very brittle serialization -- is there a better way?
            graph_serialized["nodes"][ std::to_string(id) ] = { 
                { "x", node->x }, 
                { "y", node->y }, 
                { "opinion", node->opinion } 
            };
        }
        uint n = 0;
        for (const edge_t& edge : graph->edges) {
            graph_serialized["edges"][ std::to_string(n) ] = { edge.first, edge.second };
            n++;
        }
    }

    void deserialize_graph(Graph* graph, json& graph_serialized) {
        for (const auto& [s_id, properties] : graph_serialized["nodes"].items()) {
            // Create and add a new node with loaded properties
            core::id_t id = std::stoul(s_id);
            create_node(&id);
            add_node(graph, id);
            Node* node = core::get_entity<Node>(id);
            assert( node );

            // TODO: This is brittle -- is there a better way?
            node->x = properties["x"];
            node->y = properties["y"];
            node->opinion = properties["opinion"];
        }
        for (const auto& edge : graph_serialized["edges"]) {
            add_edge(graph, edge[0], edge[1]);
        }
    }

    // Serialize a Graph struct to JSON and save it to a file.
    void save_graph(const Graph* graph, char* file_path) {
        json graph_serialized;
        serialize_graph(graph, graph_serialized);

        // prettified JSON output
        std::ofstream o(file_path);
        assert( o.good() );
        o << std::setw(4) << graph_serialized << std::endl;
    }

    // Load a JSON serialized graph from a file and deserialize it to a Graph struct.
    // WARNING: Make sure that this is a newly allocated Graph struct.
    // WARNING: Make sure that the entity manager has been appropriately cleared before running this by calling core::clear_all_entities.
    bool load_graph(Graph* graph, char* file_path) {
        json graph_serialized;
        std::ifstream f(file_path);
        if ( ! f.good() ) return false;
        f >> graph_serialized;

        deserialize_graph(graph, graph_serialized);
        return true;
    }

} // end namespace


#endif