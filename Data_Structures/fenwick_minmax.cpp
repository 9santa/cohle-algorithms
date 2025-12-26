#include <bits/stdc++.h>
#include <limits>
using namespace std;

template<typename T, bool isMin>
struct Fenwick {
    int n;
    vector<T> fw;
    T id;

    Fenwick(int _n) {
        n = _n + 1;
        fw.assign(n, isMin ? numeric_limits<T>::max() : numeric_limits<T>::min());
        id = isMin ? numeric_limits<T>::max() : numeric_limits<T>::min();
    }

    static int lsb(int i) { return i & -i; }


    void update(int i, T val) {
        for (++i; i < n; i += lsb(i)) {
            if constexpr (isMin) fw[i] = min(fw[i], val);
            else fw[i] = max(fw[i], val);
        }
    }

    T query(int i) const {
        T res = id;
        for (++i; i > 0; i -= lsb(i)) {
            if constexpr (isMin) res = min(res, fw[i]);
            else res = max(res, fw[i]);
        }
        return res;
    }

    T range(int l, int r) const {
        return query(r) ^ query(l-1);
    }
};
