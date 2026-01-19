#pragma once

// Counting SegTree for frequency of some value 'x'
constexpr int MAXN = 100005;
vi t(4*MAXN);

void build(const vi& a, int v, int tl, int tr, int x) {
    if (tl == tr) t[v] = (a[tl] == x ? 1 : 0);
    else {
        int tm = tl+(tr-tl)/2;
        build(a, v*2, tl, tm, x);
        build(a, v*2+1, tm+1, tr, x);
        t[v] = t[v*2] + t[v*2+1];
    }
}

int sum(int v, int tl, int tr, int l, int r) {
    if (l > r) return 0;
    if (l == tl && r == tr) return t[v];
    int tm = tl+(tr-tl)/2;
    return sum(v*2, tl, tm, l, min(r, tm))
        + sum(v*2+1, tm+1, tr, max(l, tm+1), r);
}

void update(int v, int tl, int tr, int pos, int val) {
    if (tl == tr) t[v] = val;
    else {
        int tm = tl+(tr-tl)/2;
        if (pos <= tm) {
            update(v*2, tl, tm, pos, val);
        } else {
            update(v*2+1, tm+1, tr, pos, val);
        }
        t[v] = t[v*2] + t[v*2+1];
    }
}

// returns the index of k-th 'x', or -1 if it doesn't exist
int find_kth(int v, int tl, int tr, int k) {
    if (k > t[v]) return -1;
    if (tl == tr) return tl;
    int tm = tl+(tr-tl)/2;
    if (t[v*2] >= k) return find_kth(v*2, tl, tm, k);
    else return find_kth(v*2+1, tm+1, tr, k - t[v*2]);
}
