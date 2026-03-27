#include <bits/stdc++.h>
#include <climits>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

// 3. Adjacency Matrix. Space: O(V^2)
class GraphMatrix {
private:
    int V;  // number of vertices
    std::vector<std::vector<int>> adj_matrix;
    bool directed;
    const int NO_EDGE = INT_MAX;

public:
    GraphMatrix(int vertices, bool is_directed = false) : V(vertices), directed(is_directed) {
        adj_matrix.resize(V, std::vector<int>(V, NO_EDGE));

        // diagonal (edge itself to itself)
        for(int i = 0; i < V; i++) {
            adj_matrix[i][i] = 0;
        }
    }

    // Add edge
    void add_edge(int u, int v, int weight = 1) {
        adj_matrix[u][v] = weight;
        if(!directed) {
            adj_matrix[v][u] = weight;
        }
    }

    // Remove edge
    void remove_edge(int u, int v) {
        adj_matrix[u][v] = NO_EDGE;
        if(!directed) {
            adj_matrix[v][u] = NO_EDGE;
        }
    }

    // Check if an edge exists
    bool has_edge(int u, int v) const {
        if(u < 0 || u >= V || v < 0 || v >= V) return false;

        return adj_matrix[u][v] != NO_EDGE;
    }

    // Get weight of an edge
    int get_weight(int u, int v) const {
        if(u < 0 || u >= V || v < 0 || v >= V) {
            throw std::out_of_range("Invalid vertex index\n");
        }

        if(adj_matrix[u][v] == NO_EDGE) {
            throw std::runtime_error("Edge does not exist\n");
        }

        return adj_matrix[u][v];
    }

    // Print graph
    void print() const {
        std::cout << "Adjacency Matrix:\n   ";
        for(int i = 0; i < V; i++) {
            std::cout << i << " ";
        }
        std::cout << "\n";

        for(int i = 0; i < V; i++) {
            std::cout << i << ": ";
            for(int j = 0; j < V; j++) {
                if(adj_matrix[i][j] == NO_EDGE) {
                    std::cout << "∞ ";  // infinity
                } else {
                    std::cout << adj_matrix[i][j] << " ";
                }
            }
            std::cout << "\n";
        }
    }
};


// testing example
int main(void) {
    std::cout << "Adjacency Matrix Representation:\n";
    GraphMatrix matrixGraph(5);
    matrixGraph.add_edge(0, 1);
    matrixGraph.add_edge(0, 4);
    matrixGraph.add_edge(1, 2);
    matrixGraph.add_edge(1, 3);
    matrixGraph.add_edge(1, 4);
    matrixGraph.add_edge(2, 3);
    matrixGraph.add_edge(3, 4);
    matrixGraph.print();
}
