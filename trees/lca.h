#pragma once
#include "rooted_tree.h"

/** Binary-lifting LCA structure for a rooted tree. Space: O(n log n). */
struct LCA {
    int N = 0;
    int LOG = 0;
    vi depth;
    V<vi> up; // up[j][v] = 2^j ancestor of v

    /** Builds jump table from rooted tree metadata. Time: O(n log n). */
    void build(const RootedTree& t) {
        N = sz(t.parent);
        depth = t.depth;

        LOG = 32 - __builtin_clz(N);
        up.assign(LOG, vi(N, -1));

        for (int v = 0; v < N; v++) up[0][v] = t.parent[v];

        for (int j = 1; j < LOG; j++) {
            for (int v = 0; v < N; v++) {
                int p = up[j-1][v];
                up[j][v] = (p == -1 ? -1 : up[j-1][p]);
            }
        }
    }

    /** Returns the k-th ancestor of v, or -1. Time: O(log n). */
    int kth_ancestor(int v, int k) const {
        for (int j = 0; j < LOG; j++) {
            if (v == -1) return -1;
            if (k & (1 << j)) v = up[j][v];
        }
        return v;
    }

    /** Returns lowest common ancestor of a and b. Time: O(log n). */
    int lca(int a, int b) const {
        if (depth[a] < depth[b]) swap(a, b);
        a = kth_ancestor(a, depth[a] - depth[b]);
        if (a == b) return a;
        for (int j = LOG-1; j >= 0; j--) {
            if (up[j][a] != up[j][b]) {
                a = up[j][a];
                b = up[j][b];
            }
        }
        return up[0][a];
    }

    /** Returns edge distance between a and b. Time: O(log n). */
    int dist(int a, int b) const {
        int c = lca(a, b);
        return depth[a] + depth[b] - 2 * depth[c];
    }
};
