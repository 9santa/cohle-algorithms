#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Residual edge used by max-flow algorithms. */
template<class Cap>
struct FlowEdge {
    int to;
    int rev;
    Cap cap;
};

/** Residual graph for max-flow algorithms. Space: O(n + m). */
template<class Cap = long long>
struct FlowGraph {
    int N = 0;
    vector<vector<FlowEdge<Cap>>> g;
    Cap max_cap = 0;

    FlowGraph() = default;
    explicit FlowGraph(int n) { reset(n); }

    void reset(int n) {
        N = n;
        g.assign(N, {});
        max_cap = 0;
    }

    void add_edge(int u, int v, Cap cap) {
        assert(0 <= u && u < N && 0 <= v && v < N);
        assert(cap >= 0);
        FlowEdge<Cap> a{v, int(g[v].size()), cap};
        FlowEdge<Cap> b{u, int(g[u].size()), Cap(0)};
        g[u].push_back(a);
        g[v].push_back(b);
        max_cap = max(max_cap, cap);
    }

    void add_undirected_edge(int u, int v, Cap cap) {
        add_edge(u, v, cap);
        add_edge(v, u, cap);
    }
};
