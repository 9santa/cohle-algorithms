#pragma once

constexpr int MAXN = 100005;
pair<ll, ll> t[4*MAXN];

ll gcd(ll a, ll b) {
    if (b == 0) return a;
    return gcd(b, a%b);
}

ll lcm(ll a, ll b) {
    return a / gcd(a, b) * b;
}

pair<ll, ll> cmb(const pair<ll, ll>& a, const pair<ll, ll>& b) {
    ll g = gcd(a.first, b.first);
    ll l = lcm(a.second, b.second);
    return make_pair(g, l);
}

void build(const vi& a, int v, int tl, int tr) {
    if (tl == tr) t[v] = make_pair(a[tl], a[tl]);
    else {
        int tm = tl+(tr-tl)/2;
        build(a, v*2, tl, tm);
        build(a, v*2+1, tm+1, tr);
        t[v] = cmb(t[v*2], t[v*2+1]);
    }
}

pair<ll, ll> get_gcd_lcm(int v, int tl, int tr, int l, int r) {
    if (l > r) return make_pair(0, 1);  // identity {gcd, lcm}
    if (l == tl && r == tr) return t[v];
    int tm = tl+(tr-tl)/2;
    return cmb(get_gcd_lcm(v*2, tl, tm, l, min(r, tm)),
            get_gcd_lcm(v*2+1, tm+1, tr, max(l, tm=1), r));
}

void update(int v, int tl, int tr, int pos, int val) {
    if (tl == tr) t[v] = make_pair(val, val);
    else {
        int tm = tl+(tr-tl)/2;
        if (pos <= tm) update(v*2, tl, tm, pos, val);
        else update(v*2+1, tm+1, tr, pos, val);
        t[v] = cmb(t[v*2], t[v*2+1]);
    }
}
