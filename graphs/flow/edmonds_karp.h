#include "base.h"

// Edmonds-Karp = BFS shortest augmenting paths (basically Ford-Fulkerson but BFS instead of DFS)
// O(V * E^2)
template<class Cap>
static Cap ek_augment(FlowGraph<Cap>& F, int s, int t, Cap delta) {
    int N = F.N;
    vi pv(N, -1), pe(N, -1);
    queue<int> q;
    q.push(s);
    pv[s] = s;

    while (!q.empty() && pv[t] == -1) {
        int v = q.front(); q.pop();
        for (int i = 0; i < sz(F.g[v]); i++) {
            auto& e = F.g[v][i];
            if (e.cap < delta) continue;
            if (pv[e.to] != -1) continue;
            pv[e.to] = v;
            pe[e.to] = i;
            q.push(e.to);
            if (e.to == t) break;
        }
    }

    if (pv[t] == -1) return 0; // target unreachable

    Cap add = numeric_limits<Cap>::max();
    for (int v = t; v != s; v = pv[v]) {
        auto& e = F.g[pv[v]][pe[v]];
        add = min(add, e.cap);
    }

    for (int v = t; v != s; v = pv[v]) {
        int u = pv[v];
        int ei = pe[v];
        auto& e = F.g[u][ei];
        auto& re = F.g[v][e.rev];
        e.cap -= add;
        re.cap += add;
    }
    return add;
}

template<class Cap = long long>
Cap max_flow_edmonds_karp(FlowGraph<Cap>& F, int s, int t) {
    Cap flow = 0;
    while (true) {
        Cap pushed = ek_augment(F, s, t, (Cap)1);
        if (pushed == 0) break;
        flow += pushed;
    }
    return flow;
}

template<class Cap = long long>
Cap max_flow_edmonds_karp_scaling(FlowGraph<Cap> &F, int s, int t) {
    Cap flow = 0;
    Cap delta = 1;
    while (delta <= F.max_cap / 2) delta <<= 1;

    while (delta > 0) {
        while (true) {
            Cap pushed = ek_augment(F, s, t, delta);
            if (pushed == 0) break;
            flow += pushed;
        }
        delta >>= 1;
    }
    return flow;
}
