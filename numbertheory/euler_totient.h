#pragma once
#include "core.h"

namespace nt {

// O(sqrt(n))
inline ll phi_ll(ll n) {
    if (n == 0) return 0;
    ll res = n;
    for (ll p = 2; p * p <= n; p += (p == 2 ? 1 : 2)) {
        if (n % p == 0) {
            while (n % p == 0) n /= p;
            res -= res / p;
        }
    }
    if (n > 1) res -= res / n;
    return res;
}

// O(n log log n)
inline vi phi_1_to_n(int n) {
    vi phi(n+1);
    iota(all(phi), 0);
    for (int i = 2; i <= n; i++) if (phi[i] == i) {
        for (int j = i; j <= n; j += i) phi[j] -= phi[j] / i;
    }
    return phi;
}

} // namespace nt
