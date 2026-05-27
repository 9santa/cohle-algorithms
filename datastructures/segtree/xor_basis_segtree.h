#pragma once
#include "../core.h"
#include "../../linalg/xor_basis.h"

/** Segment tree storing xor bases for range linear-basis queries. Space: O(60n). */
struct SegTree {
    int n;
    vector<XorBasis<60>> t;

    /** Builds from a 0-indexed array. Time: O(60n). */
    SegTree(const vector<ll>& a) {
        n = sz(a);
        t.resize(2 * n);
        for (int i = 0; i < n; i++) t[n+i].insert(a[i]);
        for (int i = n-1; i > 0; i--) {
            t[i] = t[i<<1];
            t[i].merge_basis(t[i<<1|1]);
        }
    }

    /** Returns the xor basis of values in [l, r). Time: O(60 log n). */
    XorBasis<60> query(int l, int r) {
        XorBasis<60> res;
        for (l+=n, r+=n; l<r; l>>=1, r>>=1) {
            if (l&1) { res.merge_basis(t[l++]); }
            if (r&1) { res.merge_basis(t[--r]); }
        }
        return res;
    }
};
