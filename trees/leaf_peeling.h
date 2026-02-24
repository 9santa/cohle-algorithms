#include "../graphs/base.h"

template<class Cost>
vi tree_centers_by_leaf_peeling(const Graph<Cost, false>& G) {
    const int N = G.N;
    if (N == 1) return {0};

    vi deg(N, 0);
    for (int v = 0; v < N; v++) for (auto& e : G[v]) deg[v]++;

    queue<int> q;
    for (int v = 0; v < N; v++) if (deg[v] <= 1) q.push(v);

    int remaining = N;
    while (remaining > 2) {
        int siz = sz(q);
        remaining -= siz;
        for (int i = 0; i < siz; i++) {
            int v = q.front(); q.pop();
            for (auto& e : G[v]) {
                int to = e.to;
                if (deg[to] == 0) continue;
                if (--deg[to] == 1) q.push(to);
            }
            deg[v] = 0;
        }
    }

    vi centers;
    while (!q.empty()) { centers.push_back(q.front()); q.pop(); }
    return centers;
}
