#include <bits/stdc++.h>

using namespace std;

// Floyd-Warshall algorithm for shortest paths between any 2 vertices
// class FloydWarshall {
// private:
//     vector<vector<int>> dist;
//     vector<vector<int>> next;
// };
//
//
// void FloydWarshall(vector<vector<int>>const& graph, int n) {
//     vector<vector<int>> dist = graph;
//
//     for (int k = 0; k < n; k++) {                    // k is the middle vertex
//         for (int i = 0; i < n; i++) {                // i and j are previous and next vertices
//             for (int j = 0; j < n; j++) {
//                 // we check if going through the middle vertex 'k' is 'faster'
//                 if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX && dist[i][k] + dist[k][j] < dist[i][j]) {
//                     dist[i][j] = dist[i][k] + dist[k][j];
//                 }
//             }
//         }
//     }
// }

const int INF = numeric_limits<int>::max();

class FloydWarshall {
private:
    vector<vector<int>> dist;    // Distance matrix
    vector<vector<int>> next;    // Next node in shortest path
    int n;                       // Number of vertices

public:
    FloydWarshall(int vertices) : n(vertices) {
        // Initialize distance matrix with INF
        dist.resize(n+1, vector<int>(n+1, INF));
        next.resize(n+1, vector<int>(n+1, -1));
        
        // Distance from a vertex to itself is 0
        for (int i = 1; i <= n; i++) {
            dist[i][i] = 0;
            next[i][i] = i;
        }
    }
    
    // Add a directed edge from u to v with weight w
    void addEdge(int u, int v, int w) {
        dist[u][v] = w;
        next[u][v] = v;
    }
    
    // Add an undirected edge between u and v with weight w
    void addUndirectedEdge(int u, int v, int w) {
        addEdge(u, v, w);
        addEdge(v, u, w);
    }
    
    // Print distance matrix with proper formatting
    void printDistanceMatrix(int iteration = -1) {
        if (iteration >= 0) {
            cout << "Distance Matrix after iteration k = " << iteration << ":\n";
        } else if (iteration == -1) {
            cout << "Initial Distance Matrix:\n";
        } else {
            cout << "Final Distance Matrix:\n";
        }
        
        cout << "    ";
        for (int i = 1; i <= n; i++) {
            cout << setw(4) << i << " ";
        }
        cout << "\n";
        
        for (int i = 1; i <= n; i++) {
            cout << setw(2) << i << " |";
            for (int j = 1; j <= n; j++) {
                if (dist[i][j] == INF) {
                    cout << setw(4) << "INF" << " ";
                } else if (dist[i][j] == -INF) {
                    cout << setw(4) << "-INF" << " ";
                } else {
                    cout << setw(4) << dist[i][j] << " ";
                }
            }
            cout << "|\n";
        }
        cout << "\n";
    }
    
    // Print next/path matrix with proper formatting
    void printNextMatrix(int iteration = -1) {
        if (iteration >= 0) {
            cout << "Path Matrix after iteration k = " << iteration << ":\n";
        } else if (iteration == -1) {
            cout << "Initial Path Matrix:\n";

        } else {
            cout << "Final Path Matrix:\n";
        }
        
        cout << "    ";
        for (int i = 1; i <= n; i++) {
            cout << setw(2) << i << " ";
        }
        cout << "\n";
        
        for (int i = 1; i <= n; i++) {
            cout << setw(2) << i << " |";
            for (int j = 1; j <= n; j++) {
                if (next[i][j] == -1) {
                    cout << setw(2) << " -" << " ";
                } else {
                    cout << setw(2) << next[i][j] << " ";
                }
            }
            cout << "|\n";
        }
        cout << "\n";
    }
    
    // Print both matrices for a specific iteration
    void printIteration(int k) {
        cout << "==========================================\n";
        cout << "ITERATION k = " << k << "\n";
        cout << "==========================================\n";
        printDistanceMatrix(k);
        printNextMatrix(k);
    }
    
    // Print initial matrices before algorithm starts
    void printInitialMatrices() {
        cout << "==========================================\n";
        cout << "INITIAL MATRICES (k = -1)\n";
        cout << "==========================================\n";
        printDistanceMatrix(-1);
        printNextMatrix(-1);
    }
    
