#include <bits/stdc++.h>
#include <iostream>

using namespace std;

// 2. Edge list
class GraphEdgeList {
private:
    int V;  // number of vertices
    vector<tuple<int, int, int>> edges; // tuple: (u, v, weight)
    bool directed;

public:
    GraphEdgeList(int vertices, bool is_directed = false) : V(vertices), directed(is_directed) {}

    // Add edge
    void add_edge(int u, int v, int weight = 1) {
        edges.push_back({u, v, weight});
        if(!directed) {
            edges.push_back({v, u, weight});
        }
    }

    // Remove edge
    void remove_edge(int u, int v) {
        auto it = std::remove_if(edges.begin(), edges.end(),
            [u, v, this](const auto& edge) {
                auto [src, dest, weight] = edge;
                return (src == u && dest == v) || (!this->directed && src == v && dest == u);
                });
        
        edges.erase(it);
    }

    // Check if an edge exists
    bool has_edge(int u, int v) const {
        for(const auto& edge : edges) {
            auto [src, dest, weight] = edge;
            if(src == u && dest == v) {
                return true;
            }
            if(!directed) {
                if(src == v && dest == u) return true;
            }
        }
        
        return false;
    }

    // Get weight of an edge
    int get_weight(int u, int v) const {
        for(const auto& edge : edges) {
            auto [src, dest, weight] = edge;
            if(src == u && dest == v) {
                return weight;
            }
            if(!directed) {
                if(src == v && dest == u) return weight;
            }
        }

        return 0;
    }

    // Print graph
    void print() const {
        std::cout << "Edge List:\n";
        for(const auto& edge : edges) {
            auto [u, v, weight] = edge;
            std::cout << "(" << u << " - " << v << ", w: " << weight << ")\n";
        }
    }
};
