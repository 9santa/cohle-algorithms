#pragma once

constexpr int MAXN = 100005;
pair<int, int> t[4*MAXN];

// pair {max, occurences}
pair<int, int> cmb(const pair<int, int>& a, const pair<int, int>& b) {
    if (a.first > b.first) return a;
    if (b.first > a.first) return b;
    return make_pair(a.first, a.second + b.second);
}

void build(const vi& a, int v, int tl, int tr) {
    if (tl == tr) t[v] = make_pair(a[tl], 1);
    else {
        int tm = tl+(tr-tl)/2;
        build(a, v*2, tl, tm);
        build(a, v*2+1, tm+1, tr);
        t[v] = cmb(t[v*2], t[v*2+1]);
    }
}

pair<int, int> get_max(int v, int tl, int tr, int l, int r) {
    if (l > r) return make_pair(-infty<int>, 0);
    if (l == tl && r == tr) return t[v];
    int tm = tl+(tr-tl)/2;
    return cmb(get_max(v*2, tl, tm, l, min(r, tm)),
            get_max(v*2+1, tm+1, tr, max(l, tm+1), r));
}

void update(int v, int tl, int tr, int pos, int val) {
    if (tl == tr) t[v] = make_pair(val, 1);
    else {
        int tm = tl+(tr-tl)/2;
        if (pos <= tm) update(v*2, tl, tm, pos, val);
        else update(v*2+1, tm+1, tr, pos, val);
        t[v] = cmb(t[v*2], t[v*2+1]);
    }
}
