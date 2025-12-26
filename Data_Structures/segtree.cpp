#include "../header.h"

namespace default_segtree {
struct segtree {
    int n;
    vi t;

    void init(int _n) {
        for (n = 1; n < _n; n *= 2);
        t.resize(2*n);
    }

    void build(const vi& a, int v, int tl, int tr) {
        if (tl == tr) t[v] = a[tl];
        else {
            int tm = tl+(tr-tl)/2;
            build(a, v*2, tl, tm);
            build(a, v*2+1, tm+1, tr);
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
};
} // namespace default_segtree

namespace max_min {
constexpr int MAXN = 100005;
pair<int, int> t[4*MAXN];

pair<int, int> cmb(const pair<int, int> &a, const pair<int, int> &b) {
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
} // namespace max_min

namespace gcd_lcm {
constexpr int MAXN = 100005;
pair<ll, ll> t[4*MAXN];

ll gcd(ll a, ll b) {
    if (b == 0) return a;
    return gcd(b, a%b);
}

ll lcm(ll a, ll b) {
    return a / gcd(a, b) * b;
}

pair<ll, ll> cmb(const pair<ll, ll> &a, const pair<ll, ll> &b) {
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

pair<ll, ll> get_gl(int v, int tl, int tr, int l, int r) {
    if (l > r) return make_pair(0, 1); // identity {gcd, lcm}
    if (l == tl && r == tr) return t[v];
    int tm = tl+(tr-tl)/2;
    return cmb(get_gl(v*2, tl, tm, l, min(r, tm)),
           get_gl(v*2+1, tm+1, tr, max(l, tm+1), r));
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
} // namespace gcd_lcm

// Counting Segment Tree
namespace kth_occurance {
constexpr int MAXN = 100005;
vi t(4*MAXN);

void build(const vi& a, int v, int tl, int tr) {
    if (tl == tr) t[v] = (a[tl] == 0 ? 1 : 0);
    else {
        int tm = tl+(tr-tl)/2;
        build(a, v*2, tl, tm);
        build(a, v*2+1, tm+1, tr);
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

// returns the index of k-th zero, or -1 if it doesn't exist
int find_kth(int v, int tl, int tr, int k) {
    if (k > t[v]) return -1;
    if (tl == tr) return tl;
    int tm = tl+(tr-tl)/2;
    if (t[v*2] >= k) return find_kth(v*2, tl, tm, k);
    else return find_kth(v*2+1, tm+1, tr, k - t[v*2]);
}
} // namespace kth_occurance


// Searching for the first element greater than a given amount
namespace first_element {
int get_first(int v, int tl, int tr, int l, int r, int x) {
    if(tl > r || tr < l) return -1;
    if(t[v] <= x) return -1;

    if (tl== tr) return tl;

    int tm = tl + (tr-tl)/2;
    int left = get_first(2*v, tl, tm, l, r, x);
    if(left != -1) return left;
    return get_first(2*v+1, tm+1, tr, l ,r, x);
}
} // first_element


namespace subsegments {
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
} // namespace subsegments
