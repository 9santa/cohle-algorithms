#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Fenwick tree for prefix minimum or maximum queries. Space: O(n). */
template<typename T, bool isMin>
struct Fenwick {
    int n;
    vector<T> fw;
    T id;

    /** Creates an empty min tree when isMin is true, otherwise a max tree. */
    Fenwick(int _n) {
        n = _n + 1;
        fw.assign(n, isMin ? numeric_limits<T>::max() : numeric_limits<T>::min());
        id = isMin ? numeric_limits<T>::max() : numeric_limits<T>::min();
    }

    /** Returns the least significant set bit of i. */
    static int lsb(int i) { return i & -i; }

    /** Applies min/max update at index i, depending on isMin. Time: O(log n). */
    void update(int i, T val) {
        for (++i; i < n; i += lsb(i)) {
            if constexpr (isMin) fw[i] = min(fw[i], val);
            else fw[i] = max(fw[i], val);
        }
    }

    /** Returns the min/max value over [0, i], depending on isMin. Time: O(log n). */
    T query(int i) const {
        T res = id;
        for (++i; i > 0; i -= lsb(i)) {
            if constexpr (isMin) res = min(res, fw[i]);
            else res = max(res, fw[i]);
        }
        return res;
    }

};
