#pragma once

#include "core.h"

/*
Problems like:
- maximum subset xor
- can represent x
- count subsets with xor x
- kth element in xor-span
- merge basis on trees / DSU on tree / divide and conquer
*/

template<int BITS = 64, typename T = u64>
struct XorBasis {
    T basis[BITS] = {0};   // basis[i] stores a number whose highest set bit is 'i'
    u32 inserted = 0;      // total elements inserted
    int rk = 0;            // rank

    void clear() {
        basis = {0};
        inserted = 0;
        rk = 0;
    }

    bool empty() const { return rk == 0; }
    int rank() const { return rk; }

    // insert into the span only; does NOT change 'inserted'
    // O(BITS)
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
    inline bool insert(T x) {
        inserted++;
        return insert_vector(x);

    }
 
    inline void merge_from(const XorBasis& other) {
        for (int i = 0; i < BITS; i++) {
            if (other.basis[i]) add_vec(other.basis[i]);
        }
    }
 
 
    // maximize seed xor y, where y is in span
    T max_xor(T seed = 0) const {
        T res = seed;
        for (int i = BITS-1; i >= 0; i--) {
            if (!basis[i]) continue;
            res = max(res, T(res ^ basis[i]));
        }
        return res;
    }
 
    // minimize seed xor y, where y is in span
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

    T min_nonzero() const {
        if (rk == 0) return 0;
        auto v = reduces_basis();
        T ans = std::numeric_limits<T>::max();
        for (auto x : v) ans = min(ans, x);
        return ans;
    }

    // kth smallest value in the span, 0-indexed, INCLUDING 0
    // valid when k < 2^rank
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
    ll span_size() const {
        if (rk >= 63) return -1;
        return (1LL << rk);
    }
 
    // applies bitwise AND with 'x' to every value in the basis, then rebuilds the basis
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
    void merge_span(const XorBasis& other) {
        for (int i = 0; i < BITS; i++) {
            if (other.basis[i]) insert_vector(other.basis[i]);
        }
    }
 
    ll count_subsets(T x) const {
        if (!can(x)) return 0;
        ll free_vars = inserted - rk;
        if (free_vars >= 63) return -1;
        return (1LL << free_vars);
    }

    vector<T> raw_basis_desc() const {
        vector<T> v;
        for (int i = BITS - 1; i >= 0; i--) {
            if (basis[i]) v.push_back(basis[i]);
        }
        return v;
    }
};
