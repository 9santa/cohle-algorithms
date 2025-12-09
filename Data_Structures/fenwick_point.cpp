#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define sz(x) int((x).size())

template<class T>
struct Fenwick {
    int n;
    vector<T> fw;

    Fenwick(int _n) { n = _n+1; fw.assign(n, 0); }

    static int lsb(int i) { return i & -i; }

    void build(const vector<T>& a) {
        n = sz(a) + 1;
        fw.assign(n, 0);
        for (int i = 1; i < n; i++) {
            fw[i] += a[i-1];
            if (i + lsb(i) <= sz(a)) {
                fw[i + lsb(i)] += fw[i];
            }
        }
    }

    void update(int i, T val) {
        for (++i; i < n; i += lsb(i)) {
            fw[i] += val;
        }
    }

    void updateRange(int l, int r, T val) {
        update(l, val);
        update(r+1, -val);
    }

    T get(int i) {
        T res = 0;
        for (++i; i > 0; i -= lsb(i)) {
            res += fw[i];
        }
        return res;
    }

    T getRange(int l, int r) {
        return get(r) - get(l-1);
    }

    // 0-based index of first prefix sum >= x
    T lower_bound(T x) {
        int pos = 0;
        ll sum = 0;
        for (int i = 31-__builtin_clz(n); i >= 0; i--) {
            int nxt = pos + (1 << i);
            if (nxt < n && sum + fw[nxt] < x) {
                sum += fw[nxt];
                pos = nxt;
            }
        }
        return pos;
    }
};
