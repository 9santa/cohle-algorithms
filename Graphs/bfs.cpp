#include <bits/stdc++.h>
using namespace std;

// assuming graph is adjacency list
void bfs(const vector<vector<int>>& graph, int root) {
    int n = static_cast<int>(graph.size());
    vector<bool> visited(n, false);
    vector<int> dist(n, INT_MAX);

    queue<int> q;
    q.push(root);
    dist[root] = 0;
    visited[root] = true;

    while(!q.empty()) {
        int cur = q.front();
        q.pop();

        for(auto& v : graph[cur]) {
            if(!visited[v]) {
                visited[v] = true;
                q.push(v);
                dist[v] = dist[cur] + 1;
            }
        }

    }
}
