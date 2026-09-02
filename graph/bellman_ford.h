#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Bellman-Ford result with negative-cycle flag. Space: O(n). */
template<class Cost>
struct BellmanFordRes {
    vector<Cost> dist;
    vector<int> parent;
    bool has_negative_cycle = false; // reachable from src
};

/**
 * Runs Bellman-Ford from src on a directed edge list (u, v, weight).
 * For an undirected graph, add both directed copies.
 * Time: O(nm). Space: O(n).
 */
template<class Cost>
BellmanFordRes<Cost> bellman_ford(
    int n,
    const vector<tuple<int, int, Cost>>& edges,
    int src
) {
    const Cost INF = numeric_limits<Cost>::max() / 4;

    BellmanFordRes<Cost> res;
    res.dist.assign(n, INF);
    res.parent.assign(n, -1);
    res.dist[src] = 0;

    for (int it = 0; it < n - 1; ++it) {
        bool changed = false;
        for (auto [u, v, w] : edges) {
            if (res.dist[u] == INF) continue;
            if (res.dist[v] > res.dist[u] + w) {
                res.dist[v] = res.dist[u] + w;
                res.parent[v] = u;
                changed = true;
            }
        }
        if (!changed) break;
    }

    for (auto [u, v, w] : edges) {
        if (res.dist[u] != INF && res.dist[v] > res.dist[u] + w) {
            res.has_negative_cycle = true;
            break;
        }
    }
    return res;
}

/** Restores a path from the source encoded by parent[] to target. */
vector<int> restore_path(int target, const vector<int>& parent) {
    if (target < 0 || target >= (int)parent.size()) return {};
    vector<int> path;
    for (int v = target; v != -1; v = parent[v]) path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}
