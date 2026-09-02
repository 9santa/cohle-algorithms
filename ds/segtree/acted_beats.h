#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
template<class T> using V = vector<T>;
#define sz(x) int((x).size())
#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define F0R(i, n) for (int i = 0; i < (n); ++i)
inline int topbit(int x) { return x == 0 ? -1 : 31 - __builtin_clz((unsigned)x); }
inline int topbit(unsigned x) { return x == 0 ? -1 : 31 - __builtin_clz(x); }
inline int topbit(long long x) { return x == 0 ? -1 : 63 - __builtin_clzll((unsigned long long)x); }
inline int topbit(unsigned long long x) { return x == 0 ? -1 : 63 - __builtin_clzll(x); }

/** Lazy segment tree for acted monoids with segment-tree-beats style failures. Space: O(n). */
template<typename ActedMonoid>
struct Beats_Segtree {
    using AM = ActedMonoid;
    using MX = typename AM::Monoid_X;
    using MA = typename AM::Monoid_A;
    using X = typename MX::value_type;
    using A = typename MA::value_type;
    int n, log, size;   // n = array size, size = actual segtree size
    V<X> data;
    V<A> lazy;

    Beats_Segtree() {}
    Beats_Segtree(int n) { build(n); }
    template<typename F>
    Beats_Segtree(int n, F f) { build(n, f); }
    Beats_Segtree(const V<X>& v) { build(v); }

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

    /** Sets a[p] to x. Time: O(log n) amortized. */
    void set(int p, X x) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = log; i >= 1; i--) push(p>>i);
        data[p] = x;
        for (int i = 1; i <= log; i++) update(p>>i);
    }

    /** Replaces a[p] by Monoid_X::op(a[p], x). Time: O(log n) amortized. */
    void multiply(int p, X x) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = log; i >= 1; i--) push(p>>i);
        data[p] = MX::op(data[p], x);
        for (int i = 1; i <= log; i++) update(p>>i);
    }

    /** Returns a[p]. Time: O(log n) amortized. */
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

    /** Returns the monoid product over [l, r). Time: O(log n) amortized. */
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

    /** Applies action a to every element in [l, r). Time: O(log n) amortized. */
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

    /** Returns the maximum r such that check(prod(l, r)) is true. Time: O(log n) amortized. */
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

    /** Returns the minimum l such that check(prod(l, r)) is true. Time: O(log n) amortized. */
    template<typename F>
    int min_left(const F check, int r) {
        assert(0 <= r && r <= n);
        assert(check(MX::id()));
        if (r == 0) return 0;
        r += size;
        for (int i = log; i >= 1; i++) push((r-1) >> i);
        X sm = MX::id();
        do {
            r--;
            while (r > 1 && (r%2)) r >>= 1;
            if (!check(MX::op(sm, data[r]))) {
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
        if (p < size) {
            lazy[p] = MA::op(lazy[p], a);
            if (data[p].fail) push(p), update(p);
        }
    }

    // propagate p's pending lazy action to its children and clear it
    void push(int p) {
        if (lazy[p] == MA::id()) return;
        apply_at(p<<1, lazy[p]), apply_at(p<<1|1, lazy[p]);
        lazy[p] = MA::id();
    }
};
