#pragma once
#include <bits/stdc++.h>
using namespace std;

// Ford-Fulkerson using DFS augmenting paths.
// O(FE) for integral capacities, where F is the value of the maximum flow.
template<class Cap = long long>
struct FordFulkerson {
    struct Edge {
        int to;
        int rev;
        Cap cap;
    };

    int n;
    vector<vector<Edge>> g;
    vector<char> vis;
    Cap max_cap = 0;

    explicit FordFulkerson(int n) : n(n), g(n), vis(n) {}

    void add_edge(int u, int v, Cap cap) {
        assert(0 <= u && u < n && 0 <= v && v < n);
        assert(cap >= 0);
        if (cap == 0) return;

        Edge a{v, int(g[v].size()), cap};
        Edge b{u, int(g[u].size()), Cap(0)};
        g[u].push_back(a);
        g[v].push_back(b);
        max_cap = max(max_cap, cap);
    }

    void add_undirected_edge(int u, int v, Cap cap) {
        add_edge(u, v, cap);
        add_edge(v, u, cap);
    }

private:
    Cap dfs(int v, int t, Cap pushed, Cap delta) {
        if (v == t) return pushed;
        vis[v] = 1;

        for (auto &e : g[v]) {
            if (vis[e.to] || e.cap < delta) continue;
            Cap add = dfs(e.to, t, min(pushed, e.cap), delta);
            if (add == 0) continue;

            e.cap -= add;
            g[e.to][e.rev].cap += add;
            return add;
        }
        return 0;
    }

public:
    Cap max_flow(int s, int t) {
        assert(0 <= s && s < n && 0 <= t && t < n);
        if (s == t) return 0;

        Cap flow = 0;
        while (true) {
            fill(vis.begin(), vis.end(), 0);
            Cap pushed = dfs(s, t, numeric_limits<Cap>::max(), Cap(1));
            if (pushed == 0) break;
            flow += pushed;
        }
        return flow;
    }

    // Capacity-scaling variant. Call on a fresh residual network.
    Cap max_flow_scaling(int s, int t) {
        assert(0 <= s && s < n && 0 <= t && t < n);
        if (s == t || max_cap == 0) return 0;

        Cap delta = 1;
        while (delta <= max_cap / 2) delta <<= 1;

        Cap flow = 0;
        for (; delta > 0; delta >>= 1) {
            while (true) {
                fill(vis.begin(), vis.end(), 0);
                Cap pushed = dfs(s, t, numeric_limits<Cap>::max(), delta);
                if (pushed == 0) break;
                flow += pushed;
            }
        }
        return flow;
    }
};
