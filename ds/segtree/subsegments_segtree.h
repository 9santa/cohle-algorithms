#pragma once
#include <bits/stdc++.h>
using namespace std;

using vi = vector<int>;

/** Segment value for maximum subarray sum queries. */
struct MaxSubarrayNode {
    int sum, pref, suf, ans;
};

constexpr int MAXN = 100005;
vector<MaxSubarrayNode> t(MAXN);

/** Creates a leaf value. Time: O(1). */
MaxSubarrayNode make_data(int val) {
    MaxSubarrayNode res;
    res.sum = val;
    res.pref = res.suf = res.ans = max(0, val);
    return res;
}

/** Combines two adjacent segment values. Time: O(1). */
MaxSubarrayNode cmb(MaxSubarrayNode l, MaxSubarrayNode r) {
    MaxSubarrayNode res;
    res.sum = l.sum + r.sum;
    res.pref = max(l.pref, l.sum + r.pref);
    res.suf = max(r.suf, r.sum + l.suf);
    res.ans = max(max(l.ans, r.ans), l.suf + r.pref);
    return res;
}

/** Builds a maximum subarray sum segment tree. Time: O(n). Space: O(MAXN). */
void build(const vi& a, int v, int tl, int tr) {
    if (tl == tr) t[v] = make_data(a[tl]);
    else {
        int tm = tl+(tr-tl)/2;
        build(a, v*2, tl, tm);
        build(a, v*2+1, tm+1, tr);
        t[v] = cmb(t[v*2], t[v*2+1]);
    }
}

/** Sets a[pos] to val. Time: O(log n). */
void update(int v, int tl, int tr, int pos, int val) {
    if (tl == tr) t[v] = make_data(val);
    else {
        int tm = tl+(tr-tl)/2;
        if (pos <= tm) update(v*2, tl, tm, pos, val);
        else update(v*2+1, tm+1, tr, pos, val);
        t[v] = cmb(t[v*2], t[v*2+1]);
    }
}

/** Returns maximum subarray data over [l, r]. Time: O(log n). */
MaxSubarrayNode query(int v, int tl, int tr, int l, int r) {
    if (l > r) return make_data(0);
    if (l == tl && r == tr) return t[v];
    int tm = tl+(tr-tl)/2;
    return cmb(query(v*2, tl, tm, l, min(r, tm)),
            query(v*2+1, tm+1, tr, max(l, tm+1), r));
}
