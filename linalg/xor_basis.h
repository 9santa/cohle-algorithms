#pragma once
#include "../header.h"

template<int BITS = 60>
struct XorBasis {
    ll basis[BITS] = {0};  // basis[i] stores a number whose highest set bit is 'i'
    ll total = 0;         // total elements inserted
    ll size = 0;          // basis size (rank)

    // O(BITS)
    void insert(ll x) {
        total++;
        for (int i = BITS-1; i >= 0; i--) {
            if (!(x & (1LL << i))) continue;
            if (!basis[i]) {
                basis[i] = x; // x is the highest number with set bit at pos 'i' in the current basis (pivot for i)
                size++;
                return;
            }
            else if (basis[i]) x ^= basis[i]; // eliminate bit 'i' if basis[i] exists
        }
    }

    ll max_xor() const {
        ll res = 0;
        for (int i = BITS-1; i >= 0; i--) {
            res = max(res, res ^ basis[i]);
        }
        return res;
    }

    ll min_xor() const {
        ll res = 0;
        for (int i = 0; i < BITS; i++) {
            if (basis[i])
                res = min(res, res ^ basis[i]);
        }
        return res;
    }

    // returns true if 'x' can be formed as the XOR of some subset of the basis
    bool can(ll x) const {
        for (int i = BITS-1; i >= 0; i--) {
            if (x & (1LL << i)) {
                if (!basis[i]) return false;
                x ^= basis[i];
            }
        }
        return true;
    }

    // rank (basis size)
    ll rank() const {
        int r = 0;
        F0R(i, BITS) if (basis[i]) r++;
        return r;
    }

    // span{} size
    ll span_size() const {
        return (1LL << rank());
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
        size = 0;
        for (const auto& el : v) insert(el);
    }

    vector<ll> to_reduced_vector() {
        vector<ll> res;
        for (int i = BITS-1; i >= 0; i--) {
            if (basis[i]) {
                // remove bits lower than pivot
                for (auto& x : res) x = min(x, x ^ basis[i]);
                res.push_back(basis[i]);
            }
        }
        return res;
    }

    ll kth_smallest(ll k) {
        ll res = 0;
        auto b = to_reduced_vector();
        for (int i = 0; i < sz(b); i++) {
            if (k & (1LL << i))
                res ^= b[i];
        }
        return res;
    }

    // O(BITS^2)
    bool merge_basis(XorBasis& other) {
        bool added = false;
        for (ll i = 0; i < BITS; i++)
            if (other.basis[i]) added |= insert(other.basis[i]);
        return added;
    }

    ll count_subsets(ll x) const {
        if (!can(x)) return 0;
        return 1LL << (total - rank());
    }
};

