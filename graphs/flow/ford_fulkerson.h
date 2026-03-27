#include "base.h"

// Brute force algorithm, we just keep trying to push flow while we can, finds max flow but pretty slow
// Find any augmenting (improving) path using iterative stack DFS, optionally only edges with cap >= delta
// Returns pushed flow (0 if none)
template<class Cap>
static Cap ff_augment(FlowGraph<Cap>& F, int s, int t, Cap delta) {
    int N = F.N;
    vi pv(N, -1), pe(N, -1); // parent vertex, parent edge index
    vi it(N, 0);
    V<char> vis(N, 0);

    vi stk = {s};
    vis[s] = 1;

    while (!stk.empty() && !vis[t]) {
        int v = stk.back();
        if (it[v] == sz(F.g[v])) {
            stk.pop_back();
            continue;
        }
        int ei = it[v]++;
        auto& e = F.g[v][ei];
        if (e.cap < delta) continue;
        if (vis[e.to]) continue;
        vis[e.to] = 1;
        pv[e.to] = v;
        pe[e.to] = ei;
        stk.push_back(e.to);
    }

    if (!vis[t]) return 0;

    // min edge capacity to add
    Cap add = numeric_limits<Cap>::max();
    for (int v = t; v != s; v = pv[v]) {
        auto& e = F.g[pv[v]][pe[v]];
        add = min(add, e.cap);
    }

    // augment
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

// O(F*M), F - max flow, M - #edges
template<class Cap = long long>
Cap max_flow_ford_fulkerson(FlowGraph<Cap>& F, int s, int t) {
    Cap flow = 0;
    // keep trying until we can't find any path s->t no more
    while (true) {
        Cap pushed = ff_augment(F, s, t, (Cap)1);
        if (pushed == 0) break;
        flow += pushed;
    }
    return flow;
}

// Capacity scaling Ford-Fulkerson:
// delta = highest power of two <= max_cap, and only use edges with residual >= delta in that phase
// O(E^2 log C), C - max capacity in graph
template<class Cap = long long>
Cap max_flow_ford_fulkerson_scaling(FlowGraph<Cap>& F, int s, int t) {
    Cap flow = 0;
    Cap delta = 1;
    while (delta <= F.max_cap / 2) delta <<= 1;

    while (delta > 0) {
        while (true) {
            Cap pushed = ff_augment(F, s, t, delta);
            if (pushed == 0) break;
            flow += pushed;
        }
        delta >>= 1;
    }
    return flow;
}
