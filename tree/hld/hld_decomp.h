#pragma once
#include <bits/stdc++.h>
using namespace std;

#define sz(x) int((x).size())

/** Heavy-light decomposition metadata and path interval iterator. Space: O(n). */
struct HLDDecomp {
    int n = 0;
    vector<vector<int>> g;

    vector<int> parent, depth, heavy, head, pos, inv, siz;
    int curPos = 0;

    HLDDecomp() {}
    explicit HLDDecomp(int n_) { init(n_); }

    /** Initializes empty decomposition storage for n vertices. Time: O(n). */
    void init(int n_) {
        n = n_;
        g.assign(n, {});
        parent.assign(n, -1);
        depth.assign(n, -1);
        heavy.assign(n, -1);
        head.assign(n, 0);
        pos.assign(n, 0);
        inv.assign(n, 0);
        siz.assign(n, 0);
        curPos = 0;
    }

    /** Builds HLD from an adjacency list. Time: O(n). */
    void build(const vector<vector<int>>& G, int root = 0) {
        g = G;
        n = sz(g);
        parent.assign(n, -1);
        depth.assign(n, 0);
        heavy.assign(n, -1);
        head.assign(n, 0);
        pos.assign(n, 0);
        inv.assign(n, 0);
        siz.assign(n, 0);
        curPos = 0;

        depth[root] = 0;
        dfs1(root, -1);
        dfs2(root, root);
    }

    /** Adds an undirected edge before build. Time: O(1). */
    void add_edge(int u, int v) {
        g[u].push_back(v);
        g[v].push_back(u);
    }

    /** Computes subtree sizes and heavy children. Time: O(subtree size). */
    int dfs1(int u, int p) {
        parent[u] = p;
        siz[u] = 1;
        heavy[u] = -1;
        int mx = 0;
        for (auto v : g[u]) {
            if (v == p) continue;
            depth[v] = 1 + depth[u];
            int sub = dfs1(v, u);
            siz[u] += sub;
            if (sub > mx) mx = sub, heavy[u] = v;
        }
        return siz[u];
    }

    /** Assigns chain heads and positions. Time: O(subtree size). */
    void dfs2(int u, int h) {
        head[u] = h;
        pos[u] = curPos;
        inv[curPos] = u;
        curPos++;
        if (heavy[u] != -1) dfs2(heavy[u], h);
        for (auto v : g[u]) {
            if (v == parent[u] || v == heavy[u]) continue;
            dfs2(v, v);
        }
    }

    /** Returns the half-open vertex-subtree interval of u. Time: O(1). */
    pair<int, int> subtree_interval(int u) const {
        return {pos[u], pos[u] + siz[u]}; // [l, r)
    }

    /** Returns lowest common ancestor of u and v. Time: O(log n). */
    int lca(int u, int v) const {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            u = parent[head[u]];
        }
        return depth[u] < depth[v] ? u : v; // d[u] < d[v] => u is above v
    }

    // unordered path segments (commut queries + updates)
    /** Calls f(l, r) for vertex intervals covering path u-v. Time: O(log n) calls. */
    template<class F>
    void for_each_path_unordered(int u, int v, F f) const {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            int hu = head[u];
            f(pos[hu], pos[u] + 1);
            u = parent[hu];
        }
        if (depth[u] > depth[v]) swap(u, v);
        f(pos[u], pos[v] + 1);
    }

    /** Calls f(l, r) for edge intervals covering path u-v. Time: O(log n) calls. */
    template<class F>
    void for_each_path_edge_unordered(int u, int v, F f) const {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            int hu = head[u];
            f(pos[hu], pos[u] + 1);
            u = parent[hu];
        }
        if (depth[u] < depth[v]) swap(u, v);
        if (u == v) return; // no edges
        f(pos[v] + 1, pos[u] + 1); // skip LCA vertex position
    }
};
