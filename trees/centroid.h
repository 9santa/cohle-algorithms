#include "../graphs/base.h"

template<class Cost>
int tree_centroid(const Graph<Cost, false>& G) {
    const int N = G.N;
    vi sub(N, 0);
    int root = 0;

    auto dfs = [&](auto&& self, int v, int p) -> void {
        sub[v] = 1;
        for (auto& e : G[v]) {
            int to = e.to;
            if (to == p) continue;
            self(self, to, v);
            sub[v] += sub[to];
        }
    };
    dfs(dfs, root, -1);

    int v = root, p = -1;
    while (true) {
        int heavy = -1;
        for (auto& e : G[v]) {
            int to = e.to;
            if (to == p) continue;
            if (sub[to] > N / 2) { heavy = to; break; }
        }
        if (heavy == -1) return v;
        p = v; v = heavy;
    }
}
