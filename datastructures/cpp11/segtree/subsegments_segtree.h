#pragma once

struct data {
    int sum, pref, suf, ans;
};

constexpr int MAXN = 100005;
vector<data> t(MAXN);

data make_data(int val) {
    data res;
    res.sum = val;
    res.pref = res.suf = res.ans = max(0, val);
}

data cmb(data l, data r) {
    data res;
    res.sum = l.sum + r.sum;
    res.pref = max(l.pref, l.sum + r.pref);
    res.suf = max(r.suf, r.sum + l.suf);
    res.ans = max(max(l.ans, r.ans), l.suf + r.pref);
}

void build(const vi& a, int v, int tl, int tr) {
    if (tl == tr) t[v] = make_data(a[tl]);
    else {
        int tm = tl+(tr-tl)/2;
        build(a, v*2, tl, tm);
        build(a, v*2+1, tm+1, tr);
        t[v] = cmb(t[v*2], t[v*2+1]);
    }
}

void update(int v, int tl, int tr, int pos, int val) {
    if (tl == tr) t[v] = make_data(val);
    else {
        int tm = tl+(tr-tl)/2;
        if (pos <= tm) update(v*2, tl, tm, pos, val);
        else update(v*2+1, tm+1, tr, pos, val);
        t[v] = cmb(t[v*2], t[v*2+1]);
    }
}

data query(int v, int tl, int tr, int l, int r) {
    if (l > r) return make_data(0);
    if (l == tl && r == tr) return t[v];
    int tm = tl+(tr-tl)/2;
    return cmb(query(v*2, tl, tm, l, min(r, tm)),
            query(v*2+1, tm+1, tr, max(l, tm+1), r));
}

