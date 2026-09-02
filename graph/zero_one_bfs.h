#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Adjacency entries are (to, weight), with weight in {0, 1}. */
vector<long long> zero_one_bfs(const vector<vector<pair<int, int>>>& g, int src) {
    int n = (int)g.size();
    const long long INF = numeric_limits<long long>::max() / 4;
    vector<long long> dist(n, INF);
    deque<int> dq;

    dist[src] = 0;
    dq.push_front(src);

    while (!dq.empty()) {
        int v = dq.front();
        dq.pop_front();
        for (auto [to, w] : g[v]) {
            assert(w == 0 || w == 1);
            if (dist[to] <= dist[v] + w) continue;
            dist[to] = dist[v] + w;
            if (w == 0) dq.push_front(to);
            else dq.push_back(to);
        }
    }
    return dist;
}
