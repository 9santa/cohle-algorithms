#include <bits/stdc++.h>
using namespace std;

template<typename T>
struct Fenwick {
    int n;
    vector<T> fw;

    Fenwick(int _n) { n = _n + 1; fw.assign(n, 0); }

    static int lsb(int i) { return i & -i; }


    void update(int i, T val) {
        for (++i; i < n; i += lsb(i)) fw[i] ^= val;
    }

    T query(int i) const {
        T res = 0;
        for (++i; i > 0; i -= lsb(i)) res ^= fw[i];
        return res;
    }

    T range(int l, int r) const {
        return query(r) ^ query(l-1);
    }
};
