#pragma once
#include "core.h"

namespace nt {

// SPF sieve. fast factorization for many queries
struct SPF {
    int n = 0;
    vi spf; // spf[x] = smallest prime factor of x

    SPF() {}
    SPF(int _n) { init(_n); }

    void init(int _n) {
        n = _n;
        spf.resize(n+1);
        iota(all(spf), 0);
        if (n >= 0) spf[0] = 0;
        if (n >= 1) spf[1] = 1;

        for (int i = 2; 1LL * i * i <= n; i++) {
            if (spf[i] == i) { // i is prime
                for (int j = i * i; j <= n; j += i) {
                    if (spf[j] == j) spf[j] = i;
                }
            }
        }
    }

    // factorize x (x must be <= n)
    vector<pair<int, int>> factorize(int x) const {
        vector<pair<int, int>> res;
        while (x > 1) {
            int p = spf[x];
            int e = 0;
            while (x % p == 0) x /= p, e++;
            res.pb({p, e});
        }
        return res;
    }

    // distinct primes only
    vi distinct_primes(int x) const {
        vi res;
        while (x > 1) {
            int p = spf[x];
            res.pb(p);
            while (x % p == 0) x /= p;
        }
        UNIQUE(res);
        return res;
    }
};

}
