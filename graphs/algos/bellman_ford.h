#include "../base.h"

template<class Cost>
struct BellmanFordRes {
    V<Cost> dist;
    vi parent;
    bool has_neg_cycle = false; // reachable from src
};

// Requires a directed graph (if you want undirected, add both directions when building)
template<class Cost>
BellmanFordRes<Cost> bellman_ford(const Graph<Cost, true>& G, int src) {
    const int N = G.N;
    const Cost INF = std::numeric_limits<Cost>::max() / 4;

    BellmanFordRes<Cost> res;
    res.dist.assign(N, INF);
    res.parent.assign(N, -1);
    res.dist[src] = 0;

    // Relax edges N-1 times
    for (int it = 0; it < N-1; it++) {
        bool updated = false;
        for (auto& e : G.edges) {
            int u = e.frm, v = e.to;
            Cost w = e.cost;
            if (res.dist[u] == INF) continue;
            Cost nd = res.dist[u] + w;
            if (nd < res.dist[v]) {
                res.dist[v] = nd;
                res.parent[v] = u;
                updated = true; // means we improved the distance
            }
        }
        if (!updated) break;
    }

    // Detect negative cycle reachable from src, by relaxing one more time
    for (auto& e : G.edges) {
        int u = e.frm, v = e.to;
        Cost w = e.cost;
        if (res.dist[u] == INF) continue;
        if (res.dist[u] + w < res.dist[v]) {
            res.has_neg_cycle = true;
            break;
        }
    }
    return res;
}

inline vi restore_path(int target, const vi& parent) {
    vi path;
    if (target < 0 || target >= sz(parent)) return {};
    for (int v = target; v != -1; v = parent[v]) path.push_back(v);
    reverse(all(path));
    return path;
}

