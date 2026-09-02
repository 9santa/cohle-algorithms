#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using u32 = unsigned int;
template<class T> using V = vector<T>;
#define all(x) (x).begin(), (x).end()
#define sz(x) int((x).size())

constexpr int MOD = 1e9 + 7;

/** Lazy segment tree for range multiplication and range product modulo MOD. Space: O(n). */
struct ProductLazySeg {
private:
    int n;
    vector<ll> data;
    vector<ll> lazy;
    vector<int> len; // len of each node's segment

    ll mod_pow(ll a, ll b) {
        ll res = 1;
        for (a %= MOD; b; b>>=1) {
            if (b&1) res = (res * a) % MOD;
            a = (a * a) % MOD;
        }
        return res;
    }

    // Build from array
    void build(const vector<ll>& arr) {
        fill(all(lazy), 1); // reset lazy just in-case
        // Leaves
        for (int i = 0; i < sz(arr); i++) {
            data[n+i] = arr[i] % MOD;
            len[n+i] = 1;
        }
        // Internal nodes bottom-up
        for (int i = n-1; i > 0; i--) {
            data[i] = (data[i<<1] * data[i<<1|1]) % MOD;
            len[i] = len[i<<1] + len[i<<1|1];
        }
    }

    // Apply multiplication to a node
    void apply(int idx, ll val) {
        data[idx] = data[idx] * mod_pow(val, len[idx]) % MOD;
        if (idx < n) { // if not a leaf
            lazy[idx] = lazy[idx] * val % MOD;
        }
    }

    // Push lazy down one level
    void push(int idx) {
        if (lazy[idx] != 1) {
            apply(idx<<1, lazy[idx]);
            apply(idx<<1|1, lazy[idx]);
            lazy[idx] = 1;
        }
    }

    // Push all lazy values on path from root to leaf, idx should be a leaf
    void push_path(int idx) {
        for (int h = log2(n); h > 0; h--) {
            int i = idx >> h; // ancestor at height h
            if (lazy[i] != 1) push(i);
        }
    }

public:
    /** Builds from a 0-indexed array. Time: O(n). */
    ProductLazySeg(const vector<ll>& arr) {
        int _n = sz(arr);
        n = 1;
        while (n < _n) n <<= 1;
        data.resize(2*n, 1);
        lazy.resize(n, 1); // only internal nodes need lazy
        len.resize(2*n, 0);
        build(arr);
    }

    /** Multiplies every value in [l, r] by val. Time: O(log n log MOD). */
    void range_mul(int l, int r, ll val) {
        val %= MOD;
        if (val == 1) return; // no change
        l += n, r += n;
        int l0 = l, r0 = r;
        push_path(l0), push_path(r0);

        // Apply updates
        for (; l <= r; l >>= 1, r >>= 1) {
            if (l&1) apply(l++, val);
            if (!(r&1)) apply(r--, val);
        }

        // Update ancestors
        for (l = l0; l > 0; l >>= 1) {
            if (l < n && lazy[l] == 1) {
                data[l] = (data[l<<1] * data[l<<1|1]) % MOD;
            }
        }
        for (r = r0; r > 0; r >>= 1) {
            if (r < n && lazy[r] == 1) {
                data[r] = (data[r<<1] * data[r<<1|1]) % MOD;
            }
        }
    }

    /** Returns the product over [l, r]. Time: O(log n). */
    ll range_query(int l, int r) {
        l += n, r += n;
        push_path(l), push_path(r);
        ll res = 1;
        for (; l <= r; l >>= 1, r >>= 1) {
            if (l&1) res = (res * data[l++]) % MOD;
            if (!(r&1)) res = (res * data[r--]) % MOD;
        }
        return res;
    }

    /** Multiplies a[idx] by val. Time: O(log n). */
    void point_mul(int idx, ll val) {
        idx += n;
        push_path(idx);
        data[idx] = data[idx] * val % MOD;
        for (idx >>= 1; idx > 0; idx >>= 1) {
            data[idx] = (data[idx<<1] * data[idx<<1|1]) % MOD;
        }
    }

    /** Returns a[idx]. Time: O(log n). */
    ll point_query(int idx) {
        idx += n;
        push_path(idx);
        return data[idx];
    }
};
