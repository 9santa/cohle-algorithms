#include "hld_decomp.h"

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

    void init(int n_) {
        n = n_;
        hld.init(n);
        value.assign(n, Monoid::id());
        base.assign(n, Monoid::id());
    }

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
    X subtree_prod(int u) {
        pair<int, int> it = hld.subtree_interval(u);
        return ds.prod(it.first, it.second);
    }

    // ----- subtree (edge mode, edge stored at child vertex pos) -----
    X subtree_prod_edge(int u) {
        pair<int, int> it =hld.subtree_interval(u);
        int l = it.first + 1, r = it.second;
        if (l >= r) return Monoid::id();
        return ds.prod(l, r);
    }

    // ----- path query (commute combine) -----
    X path_prod(int u, int v) {
        X res = Monoid::id();
        hld.for_each_path_unordered(u, v, [&](int l, int r) {
            res = Monoid::op(res, ds.prod(l, r));
        });
        return res;
    }

    X path_prod_edge(int u, int v) {
        X res = Monoid::id();
        hld.for_each_path_edge_unordered(u, v, [&](int l, int r) {
            res = Monoid::op(res, ds.prod(l, r));
        });
        return res;
    }

    // ----- point set (requires DS::set) -----
    void point_set(int u, const X& x) {
        value[u] = x;
        ds.set(hld.pos[u], x);
    }

    // ----- range updates (requires DS::apply) -----
    template<class A>
    void path_apply(int u, int v, const A& a) {
        hld.for_each_path_unordered(u, v, [&](int l, int r) {
            ds.apply(l, r, a);
        });
    }

    template<class A>
    void path_apply_edge(int u, int v, const A& a) {
        hld.for_each_path_edge_unordered(u, v, [&](int l, int r) {
            ds.apply(l, r, a);
        });
    }

    template<class A>
    void subtree_apply(int u, const A& a) {
        pair<int, int> it = hld.subtree_interval(u);
        ds.apply(it.first, it.second, a);
    }

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

