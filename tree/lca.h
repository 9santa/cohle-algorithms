#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Binary-lifting LCA. Build directly from an undirected tree adjacency list. */
struct LCA {
    int n = 0, LOG = 0;
    vector<int> depth;
    vector<vector<int>> up;

    LCA() = default;
    explicit LCA(const vector<vector<int>>& g, int root = 0) { build(g, root); }

    void build(const vector<vector<int>>& g, int root = 0) {
        n = (int)g.size();
        LOG = 1;
        while ((1 << LOG) <= max(1, n)) ++LOG;

        depth.assign(n, 0);
        up.assign(LOG, vector<int>(n, root));
        if (n == 0) return;

        vector<int> parent(n, -1);
        parent[root] = root;
        vector<int> st = {root};
        while (!st.empty()) {
            int v = st.back();
            st.pop_back();
            for (int to : g[v]) {
                if (to == parent[v]) continue;
                parent[to] = v;
                depth[to] = depth[v] + 1;
                st.push_back(to);
            }
        }

        up[0] = parent;
        for (int j = 1; j < LOG; ++j) {
            for (int v = 0; v < n; ++v) up[j][v] = up[j - 1][up[j - 1][v]];
        }
    }

    int kth_ancestor(int v, int k) const {
        for (int j = 0; j < LOG; ++j) if (k >> j & 1) v = up[j][v];
        return v;
    }

    int lca(int a, int b) const {
        if (depth[a] < depth[b]) swap(a, b);
        a = kth_ancestor(a, depth[a] - depth[b]);
        if (a == b) return a;

        for (int j = LOG - 1; j >= 0; --j) {
            if (up[j][a] != up[j][b]) {
                a = up[j][a];
                b = up[j][b];
            }
        }
        return up[0][a];
    }

    int dist(int a, int b) const {
        int c = lca(a, b);
        return depth[a] + depth[b] - 2 * depth[c];
    }
};
