#include "../../datastructures/segtree/lazy_segtree.h"

/** HLD with one global lazy segment tree for path add/sum. Space: O(n). */
struct HLD {
    int n;
    V<vi> g;
    /*
        pos[0..n-1] -> unique index of vertex, vertices on the same heavy path get contiguous pos[] values
        base[pos[u]] = value[u] -> linear array that segtree is built on (heavy paths are subarrays)
        head[u] -> topmost vertex of the heavy path containing u
        heavy[u] -> heavy child of u, or -1
    */
    vi parent, depth, heavy, head, pos, siz;
    vl value;   // vertex values
    vl base;    // linearized
    int curPos = 0;
    using AM = ActedMonoid_Sum_Add<ll>;
    LazySegtree<AM> seg;

    HLD(int n_) : n(n_), g(n), parent(n, -1), depth(n, 0), heavy(n, -1),
                    head(n, 0), pos(n, 0), siz(n, 0), value(n, 0) {}

    // picks heavy edges and sizes
    /** Computes subtree sizes and heavy children. Time: O(subtree size). */
    int dfs1(int u, int p) {
        parent[u] = p;
        siz[u] = 1;
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

    // assigns heads and positions
    /** Assigns heads and segment-tree positions. Time: O(subtree size). */
    void dfs2(int u, int h) {
        head[u] = h;
        pos[u] = curPos++;
        base[pos[u]] = value[u];

        if (heavy[u] != -1) dfs2(heavy[u], h); // recurse heavy child first
        for (auto v : g[u]) if (v != parent[u] && v != heavy[u]) {
            dfs2(v, v); // light children after
        }
    }

    /** Builds HLD and segment tree from graph. Time: O(n). */
    void build(const V<vi>& G, int root = 0) {
        g = G;
        curPos = 0;
        dfs1(root, -1);
        base.assign(n, 0);
        dfs2(root, root);
        seg.build(base);
    }

    // O(log^2 n)
    // do path AM::act() on vertices
    /** Adds delta to all vertices on path u-v. Time: O(log^2 n). */
    void path_update(int u, int v, ll delta) {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            int hu = head[u];
            seg.apply(pos[hu], pos[u] + 1, delta);
            u = parent[hu];
        }
        if (depth[u] > depth[v]) swap(u, v);
        seg.apply(pos[u], pos[v] + 1, delta);
    }

    // O(log^2 n)
    // path prod on vertices
    /** Returns sum over vertices on path u-v. Time: O(log^2 n). */
    ll path_prod(int u, int v) {
        ll res = 0;
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            int hu = head[u];
            res += seg.prod(pos[hu], pos[u] + 1);
            u = parent[hu];
        }
        if (depth[u] > depth[v]) swap(u, v);
        res += seg.prod(pos[u], pos[v] + 1);
        return res;
    }
};
