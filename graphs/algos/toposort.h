#include "../base.h"
#include <optional>

// Topsort with cycle detection, Kahn's algorithm
template<class Cost>
std::optional<vi> toposort_kahn(const Graph<Cost, true>& G) {
    const int N = G.N;
    vi indeg(N, 0);
    for (int v = 0; v < N; v++) {
        for (auto& e : G[v]) indeg[e.to]++;
    }

    queue<int> q;
    for (int v = 0; v < N; v++) if (indeg[v] == 0) q.push(v);

    vi topo;
    topo.reserve(N);

    while (!q.empty()) {
        int v = q.front(); q.pop();
        topo.push_back(v);
        for (auto& e : G[v]) {
            int to = e.to;
            if (--indeg[to] == 0) q.push(to);
        }
    }

    if (sz(topo) != N) return std::nullopt; // cycle
    return topo;
}

// DFS version with cycle detection
template<class Cost>
std::optional<vi> toposort_dfs(const Graph<Cost, true>& G) {
    const int N = G.N;
    V<char> state(N, 0); // 0-unvisited, 1-in stack, 2-done
    vi topo;
    topo.reserve(N);

    auto dfs = [&](auto&& self, int v) -> bool {
        state[v] = 1;
        for (auto& e : G[v]) {
            int to = e.to;
            if (state[to] == 1) return false; // cycle
            if (state[to] == 0 && !self(self, to)) return false;
        }
        state[v] = 2;
        topo.push_back(v);
        return true;
    };

    for (int v = 0; v < N; v++) {
        if (state[v] == 0) {
            if (!dfs(dfs, v)) return std::nullopt;
        }
    }
    reverse(all(topo));
    return topo;
}
