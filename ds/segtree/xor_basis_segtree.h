#pragma once
#include <bits/stdc++.h>
using namespace std;

using u64 = unsigned long long;
using ll = long long;
using u32 = unsigned int;
#define sz(x) int((x).size())

/*
Problems like:
- maximum subset xor
- can represent x
- count subsets with xor x
- kth element in xor-span
- merge basis on trees / DSU on tree / divide and conquer
*/

/** Linear xor basis over fixed-width unsigned values. Space: O(BITS). */
template<int BITS = 64, typename T = u64>
struct XorBasis {
    T basis[BITS] = {0};   // basis[i] stores a number whose highest set bit is 'i'
    u32 inserted = 0;      // total elements inserted
    int rk = 0;            // rank

    /** Clears all inserted vectors and rank state. Time: O(BITS). */
    void clear() {
        basis = {0};
        inserted = 0;
        rk = 0;
    }

    /** Returns whether the span has rank zero. Time: O(1). */
    bool empty() const { return rk == 0; }
    /** Returns current basis rank. Time: O(1). */
    int rank() const { return rk; }

    // insert into the span only; does NOT change 'inserted'
    // O(BITS)
    /** Inserts a vector into the span without changing inserted count. Time: O(BITS). */
    bool insert_vector(T x) {
        for (int i = BITS-1; i >= 0; i--) {
            if (((x >> i) & 1) == 0) continue;
            if (!basis[i]) {
                basis[i] = x; // x is the highest number with set bit at pos 'i' in the current basis (pivot for i)
                rk++;
                return true;
            }
            x ^= basis[i]; // eliminate bit 'i' if basis[i] exists
        }
        return false;
    }

    // insert one original vector into the multiset
    // O(BITS)
    /** Inserts one original vector and updates inserted count. Time: O(BITS). */
    inline bool insert(T x) {
        inserted++;
        return insert_vector(x);

    }

    /** Merges another basis into this basis. Time: O(BITS^2). */
    inline void merge_from(const XorBasis& other) {
        for (int i = 0; i < BITS; i++) {
            if (other.basis[i]) insert_vector(other.basis[i]);
        }
    }


    // maximize seed xor y, where y is in span
    /** Maximizes seed xor y over y in the span. Time: O(BITS). */
    T max_xor(T seed = 0) const {
        T res = seed;
        for (int i = BITS-1; i >= 0; i--) {
            if (!basis[i]) continue;
            res = max(res, T(res ^ basis[i]));
        }
        return res;
    }

    // minimize seed xor y, where y is in span
    /** Minimizes seed xor y over y in the span. Time: O(BITS). */
    T min_xor(T seed = 0) const {
        T res = seed;
        for (int i = BITS-1; i >= 0; i--) {
            if (!basis[i]) continue;
            res = min(res, T(res ^ basis[i]));
        }
        return res;
    }

    // Reduces basis in increasing pivot order.
    // Good for kth_smallest()
    /** Returns a reduced basis ordered by increasing pivot. Time: O(BITS^2). */
    vector<T> reduces_basis() const {
        auto tmp = *this;

        for (int i = 0; i < BITS; i++) {
            if (!tmp.basis[i]) continue;
            // remove bits lower than pivot
            for (int j = i + 1; j < BITS; j++) {
                if (tmp.basis[j] && ((tmp.basis[j] >> i) & 1)) {
                    tmp.basis[j] ^= tmp.basis[i];
                }
            }
        }

        vector<T> v;
        v.reserve(tmp.rk);
        for (int i = 0; i < BITS; i++) {
            if (tmp.basis[i]) v.push_back(tmp.basis[i]);
        }
        return v;
    }

    /** Returns the minimum nonzero value in the span, or 0 if empty. Time: O(BITS^2). */
    T min_nonzero() const {
        if (rk == 0) return 0;
        auto v = reduces_basis();
        T ans = std::numeric_limits<T>::max();
        for (auto x : v) ans = min(ans, x);
        return ans;
    }

    // kth smallest value in the span, 0-indexed, INCLUDING 0
    // valid when k < 2^rank
    /** Returns the k-th smallest span value, including zero. Time: O(BITS^2). */
    T kth_smallest(u64 k) const {
        auto v = reduces_basis();
        T res = 0;
        for (int i = 0; i < sz(v); i++) {
            if (k & (1ULL << i))
                res ^= v[i];
        }
        return res;
    }

    // returns true if 'x' can be formed as the XOR of some subset of the basis
    /** Returns whether x is representable by the basis. Time: O(BITS). */
    bool can(T x) const {
        for (int i = BITS-1; i >= 0; i--) {
            if (x & (1LL << i)) {
                if (!basis[i]) return false;
                x ^= basis[i];
            }
        }
        return true;
    }

    // span{} size
    /** Returns span size, or -1 if it does not fit in signed 64-bit. Time: O(1). */
    ll span_size() const {
        if (rk >= 63) return -1;
        return (1LL << rk);
    }

    // applies bitwise AND with 'x' to every value in the basis, then rebuilds the basis
    /** Applies bitwise AND to basis vectors and rebuilds. Time: O(BITS^2). */
    void updateAnd(ll x) {
        vector<ll> v;
        for (int i = 0; i < BITS; i++) {
            if (basis[i]) {
                v.push_back(basis[i] & x);
                basis[i] = 0;
            }
        }
        for (const auto& el : v) insert(el);
    }

    // O(BITS^2)
    /** Merges only the span of another basis. Time: O(BITS^2). */
    void merge_span(const XorBasis& other) {
        for (int i = 0; i < BITS; i++) {
            if (other.basis[i]) insert_vector(other.basis[i]);
        }
    }

    /** Counts original subsets with xor x, or -1 on overflow. Time: O(BITS). */
    ll count_subsets(T x) const {
        if (!can(x)) return 0;
        ll free_vars = inserted - rk;
        if (free_vars >= 63) return -1;
        return (1LL << free_vars);
    }

    /** Returns nonzero basis vectors in descending pivot order. Time: O(BITS). */
    vector<T> raw_basis_desc() const {
        vector<T> v;
        for (int i = BITS - 1; i >= 0; i--) {
            if (basis[i]) v.push_back(basis[i]);
        }
        return v;
    }
};


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
            t[i].merge_from(t[i<<1|1]);
        }
    }

    /** Returns the xor basis of values in [l, r). Time: O(60 log n). */
    XorBasis<60> query(int l, int r) {
        XorBasis<60> res;
        for (l+=n, r+=n; l<r; l>>=1, r>>=1) {
            if (l&1) { res.merge_from(t[l++]); }
            if (r&1) { res.merge_from(t[--r]); }
        }
        return res;
    }
};
