#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// infinity
constexpr int INF = 1e9;

int N;  // number of vertices
vector<vector<int>> capacity;   // capacities matrix
vector<vector<int>> flow;       // current flow matrix
vector<bool> visited;           // for marking in dfs
int source, sink;               // source and sink vertices numbers

int dfs(int v, int current_flow) {
    // found path, return delivered flow
    if (v == sink) {
        return current_flow;
    }

    // mark as visited in this path
    visited[v] = true;

    // iterate through neighbors of v
    for (int u = 0; u < N; u++) {
        // residual copacity (v, u)
        int residual_capacity = capacity[v][u] - flow[v][u];

        // if not marked and we can push flow through (v, u)
        if (!visited[u] && residual_capacity > 0) {
            // trying to reach the sink
            int pushed = dfs(u, min(current_flow, residual_capacity));

            // if reached the end and have some flow left
            if (pushed > 0) {
                // increase flow on straight edge
                flow[v][u] += pushed;
                // decrease flow on reversed edge
                flow[u][v] -= pushed;
                // found path and flow, return the value of pushed flow
                return pushed;
            }
        }
    }

    return 0;
}

// main function for finding max flow
int max_flow() {
    int total_flow = 0;
    flow.assign(N, vector<int>(N, 0));

    while (true) {
        // before each new search 'forget' marked vertices
        visited.assign(N, false);

        // start dfs from source with INF flow
        int pushed = dfs(source, INF);
        // if can't reach the end, stop the cycle
        if (pushed == 0) break;

        // add found flow to total
        total_flow += pushed;

    }

    return total_flow;
}


signed main(void) {
    N = 8;
    source = 0;
    sink = 7;

    // capacity = {
    //     {0,4,0,3,0,0},
    //     {0,0,3,0,1,0},
    //     {0,0,0,2,0,4},
    //     {0,0,0,0,5,0},
    //     {0,0,0,0,0,3},
    //     {0,0,0,0,0,0}
    // };
    
    // capacity = {
    //     {0, 16, 13, 0, 0, 0},
    //     {0, 0, 10, 12, 0, 0},
    //     {0, 4, 0, 0, 14, 0},
    //     {0, 0, 9, 0, 0, 20},
    //     {0, 0, 0, 7, 0, 4},
    //     {0, 0, 0, 0, 0, 0}
    // };

    // capacity = {
    // //   0   1   2   3   4   5   6   7   8   9    <- Индекс "куда" (Вершины 1-10)
    //   {  0, 11,  0, 10,  0,  0,  0,  0,  0,  0 }, // 0 (Вершина 1)
    //   {  0,  0,  5,  0, 24,  4,  0,  0,  0,  0 }, // 1 (Вершина 2)
    //   {  0,  0,  0,  0, 14,  0, 13,  0,  0, 15 }, // 2 (Вершина 3)
    //   {  0,  0, 15,  0,  0,  0,  0,  4,  0,  0 }, // 3 (Вершина 4)
    //   {  0,  0,  0,  0,  0, 11,  0,  0, 23,  0 }, // 4 (Вершина 5)
    //   {  0,  0, 23,  0,  0,  0,  0,  0,  0,  0 }, // 5 (Вершина 6)
    //   {  0,  0,  0,  0,  0,  0,  0,  0,  0, 11 }, // 6 (Вершина 7)
    //   {  0,  0,  1,  0,  0,  0,  0,  0,  0,  1 }, // 7 (Вершина 8)
    //   {  0,  0,  0,  0,  0, 12,  0, 13,  0,  2 }, // 8 (Вершина 9)
    //   {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }  // 9 (Вершина 10)
    // };

    capacity = {
        {0,4,0,4,0,6,0,0},
        {0,0,3,5,0,0,0,0},
        {0,0,0,0,2,0,0,2},
        {0,0,0,0,3,0,0,0},
        {0,0,0,0,0,0,3,4},
        {0,0,0,0,7,0,3,0},
        {0,0,0,0,0,0,0,5},
        {0,0,0,0,0,0,0,0}
    };

    cout << "Max flow: " << max_flow() << "\n";
}
