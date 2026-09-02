#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Dijkstra distances and parent tree. */
template<class Cost>
struct DijkstraRes {
    vector<Cost> dist;
    vector<int> parent;
};

/** Adjacency entries are (to, weight). Weights must be nonnegative. */
template<class Cost>
DijkstraRes<Cost> dijkstra(const vector<vector<pair<int, Cost>>>& g, int src) {
    int n = (int)g.size();
    const Cost INF = numeric_limits<Cost>::max() / 4;

    DijkstraRes<Cost> res{vector<Cost>(n, INF), vector<int>(n, -1)};
    using State = pair<Cost, int>; // (distance, vertex)
    priority_queue<State, vector<State>, greater<State>> pq;

    res.dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, v] = pq.top();
        pq.pop();
        if (d != res.dist[v]) continue;

        for (auto [to, w] : g[v]) {
            Cost nd = d + w;
            if (nd < res.dist[to]) {
                res.dist[to] = nd;
                res.parent[to] = v;
                pq.push({nd, to});
            }
        }
    }
    return res;
}