    // Run Floyd-Warshall algorithm with step-by-step printing
    void compute() {
        cout << "FLOYD-WARSHALL ALGORITHM EXECUTION\n";
        cout << "Number of vertices: " << n << "\n\n";
        
        // Print initial state
        printInitialMatrices();
        
        // Main algorithm with iteration printing
        for (int k = 1; k <= n; k++) {
            bool changed = false;
            
            cout << "Processing intermediate vertex k = " << k << "\n";
            cout << "Checking all pairs (i, j) for improvements using vertex " << k << " as intermediate...\n";
            
            for (int i = 1; i <= n; i++) {
                for (int j = 1; j <= n; j++) {
                    // Check for overflow and valid paths
                    if (dist[i][k] != INF && dist[k][j] != INF && 
                        dist[i][k] + dist[k][j] < dist[i][j]) {
                        
                        int oldDist = dist[i][j];
                        dist[i][j] = dist[i][k] + dist[k][j];
                        next[i][j] = next[i][k];
                        
                        cout << "  Updated: dist[" << i << "][" << j << "] = " 
                             << oldDist << " -> " << dist[i][j] 
                             << " (via " << k << "), next[" << i << "][" << j 
                             << "] = " << next[i][j] << "\n";
                        changed = true;
                    }
                }
            }
            
            if (!changed) {
                cout << "  No updates in this iteration.\n";
            }
            
            // Print matrices after this iteration
            printIteration(k);
        }
        
        // // Check for negative cycles
        // cout << "==========================================\n";
        // cout << "CHECKING FOR NEGATIVE CYCLES\n";
        // cout << "==========================================\n";
        //
        // bool negativeCycleFound = false;
        // for (int k = 0; k < n; k++) {
        //     for (int i = 0; i < n; i++) {
        //         for (int j = 0; j < n; j++) {
        //             if (dist[i][k] != INF && dist[k][j] != INF && 
        //                 dist[i][k] + dist[k][j] < dist[i][j]) {
        //                 // Negative cycle detected
        //                 dist[i][j] = -INF;
        //                 next[i][j] = -1;
        //                 negativeCycleFound = true;
        //                 cout << "Negative cycle detected affecting path from " 
        //                      << i << " to " << j << "\n";
        //             }
        //         }
        //     }
        // }
        //
        // if (negativeCycleFound) {
        //     cout << "Negative cycles were detected in the graph!\n";
        // } else {
        //     cout << "No negative cycles detected.\n";
        // }
        
        // Print final matrices
        // cout << "\n==========================================\n";
        // cout << "FINAL RESULTS\n";
        // cout << "==========================================\n";
        // printDistanceMatrix();
        // printNextMatrix();
    }
    
    // Get shortest path distance from u to v
    int getDistance(int u, int v) {
        return dist[u][v];
    }
    
    // Reconstruct shortest path from u to v
    vector<int> getPath(int u, int v) {
        vector<int> path;
        
        // No path exists or negative cycle detected
        if (dist[u][v] == INF || dist[u][v] == -INF) {
            return path;
        }
        
        // Build path using next matrix
        int current = u;
        path.push_back(current);
        
        while (current != v) {
            if (next[current][v] == -1) {
                // No path exists
                return vector<int>();
            }
            current = next[current][v];
            path.push_back(current);
        }
        
        return path;
    }
};

int main() {
    // Create a graph with 4 vertices for clearer demonstration
    FloydWarshall fw(6);

    fw.addEdge(1, 2, 5);
    fw.addEdge(1, 5, 2);
    fw.addEdge(1, 6, 12);
    fw.addEdge(1, 4, 5);
    fw.addEdge(2, 3, 2);
    fw.addEdge(3, 6, 2);
    fw.addEdge(4, 3, 2);
    fw.addEdge(5, 4, 2);
    fw.addEdge(5, 2, 1);
    
    // Run Floyd-Warshall algorithm with detailed printing
    fw.compute();
    
    // Test path reconstruction
    // cout << "==========================================\n";
    // cout << "PATH RECONSTRUCTION EXAMPLES\n";
    // cout << "==========================================\n";
    
    // vector<pair<int, int>> testPaths = {{0, 1}, {0, 2}, {0, 3}, {1, 3}};
    
    // for (auto& path : testPaths) {
    //     int u = path.first;
    //     int v = path.second;
    //     vector<int> shortestPath = fw.getPath(u, v);
    //
    //     cout << "Shortest path from " << u << " to " << v << ": ";
    //     if (shortestPath.empty()) {
    //         cout << "No path exists";
    //     } else {
    //         for (int i = 0; i < shortestPath.size(); i++) {
    //             cout << shortestPath[i];
    //             if (i < shortestPath.size() - 1) {
    //                 cout << " -> ";
    //             }
    //         }
    //     }
    //     cout << " (Distance: ";
    //     int dist = fw.getDistance(u, v);
    //     if (dist == INF) {
    //         cout << "INF";
    //     } else if (dist == -INF) {
    //         cout << "-INF (negative cycle)";
    //     } else {
    //         cout << dist;
    //     }
    //     cout << ")\n";
    // }
    
    return 0;
}
