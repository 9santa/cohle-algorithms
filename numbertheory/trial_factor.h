#pragma once
#include "core.h"

namespace nt {

// returns prime factors with multiplicities
inline vl prime_factors_trial(ll n) {
    vl f;
    if (n < 0) n = -n;
    for (ll p = 2; p * p <= n; p += (p == 2 ? 1 : 2)) {
        while (n % p == 0) {
            f.pb(p);
            n /= p;
        }
    }
    if (n > 1) f.pb(n);
    return f;
}

// returns {prime, exponent}
inline vector<pair<ll, int>> factorize_trial(ll n) {
    vector<pair<ll, int>> res;
    if (n < 0) n = -n;
    for (ll p = 2; p * p <= n; p += (p == 2 ? 1 : 2)) {
        if (n % p != 0) continue;
        int e = 0;
        while (n % p == 0) n /= p, e++;
        res.pb({p, e});
    }
    if (n > 1) res.pb({n, 1});
    return res;
}

} // namespace nt
