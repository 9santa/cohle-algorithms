#include "../base.h"

// Undirected Eulerian trail/cycle in multigraph using edge ids and Hierholzer algorithm
// Input: edges list (u, v). Returns vertex sequence of the rtail, or {} if impossible
inline vi eulerian_trail_undirected(int N, const vpii& edges) {
    int M = sz(edges);
    V<vpii> adj(N); // (to, eid)
    vi deg(N, 0);

    for (int i = 0; i < M; i++) {
        auto [u, v] = edges[i];
        adj[u].push_back({v, i});
        adj[v].push_back({u, i});
        deg[u]++, deg[v]++;
    }

    // find start: if Euler trail exists, start is odd-deg vertex if any, else any with deg > 0
    int start = -1;
    int odd = 0;
    for (int v = 0; v < N; v++) {
        if (deg[v]&1) { odd++; if (start == -1) start = v; }
    }
    if (odd != 0 && odd != 2) return {};
    if (start == -1) {
        for (int v = 0; v < N; v++) if (deg[v]) { start = v; break; }
        if (start == -1) return {0}; // no edges
    }

    V<char> used(M, 0);
    vi it(N, 0);
    vi stk = {start};
    vi path;
    path.reserve(M+1);

    while (!stk.empty()) {
        int v = stk.back();
        while (it[v] < sz(adj[v]) && used[adj[v][it[v]].second]) it[v]++;
        if (it[v] == sz(adj[v])) {
            path.push_back(v);
            stk.pop_back();
        } else {
            auto [to, eid] = adj[v][it[v]++];
            if (used[eid]) continue;
            used[eid] = 1;
            stk.push_back(to);
        }
    }

    if (sz(path) != M+1) return {};
    reverse(all(path));
    return path;
}
