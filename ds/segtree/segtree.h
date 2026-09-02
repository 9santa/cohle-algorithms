#pragma once
#include <bits/stdc++.h>
using namespace std;

template<class T> using V = vector<T>;
using vl = vector<long long>;
#define sz(x) int((x).size())
#define F0R(i, n) for (int i = 0; i < (n); ++i)
#define ROF(i, a, b) for (int i = (b) - 1; i >= (a); --i)

/** Generic iterative segment tree over an associative monoid. Space: O(n). */
template<class Monoid>
struct SegTree {
    using X = typename Monoid::value_type;
    V<X> dat;
    int n, log, size;

    SegTree() {}
    SegTree(int n) { build(n); }
    template<typename F>
    SegTree(int n, F f) {
        build(n, f);
    }
    SegTree(const V<X>& v) { build(v); }

    /** Builds a tree of size m filled with the monoid identity. Time: O(n). */
    void build(int m) {
        build(m, [](int i) -> X { return Monoid::id(); });
    }

    /** Builds the tree from a 0-indexed array. Time: O(n). */
    void build(const V<X>& v) {
        build(sz(v), [&](int i) -> X { return v[i]; });
    }

    /** Builds the tree from values f(i). Time: O(n). */
    template<typename F>
    void build(int m, F f) {
        n = m, log = 1;
        while ((1 << log) < n) log++;
        size = 1 << log;
        dat.assign(size << 1, Monoid::id());
        F0R(i, n) dat[size + i] = f(i);
        ROF(i, 1, size) update(i);
    }

    /** Returns a[i]. Time: O(1). */
    X get(int i) const { return dat[size+i]; }

    /** Returns all stored values in index order. Time: O(n). */
    V<X> get_all() const { return {dat.begin() + size, dat.begin() + size + n}; }

    void update(int i) { dat[i] = Monoid::op(dat[2*i], dat[2*i+1]); }

    /** Sets a[i] to x. Time: O(log n). */
    void set(int i, const X& x) {
        assert(i < n);
        dat[i += size] = x;
        while (i >>= 1) update(i);
    }

    /** Returns the monoid product over [L, R). Time: O(log n). */
    X prod(int L, int R) {
        assert(0 <= L && L <= R && R <= n);
        X vl = Monoid::id(), vr = Monoid::id();
        L += size, R += size;
        while (L < R) {
            if (L & 1) vl = Monoid::op(vl, dat[L++]);
            if (R & 1) vr = Monoid::op(dat[--R], vr);
            L >>= 1, R >>= 1;
        }
        return Monoid::op(vl, vr);
    }
};
