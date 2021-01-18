#pragma once

#include <queue>
#include <unordered_set>
#include <vector>

#include "../types.h"
#include "../data_structures/graph.h"

namespace graph {

    // Function that can just grab 
    void accumulate_nodes(Graph graph, uint node, void* ordering) {
        ((std::vector<uint>*) ordering)->push_back(node);
    }

    // Get a BFS linearization of a graph.
    // Allows an optional custom function to apply to nodes over the course of the traversal.
    void
    bfs(
        const Graph graph, uint source, 
        void (*f) (Graph graph, uint node, void* data) = nullptr,
        void* data = nullptr
    ) {
        std::unordered_set<uint> visited;
        std::queue<uint> queue;
        queue.push(source);
        int i;

        while (! queue.empty()) {
            auto node = queue.front();
            queue.pop();
            for (i = 0; i < graph->nodes[node]->num_adjacent; ++i) {
                auto next = graph->nodes[node]->adjacent[i];
                if (visited.find(next) == visited.end()) {
                    visited.insert(next);
                    queue.push(graph->nodes[node]->adjacent[i]);

                    // apply custom function to node
                    if (f != nullptr) f(graph, next, data);
                }
            }
        }
    } // end bfs

    void
    dfs(
        const Graph graph, uint source, 
        void (*f) (Graph graph, uint node, void* data) = nullptr,
        void* data = nullptr
    ) {
        std::unordered_set<uint> visited;
        std::vector<uint> stack;
        stack.push_back(source);
        int i;

        while (! stack.empty()) {
            auto node = stack.back();
            stack.pop_back();
            for (i = 0; i < graph->nodes[node]->num_adjacent; ++i) {
                auto next = graph->nodes[node]->adjacent[i];
                if (visited.find(next) == visited.end()) {
                    visited.insert(next);
                    stack.push_back(next);

                    // apply custom function to node
                    if (f != nullptr) f(graph, next, data);
                }
            }
        }
    } // end dfs

} // end namespace
