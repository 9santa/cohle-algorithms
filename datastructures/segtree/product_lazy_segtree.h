#include "../../header.h"

constexpr int MOD = 1e9 + 7;

// Multiplicative (product) lazy segment tree
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
    ProductLazySeg(const vector<ll>& arr) {
        int _n = sz(arr);
        n = 1;
        while (n < _n) n <<= 1;
        data.resize(2*n, 1);
        lazy.resize(n, 1); // only internal nodes need lazy
        len.resize(2*n, 0);
        build(arr);
    }

    // Range multiplication update [l, r]
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

    // Range product query [l, r]
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

    // Point mul update
    void point_mul(int idx, ll val) {
        idx += n;
        push_path(idx);
        data[idx] = data[idx] * val % MOD;
        for (idx >>= 1; idx > 0; idx >>= 1) {
            data[idx] = (data[idx<<1] * data[idx<<1|1]) % MOD;
        }
    }

    // Point query
    ll point_query(int idx) {
        idx += n;
        push_path(idx);
        return data[idx];
    }
};



// Recursive
struct mulsegtree {
private:
    int n;
    vector<ll> data;
    vector<ll> lazy;

    ll cmb(ll a, ll b) {
        return (a * b) % MOD;
    }

    void build(const V<ll>& a, int v, int tl, int tr) {
        lazy[v] = 1;
        if (tl == tr) data[v] = a[tl];
        else {
            int tm = (tl+tr)>>1;
            build(a, v*2, tl, tm);
            build(a, v*2+1, tm+1, tr);
            data[v] = cmb(data[v*2], data[v*2+1]);
        }
    }

    void apply(int v, int l, int r, ll val) {
        data[v] = (data[v] * binpow(data[v], r-l+1)) % MOD;
        lazy[v] = mulMod<MOD>((u32)lazy[v], val);
    }

    void push(int v, int tl, int tr) {
        if (lazy[v] != 1) {
            auto mid = (tl+tr)>>1;
            apply(v*2, tl, mid, lazy[v]);
            apply(v*2+1, mid+1, tr, lazy[v]);
            lazy[v] = 1;
        }
    }

    void range_mul(int v, int tl, int tr, int l, int r, ll val) {
        assert(0 <= l && l <= r && r <= n);
        if (r < tl || l > tr) return;
        if (tl <= l && r <= tr) {
            apply(v, l, r, val);
            return;
        }
        push(v, l, r);
        auto mid = (l+r)>>1;
        range_mul(v*2, tl, mid, l, min(r, mid), val);
        range_mul(v*2+1, mid+1, tr, max(l, mid+1), r, val);
        data[v] = cmb(data[v*2], data[v*2+1]);
    }

    ll range_query(int v, int tl, int tr, int l, int r) {
        if (r < tl || l > tr) return 1;
        if (tl <= l && r <= tr) return data[v];
        push(v, l, r);
        auto tm = (l+r)>>1;
        ll left = range_query(v*2, tl, tm, l, min(l, tm));
        ll right = range_query(v*2+1, tm+1, tr, max(l, tm+1), r);
        return cmb(left, right);
    }

public:
    mulsegtree(const V<ll>& a) {
        n = sz(a);
        data.resize(4*n);
        lazy.resize(4*n,1);
        if (n > 0) build(a, 1, 0, n-1);
    }

    void range_mul(int l, int r, ll val) {
        range_mul(1, 0, n-1, l, r, val);
    }

    ll range_query(int l, int r) {
        return range_query(1, 0, n-1, l, r);
    }
};
