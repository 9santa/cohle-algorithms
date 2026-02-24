#include <bits/stdc++.h>
#include <iostream>

using namespace std;

// 3 ways to represent a Graph
// 1. Adjacency list
class GraphList {
private:
    int V;  // number of vertices
    vector<vector<pair<int, int>>> adj_list;    // pair: {neighbor, weight}
    bool directed;

public:
    GraphList(int vertices, bool is_directed = false) : V(vertices), directed(is_directed) {
        adj_list.resize(V);
    }

    // Add edge with weight
    void add_edge(int u, int v, int weight = 1) {
        adj_list[u].push_back({v, weight});
        if(!directed) {
            adj_list[v].push_back({u, weight});
        }
    }

    // Remove edge
    void remove_edge(int u, int v) {
        // remove v from u's list
        for(auto it = adj_list[u].begin(); it != adj_list[u].end(); it++) {
            if(it->first == v) {
                adj_list[u].erase(it);
                break;
            }
        }

        // if not directed, remove u from v's list aswell
        if(!directed) {
            for(auto it = adj_list[v].begin(); it != adj_list[v].end(); it++) {
                if(it->first == u) {
                    adj_list[v].erase(it);
                    break;
                }
            }
        }
    }

    // Check if an edge exists
    bool edge_exist(int u, int v) const {
        for(const auto& neighbor : adj_list[u]) {
            if(neighbor.first == v) {
                return true;
            }
        }

        return false;
    }

    // Get weight of an edge
    int get_weight(int u, int v) const {
        for(const auto& neighbor : adj_list[u]) {
            if(neighbor.first == v) {
                return neighbor.second;
            }
        }

        return 0;   // edge doesn't exist
    }

    // Get degree of a vertex
    int get_degree(int vertex) const {
        return adj_list[vertex].size();
    }

    // Print graph
    void print() const {
        std::cout << "Adjacency List:\n";
        for(int i = 0; i < V; i++) {
            std::cout << i << ": ";
            for(const auto& neighbor : adj_list[i]) {
                std::cout << "(" << neighbor.first << ", " << neighbor.second << ") ";
            }
            std::cout << "\n";
        }
    }
};
