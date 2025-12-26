#include <bits/stdc++.h>

using namespace std;

// This version is able to detect cycles in DAG
vector<int> topologicalSortKahn(vector<vector<int>>& graph) {
    int n = static_cast<int>(graph.size());
    vector<int> indegree(n, 0);
    queue<int> q;
    vector<int> result;

    // Count incoming edges in every vertex
    for (int u = 0; u < n; u++) {
        for (int v : graph[u]) {
            indegree[v]++;
        }
    }

    // Add vertices with indegree = 0 in the queue
    for (int i = 0; i < n; i++) {
        if (indegree[i] == 0) {
            q.push(i);
        }
    }

    while(!q.empty()) {
        int u = q.front();
        q.pop();
        result.push_back(u);

        // Reduce indegree of 'u's neighbours
        for (int v : graph[u]) {
            indegree[v]--;
            if(indegree[v] == 0) {
                q.push(v);
            }
        }
    }

    if (result.size() != n) {
        cout << "Graph contains a cycle, topological sort is not possible!\n";
        return {};
    }

    return result;
}

