#pragma once
/* ===== Graph for Flow algorithms base structure ===== */

#include "../core.h"

// Residual edge (mutable)
/** Residual edge used by max-flow algorithms. */
template<class Cap>
struct FlowEdge {
    int to;
    int rev;    // index of reverse edge in g[to]
    Cap cap;    // residual capacity
};

/** Residual graph for max-flow algorithms. Space: O(n + m). */
template<class Cap = long long>
struct FlowGraph {
    int N = 0;
    vector<vector<FlowEdge<Cap>>> g;
    Cap max_cap = 0; // track maximum capacity for scaling

    FlowGraph() {}
    explicit FlowGraph(int n) { reset(n); }

    /** Resets graph to n vertices and no edges. Time: O(n + m). */
    void reset(int n) {
        N = n;
        g.assign(N, {});
        max_cap = 0;
    }

    // add directed edge u -> v with capacity cap
    /** Adds a directed residual edge u -> v. Time: O(1). */
    void add_edge(int u, int v, Cap cap) {
        assert(0 <= u && u < N && 0 <= v && v < N);
        assert(cap >= 0);
        FlowEdge<Cap> a{v, sz(g[v]), cap};
        FlowEdge<Cap> b{u, sz(g[u]), 0}; // reversed edge
        g[u].push_back(a);
        g[v].push_back(b);
        if (cap > max_cap) max_cap = cap;
    }

    // add undirected "capacity both ways" edge
    /** Adds capacity cap in both directions. Time: O(1). */
    void add_undirected_edge(int u, int v, Cap cap) {
        add_edge(u, v, cap);
        add_edge(v, u, cap);
    }
};
