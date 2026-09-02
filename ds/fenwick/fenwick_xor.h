#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Fenwick tree for point xor updates and range xor queries. Space: O(n). */
template<typename T>
struct Fenwick {
    int n;
    vector<T> fw;

    /** Creates an empty tree with the given number of elements. */
    Fenwick(int _n) { n = _n + 1; fw.assign(n, 0); }

    /** Returns the least significant set bit of i. */
    static int lsb(int i) { return i & -i; }

    /** Applies a[i] ^= val. Time: O(log n). */
    void update(int i, T val) {
        for (++i; i < n; i += lsb(i)) fw[i] ^= val;
    }

    /** Returns the prefix xor over [0, i]. Time: O(log n). */
    T query(int i) const {
        T res = 0;
        for (++i; i > 0; i -= lsb(i)) res ^= fw[i];
        return res;
    }

    /** Returns the range xor over [l, r]. Time: O(log n). */
    T range(int l, int r) const {
        return query(r) ^ query(l-1);
    }

    /** Returns a[i]. Time: O(log n). */
    T get(int i) const {
        return range(i, i);
    }

    /** Sets a[i] to val. Time: O(log n). */
    void set(int i, T val) {
        T cur = get(i);
        update(i, cur ^ val);
    }
};
