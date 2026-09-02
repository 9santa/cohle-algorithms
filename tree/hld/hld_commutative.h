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


/** HLD wrapper for commutative path/subtree queries over an underlying data structure. Space: O(n). */
template<class Monoid, class DS>
struct HLD_Comm {
    using X = typename Monoid::value_type;

    HLDDecomp hld;
    int n = 0;
    int root = 0;

    vector<X> value; // by vertex id
    vector<X> base;  // by pos[u]
    DS ds;           // underlying DS

    HLD_Comm() {}
    explicit HLD_Comm(int n_) { init(n_); }

    /** Initializes storage for n vertices. Time: O(n). */
    void init(int n_) {
        n = n_;
        hld.init(n);
        value.assign(n, Monoid::id());
        base.assign(n, Monoid::id());
    }

    /** Builds HLD and underlying DS from vertex values. Time: O(n + DS build). */
    void build(const vector<vector<int>>& G, const vector<X>& values_by_vertex, int root_ = 0) {
        root = root_;
        n = sz(G);
        hld.build(G, root);
        value.assign(n, Monoid::id());
        for (int i = 0; i < sz(values_by_vertex); i++) value[i] = values_by_vertex[i];

        base.assign(n, Monoid::id());
        for (int u = 0; u < n; u++) base[hld.pos[u]] = value[u];
        ds.build(base); // requires DS::build(vector)
    }

    // ----- subtree (vertex mode) -----
    /** Returns aggregate over subtree vertices of u. Time: O(DS query). */
    X subtree_prod(int u) {
        pair<int, int> it = hld.subtree_interval(u);
        return ds.prod(it.first, it.second);
    }

    // ----- subtree (edge mode, edge stored at child vertex pos) -----
    /** Returns aggregate over subtree edges below u. Time: O(DS query). */
    X subtree_prod_edge(int u) {
        pair<int, int> it =hld.subtree_interval(u);
        int l = it.first + 1, r = it.second;
        if (l >= r) return Monoid::id();
        return ds.prod(l, r);
    }

    // ----- path query (commute combine) -----
    /** Returns aggregate over path vertices u-v. Time: O(log n * DS query). */
    X path_prod(int u, int v) {
        X res = Monoid::id();
        hld.for_each_path_unordered(u, v, [&](int l, int r) {
            res = Monoid::op(res, ds.prod(l, r));
        });
        return res;
    }

    /** Returns aggregate over path edges u-v. Time: O(log n * DS query). */
    X path_prod_edge(int u, int v) {
        X res = Monoid::id();
        hld.for_each_path_edge_unordered(u, v, [&](int l, int r) {
            res = Monoid::op(res, ds.prod(l, r));
        });
        return res;
    }

    // ----- point set (requires DS::set) -----
    /** Sets vertex value at u. Time: O(DS set). */
    void point_set(int u, const X& x) {
        value[u] = x;
        ds.set(hld.pos[u], x);
    }

    // ----- range updates (requires DS::apply) -----
    /** Applies action a to path vertices u-v. Time: O(log n * DS apply). */
    template<class A>
    void path_apply(int u, int v, const A& a) {
        hld.for_each_path_unordered(u, v, [&](int l, int r) {
            ds.apply(l, r, a);
        });
    }

    /** Applies action a to path edges u-v. Time: O(log n * DS apply). */
    template<class A>
    void path_apply_edge(int u, int v, const A& a) {
        hld.for_each_path_edge_unordered(u, v, [&](int l, int r) {
            ds.apply(l, r, a);
        });
    }

    /** Applies action a to subtree vertices of u. Time: O(DS apply). */
    template<class A>
    void subtree_apply(int u, const A& a) {
        pair<int, int> it = hld.subtree_interval(u);
        ds.apply(it.first, it.second, a);
    }

    /** Applies action a to subtree edges below u. Time: O(DS apply). */
    template<class A>
    void subtree_apply_edge(int u, const A& a) {
        pair<int, int> it = hld.subtree_interval(u);
        int l = it.first + 1, r = it.second;
        if (l >= r) return;
        ds.apply(l, r, a);
    }
};

/*
============================= USAGE EXAMPLE =============================

A) Your LazySegtree<AM> (half-open [l,r), build(vector), prod, apply) + commutative sum:

    using AM = ActedMonoid_Sum_Add<ll>;
    using Mon = typename AM::Monoid_X;   // Monoid_Sum<ll>
    LazySegtree<AM> seg;
    HLD_Comm<Mon, LazySegtree<AM>> hld;

    vector<vector<int>> G(n);
    vector<ll> a(n); // value[u] by vertex id
    hld.build(G, a, 0);
    hld.path_apply(u, v, +5LL);
    cout << hld.path_prod(u, v) << "\n";

B) Non-commutative directed path fold u->v (e.g. string concat, affine, matrices):
   Build your segment tree on Monoid_Bidir<Monoid>.

   Example with your SegTree<Monoid> that already has build(vector) + prod(l,r):

    using M  = Monoid_StringConcat;
    using Mb = Monoid_Bidir<M>;
    SegTree<Mb> segB;                    // your generic monoid segtree
    HLD_Directed<M, SegTree<Mb>> hlddir;
    vector<string> s(n);
    hlddir.build(G, s, 0);
    cout << hlddir.path_prod_dir(u, v) << "\n";  // correct order u->v

C) Non-commutative + lazy updates:
   Use ActedMonoid_Bidir<AM> and LazySegtree<ActedMonoid_Bidir<AM>>.

    using AM  = ActedMonoid_Sum_Add<ll>;         // example
    using AMb = ActedMonoid_Bidir<AM>;
    using Mon0 = typename AM::Monoid_X;          // underlying monoid for original value type
    LazySegtree<AMb> segB;
    HLD_Directed<Mon0, LazySegtree<AMb>> hlddir;
    vector<ll> a(n);
    hlddir.build(G, a, 0);
    hlddir.path_apply(u, v, +7LL);               // works (applies to both fwd & rev)
    auto ans = hlddir.path_prod_dir(u, v);       // returns ll

=====================================================================================
*/
