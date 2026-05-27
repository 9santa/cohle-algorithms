#include "../graphs/core.h"

// Tree diameter: weighted + unweighted
// Two DFSs

/** Returns the farthest vertex and distance from src in a tree. Time: O(n). Space: O(n). */
template<class Cost>
pair<int, Cost> farthest_from(const Graph<Cost, false>& G, int src) {
    const int N = G.N;
    V<char> vis(N, 0);
    V<Cost> dist(N, 0);

    vi stk = {src};
    vis[src] = 1;

    while (!stk.empty()) {
        int v = stk.back(); stk.pop_back();
        for (auto& e : G[v]) {
            int to = e.to;
            if (vis[to]) continue;
            vis[to] = 1;
            dist[to] = dist[v] + e.cost;
            stk.push_back(to);
        }
    }

    int best_v = src;
    for (int i = 0; i < N; i++) {
        if (dist[i] > dist[best_v]) best_v = i;
    }

    return {best_v, dist[best_v]};
}

/** Returns tree diameter length. Time: O(n). Space: O(n). */
template<class Cost>
Cost tree_diameter(const Graph<Cost, false>& G) {
    auto [a, _] = farthest_from(G, 0);
    auto [b, d] = farthest_from(G, a);
    (void)b; // silence the variable, mark it as not used
    return d;
}
