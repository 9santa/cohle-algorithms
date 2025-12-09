#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define sz(x) int((x).size())

template<class T>
struct Fenwick {
    int n;
    vector<T> fw1, fw2;

    Fenwick(int _n) { n = _n+1; fw1.assign(n, 0); fw2.assign(n, 0); }

    static int lsb(int i) { return i & -i; }

    void update(vector<T>& fw, int i, T val) {
        for (++i; i < n; i += lsb(i)) fw[i] += val;
    }

    T query(const vector<T>& fw, int i) const {
        T res = 0;
        for (++i; i > 0; i -= lsb(i)) res += fw[i];
        return res;
    }

    void updateRange(int l, int r, T val) {
        update(fw1, l, val); update(fw1, r+1, -val);
        update(fw2, l, val*(l-1)); update(fw2, r+1, -val*r);
    }

    T prefix(int i) const {
        return query(fw1, i)*i - query(fw2, i);
    }

    T prefixRange(int l, int r) const {
        return prefix(r) - prefix(l-1);
    }
};
