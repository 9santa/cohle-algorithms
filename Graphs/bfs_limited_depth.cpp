#include <bits/stdc++.h>
using namespace std;

// assuming graph is adjacency list
void bfs_limited_depth(const vector<vector<int>>& graph, int root, int max_depth) {
    int n = static_cast<int>(graph.size());
    vector<bool> visited(n, false);
    queue<pair<int, int>> q;    // {node, depth}

    q.push({root, 0});
    visited[root] = true;

    while(!q.empty()) {
        auto [node, depth] = q.front();
        q.pop();

        if(depth > max_depth) continue;

        cout << "Node: " << node << ", Depth: " << depth << "\n";

        for(auto v : graph[node]) {
            if(!visited[v]) {
                visited[v] = true;
                q.push({v, depth+1});
            }
        }
    }
}

