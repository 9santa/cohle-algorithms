#include <bits/stdc++.h>
#include <climits>
#include <functional>
#include <queue>

using namespace std;

// graph is represented as adj list: pair {to, weight}
int primMST(int n, vector<vector<pair<int, int>>>& graph, int start = 0) {
    vector<int> minEdge(n, INT_MAX); // minimal edge to connect each vertex
    vector<bool> inMST(n, false);    // whether vertex is in MST
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    // start from any vertex
    minEdge[start] = 0;
    pq.push({0, start});

    int totalWeight = 0;

    while (!pq.empty()) {
        auto [w, u] = pq.top();
        pq.pop();

        if (inMST[u]) continue;
        inMST[u] = true;
        totalWeight += w;

        for (auto& e : graph[u]) {
            int v = e.first;
            int weight = e.second;
            if (!inMST[v] && weight < minEdge[v]) {
                minEdge[v] = weight;
                pq.push({weight, v});
            }
        }
    }

    return totalWeight;
}
