#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
template<class T> using V = vector<T>;
using vi = vector<int>;
using vl = vector<long long>;
#define sz(x) int((x).size())
#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define F0R(i, n) for (int i = 0; i < (n); ++i)
inline int topbit(int x) { return x == 0 ? -1 : 31 - __builtin_clz((unsigned)x); }
inline int topbit(unsigned x) { return x == 0 ? -1 : 31 - __builtin_clz(x); }
inline int topbit(long long x) { return x == 0 ? -1 : 63 - __builtin_clzll((unsigned long long)x); }
inline int topbit(unsigned long long x) { return x == 0 ? -1 : 63 - __builtin_clzll(x); }

/**
* Lazy segment tree over an acted monoid.
* Range updates use ActedMonoid::act(), range queries use Monoid_X::op().
* Space: O(n).
*/
template<typename ActedMonoid>
struct LazySegtree {
    using AM = ActedMonoid;
    using MX = typename AM::Monoid_X;
    using MA = typename AM::Monoid_A;
    using X = typename MX::value_type;
    using A = typename MA::value_type;
    int n, log, size;   // n = array size, size = actual segtree size
    vector<X> data;
    vector<A> lazy;

    LazySegtree() {}
    LazySegtree(int n) { build(n); }
    template<typename F>
    LazySegtree(int n, F f) { build(n, f); }
    LazySegtree(const V<X>& v) { build(v); }

    /** Builds a tree of size m filled with the data identity. Time: O(n). */
    void build(int m) {
        build(m, [](int i) -> X {return MX::id(); });
    }

    /** Builds the tree from a 0-indexed array. Time: O(n). */
    void build(const V<X>& v) {
        build(sz(v), [&v](int i) -> X { return v[i]; });
    }

    /** Builds the tree from values f(i). Time: O(n). */
    template<typename F>
    void build(int m, F f) {
        n = m, log = 1;
        while ((1<<log) < n) log++;
        size = 1 << log;
        data.assign(size<<1, MX::id());
        lazy.assign(size, MA::id());
        F0R(i, n) data[size+i] = f(i);
        for (int i = size-1; i >= 1; i--) update(i);
    }

    void update(int p) { data[p] = MX::op(data[p<<1], data[p<<1|1]); }

    /** Sets a[p] to x. Time: O(log n). */
    void set(int p, X x) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = log; i >= 1; i--) push(p>>i);
        data[p] = x;
        for (int i = 1; i <= log; i++) update(p>>i);
    }

    /** Replaces a[p] by Monoid_X::op(a[p], x). Time: O(log n). */
    void multiply(int p, X x) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = log; i >= 1; i--) push(p>>i);
        data[p] = MX::op(data[p], x);
        for (int i = 1; i <= log; i++) update(p>>i);
    }

    /** Returns a[p]. Time: O(log n). */
    X get(int p) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = log; i >= 1; i--) push(p>>i);
        return data[p];
    }

    /** Pushes all lazy tags and returns the current array. Time: O(n). */
    V<X> get_all() {
        FOR(i, 1, size) { push(i); }
        return {data.begin() + size, data.begin() + size + n};
    }

    /** Returns the monoid product over [l, r). Time: O(log n). */
    X prod(int l, int r) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return MX::id();
        l += size, r += size;
        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r-1) >> i);
        }
        X xl = MX::id(), xr = MX::id();
        while (l < r) {
            if (l&1) xl = MX::op(xl, data[l++]);
            if (r&1) xr = MX::op(data[--r], xr);
            l >>= 1, r >>= 1;
        }
        return MX::op(xl, xr);
    }

    /** Returns the monoid product over the whole array. Time: O(1). */
    X prod_all() { return data[1]; }

    /** Applies action a to every element in [l, r). Time: O(log n). */
    void apply(int l, int r, A a) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return;
        l += size, r += size;
        // push top-down along the two boundary root-to-leaf paths
        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r-1) >> i);
        }
        int l2 = l, r2 = r;
        // apply the A act operation
        while (l < r) {
            if (l&1) apply_at(l++, a);
            if (r&1) apply_at(--r, a);
            l >>= 1, r >>= 1;
        }
        l = l2, r = r2;
        // recompute affected ancestors on boundary paths (those whose children changed)
        for (int i = 1; i <= log; i++) {
            if (((l >> i) << i) != l) update(l>>i);
            if (((r >> i) << i) != r) update((r-1) >> i);
        }
    }

    /** Returns the maximum r such that check(prod(l, r)) is true. Time: O(log n). */
    template<typename F>
    int max_right(const F check, int l) {
        assert(0 <= l && l <= n);
        assert(check(MX::id()));
        if (l == n) return n;
        l += size;
        for (int i = log; i >= 1; i--) push(l>>i);
        X sm = MX::id();
        do {
            while (l%2 == 0) l >>= 1;
            if (!check(MX::op(sm, data[l]))) {
                while (l < size) {
                    push(l);
                    l = 2*l;
                    if (check(MX::op(sm, data[l]))) { sm = MX::op(sm, data[l++]); }
                }
                return l - size;
            }
            sm = MX::op(sm, data[l++]);
        } while ((l & -l) != l);
        return n;
    }

    /** Returns the minimum l such that check(prod(l, r)) is true. Time: O(log n). */
    template<typename F>
    int min_left(const F check, int r) {
        assert(0 <= r && r <= n);
        assert(check(MX::id()));
        if (r == 0) return 0;
        r += size;
        for (int i = log; i >= 1; i--) push((r-1) >> i);
        X sm = MX::id();
        do {
            r--;
            while (r > 1 && (r%2)) r >>= 1;
            if (!check(MX::op(data[r], sm))) {
                while (r < size) {
                    push(r);
                    r = r<<1|1;
                    if (check(MX::op(sm, data[r]))) { sm = MX::op(sm, data[r--]); }
                }
                return r + 1 - size;
            }
            sm = MX::op(sm, data[r]);
        } while ((r & -r) != r);
        return 0;
    }

private:
    // apply act() at node p, if it's internal node -> store lazy tag
    void apply_at(int p, A a) {
        ll _sz = 1 << (log - topbit(p));
        data[p] = AM::act(data[p], a, _sz);
        if (p < size) lazy[p] = MA::op(lazy[p], a);
    }

    // propagate p's pending lazy action to its children and clear it
    void push(int p) {
        if (lazy[p] == MA::id()) return;
        apply_at(p<<1, lazy[p]), apply_at(p<<1|1, lazy[p]);
        lazy[p] = MA::id();
    }
};


/** Additive monoid used by this specialized HLD snippet. */
template<class E>
struct HLD_Monoid_Add {
    using value_type = E;
    static constexpr E op(const E& a, const E& b) { return a + b; }
    static constexpr E id() { return E(0); }
};

/** Range-add action on range-sum aggregates. */
template<class E>
struct HLD_ActedMonoid_Sum_Add {
    using Monoid_X = HLD_Monoid_Add<E>;
    using Monoid_A = HLD_Monoid_Add<E>;
    using X = E;
    using A = E;
    static constexpr X act(const X& x, const A& a, const long long& size) { return x + a * E(size); }
};

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
    using AM = HLD_ActedMonoid_Sum_Add<ll>;
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
