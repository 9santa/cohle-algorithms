#include "../graphs/base.h"

struct CentroidDecomposition {
    int N = 0;
    vi parent;      // parent in centroid tree (-1 for root)
    V<char> used;
    vi sub;

    template<class Cost>
    void build(const Graph<Cost, false>& G) {
        N = G.N;
        parent.assign(N, -1);
        used.assign(N, 0);
        sub.assign(N, 0);

        auto dfs_sz = [&](auto&& self, int v, int p) -> void {
            sub[v] = 1;
            for (auto& e : G[v]) {
                int to = e.to;
                if (to == p) continue;
                self(self, to, v);
                sub[v] += sub[to];
            }
        };

        auto find_centroid = [&](auto&& self, int v, int p, int total) -> int {
            for (auto& e : G[v]) {
                int to = e.to;
                if (to == p || used[to]) continue;
                if (sub[to] > total / 2) return self(self, to, v, total);
            }
            return v;
        };

        auto solve = [&](auto&& self, int entry, int p) -> void {
            dfs_sz(dfs_sz, entry, -1);
            int c = find_centroid(find_centroid, entry, -1, sub[entry]);

            parent[c] = p;
            used[c] = 1;

            for (auto& e : G[c]) {
                int to = e.to;
                if (!used[to]) self(self, to, c);
            }
        };

        solve(solve, 0, -1);
    }
};
