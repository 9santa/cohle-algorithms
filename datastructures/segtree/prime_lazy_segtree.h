#include "../../header.h"

ll binpow(ll a, ll b, ll MOD) {
    ll res = 1;
    for (a %= MOD; b; b>>=1) {
        if (b&1) res = (res * a) % MOD;
        a = (a * a) % MOD;
    }
    return res;
}

bool isPrime[MAXN+1];
vector<int> primes;

// O(N log log N)
void sieve(void) {
    memset(isPrime, true, sizeof(isPrime));
    isPrime[0] = isPrime[1] = 0;

    for (int i = 2; i <= MAXN; i++) {
        if (isPrime[i]) {
            primes.push_back(i);
            for (int j = i*i; j <= MAXN; j += i) {
                isPrime[j] = 0;
            }
        }
    }
}

vector<u64> compute_masks() {
    vector<u64> masks(MAXN+1, 0);
    for (int num = 1; num <= MAXN; num++) {
        u64 mask = 0;
        for (int i = 0; i < sz(primes); i++) {
            if (num % primes[i] == 0) mask |= (1ull << i);
        }
        masks[num] = mask;
    }
    return masks;
}

vector<u64> masks;

struct PrimeSeg {
private:
    int n;
    vector<u64> data;
    vector<u64> lazy;

    u64 get_mask(int x) {
        return (x <= MAXN) ? masks[x] : 0;
    }

    void apply(int idx, u64 mask) {
        data[idx] |= mask;
        if (idx < n) lazy[idx] |= mask;
    }

    // Push lazy down one level
    void push(int idx) {
        if (lazy[idx] != 0) {
            apply(idx<<1, lazy[idx]);
            apply(idx<<1|1, lazy[idx]);
            lazy[idx] = 0;
        }
    }

    // Push all lazy values on path from root to leaf, idx should be a leaf
    void push_path(int idx) {
        for (int h = 31 - __builtin_clz(n); h > 0; h--) {
            int i = idx >> h; // ancestor at height h
            if (lazy[i] != 0) push(i);
        }
    }

    // Build from array
    void build(const vector<ll>& arr) {
        int _n = sz(arr);
        for (int i = 0; i < _n; i++) {
            data[i+n] = get_mask(arr[i]);
        }
        for (int i = n-1; i > 0; i--) {
            data[i] = data[i<<1] | data[i<<1|1];
        }
    }


public:
    PrimeSeg(const vector<ll>& arr) : n(arr.size()) {
        int size = 1; while (size < n) size <<= 1;
        n = size;
        data.resize(2*n, 0);
        lazy.resize(n, 0);
        build(arr);
    }

    // Range OR update: add prime divisors of x to range [l, r]
    void range_or(int l, int r, ll x) {
        u64 xmask = get_mask(x);
        if (xmask == 0) return; // no primes
        l += n, r += n;
        int l0 = l, r0 = r;
        push_path(l0), push_path(r0);

        // Apply OR updates
        for (; l <= r; l >>= 1, r >>= 1) {
            if (l&1) apply(l++, xmask);
            if (!(r&1)) apply(r--, xmask);
        }

        // Update parents
        for (l = l0; l >= 1; l >>= 1) {
            if (l < n && lazy[l] == 0) {
                data[l] = data[l<<1] | data[l<<1|1];
            }
        }
        for (r = r0; r >= 1; r >>= 1) {
            if (r < n && lazy[r] == 0) {
                data[r] = data[r<<1] | data[r<<1|1];
            }
        }
    }

    // Range query: get union of prime divisors in range [l, r]
    u64 range_query(int l, int r) {
        l += n, r += n;
        push_path(l), push_path(r);

        u64 res = 0;
        for (; l <= r; l >>= 1, r >>= 1) {
            if (l&1) res |= data[l++];
            if (!(r&1)) res |= data[r--];
        }
        return res;
    }

    // Point query: get prime mask at index idx
    u64 point_query(int idx) {
        idx += n;
        push_path(idx);
        return data[idx];
    }

    // Check if range [l, r] has all primes in mask
    bool has_all(int l, int r, u64 mask) {
        u64 range_mask = range_query(l, r);
        return (range_mask & mask) == mask;
    }

    // Check if range [l, r] has any prime in mask
    bool has_any(int l, int r, u64 mask) {
        u64 range_mask = range_query(l, r);
        return (range_mask & mask) != 0 ;
    }

    // Count distinct primes in range [l, r]
    int count_primes(int l, int r) {
        return __builtin_popcountll(range_query(l, r));
    }

    // Get list of primes from mask
    vector<int> mask_to_primes(u64 mask) {
        vector<int> res;
        for (int i = 0; i < sz(primes); i++) {
            if (mask & (1ull << i)) {
                res.push_back(primes[i]);
            }
        }
        return res;
    }

    // Get primes in range [l, r]
    vector<int> get_primes_in_range(int l, int r) {
        return mask_to_primes(range_query(l, r));
    }

};

