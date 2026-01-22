#pragma once
#include "../../linalg/xor_basis.h"

struct SegTree {
    int n;
    vector<XorBasis<60>> t;

    SegTree(const vector<ll>& a) {
        n = sz(a);
        t.resize(2 * n);
        for (int i = 0; i < n; i++) t[n+i].insert(a[i]);
        for (int i = n-1; i > 0; i--) {
            t[i] = t[i<<1];
            t[i].merge_basis(t[i<<1|1]);
        }
    }

    // [l, r)
    XorBasis<60> query(int l, int r) {
        XorBasis<60> res;
        for (l+=n, r+=n; l<r; l>>=1, r>>=1) {
            if (l&1) { res.merge_basis(t[l++]); }
            if (r&1) { res.merge_basis(t[--r]); }
        }
        return res;
    }
};
