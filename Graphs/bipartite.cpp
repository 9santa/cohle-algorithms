#include <bits/stdc++.h>

using namespace std;

// Check if graph is bipartite with BFS
bool isBipartite(vector<vector<int>>& graph) {
    int n = (int)graph.size();
    vector<int> color(n, -1);    // -1: unvisited, 0: color1, 1: color2

    for(int i = 0; i < n; i++) {
        if(color[i] == -1) {
            queue<int> q;
            q.push(i);
            color[i] = 0;

            while(!q.empty()) {
                int node = q.front();
                q.pop();

                for(auto v : graph[node]) {
                    if(color[v] == -1) {
                        color[v] = color[node] ^ 1; // flips 0 and 1
                        q.push(v);
                    } else if(color[v] == color[node]) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}
