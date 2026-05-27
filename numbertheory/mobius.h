#pragma once
#include "core.h"

namespace nt {

/** Linear sieve for Mobius values mu[1..n]. Space: O(n). */
struct MobiusSieve {
    int n = 0;
    vector<int> mu;
    vector<int> primes;
    vector<char> comp;

    MobiusSieve() {}
    MobiusSieve(int _n) { init(_n); }

    /** Computes Mobius values up to _n. Time: O(n). */
    void init(int _n) {
        n = _n;
        mu.assign(n+1, 0);
        comp.assign(n+1, false);
        primes.clear();
        mu[1] = 1;

        for (int i = 2; i <= n; i++) {
            if (!comp[i]) {
                primes.push_back(i);
                mu[i] = -1;
            }
            for (auto p : primes) {
                ll x = 1LL * p * i;
                if (x > n) break;
                comp[(int)x] = true;
                if (i % p == 0) { mu[(int)x] = 0; break; }
                else mu[(int)x] = -mu[i];
            }
        }
    }
};

/**
 * Returns sum over divisors d of n of mu[d] * g[n / d].
 * @param g Array indexed at least up to n.
 * @param mu Mobius values indexed at least up to n.
 * Time: O(sqrt n). Space: O(1).
 */
template<class T>
inline ll mobius_convolution_divisors(ll n, const vector<T>& g, const vector<int>& mu) {
    ll res = 0;
    for (ll d = 1; d * d <= n; d++) if (n % d == 0) {
        res += 1LL * mu[(int)d] * (ll)g[(size_t)(n / d)];
        if (d * d != n) res += 1LL * mu[(int)(n / d)] * (ll)g[(size_t)d];
    }
    return res;
}

} // namespace nt
