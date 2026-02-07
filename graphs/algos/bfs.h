#include "../base.h"
#include <execution>

struct BFSRes {
    vi dist;    // -1 = unreachable
    vi parent;  // parent[v] = prev vertex on shortest path, -1 if none
};

template<class Cost, bool directed>
BFSRes bfs(const Graph<Cost, directed>& G, int src) {
    const int N = G.N;
    BFSRes res;
    res.dist.assign(N, -1);
    res.parent.assign(N, -1);

    queue<int> q;
    res.dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int v = q.front(); q.pop();
        for (auto& e : G[v]) {
            int to = e.to;
            if (res.dist[to] != -1) continue;
            res.dist[to] = res.dist[v] + 1;
            res.parent[to] = v;
            q.push(to);
        }
    }
    return res;
}

// BFS but stops expanding nodes once depth == max_depth
// Nodes deeper than max_depth will remain dist = -1
template<class Cost, bool directed>
BFSRes bfs_limited_depth(const Graph<Cost, directed>& G, int src, int max_depth) {
    const int N = G.N;
    BFSRes res;
    res.dist.assign(N, -1);
    res.parent.assign(N, -1);

    queue<int> q;
    res.dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int v = q.front(); q.pop();
        if (res.dist[v] == max_depth) continue;

        for (auto& e : G[v]) {
            int to = e.to;
            if (res.dist[to] != -1) continue;
            res.dist[to] = res.dist[v] + 1;
            res.parent[to] = v;
            q.push(to);
        }
    }
    return res;
}

// Reconstruct path from src (implicit in parent) to target
// Returns empty if target unreachable
inline vi restore_path(int target, const vi& parent, const vi& dist) {
    if (target < 0 || target >= sz(parent) || dist[target] == -1) return {};
    vi path;
    for (int v = target; v != -1; v = parent[v]) path.push_back(v);
    reverse(all(path));
    return path;
}
