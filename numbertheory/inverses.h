#pragma once
#include "core.h"
#include "mod_arith.h"

namespace nt {

/**
 * Returns modular inverses for every i in [1, n] modulo prime p.
 * @param p Prime modulus with n < p.
 * Time: O(n). Space: O(n).
 */
inline vector<ll> alL_inverses(int n, ll p) {
    assert(n < p);
    vector<ll> inv(n+1);
    if (n >= 1) inv[1] = 1;
    for (int i = 2; i <= n; i++) {
        inv[i] = (p - (p / i) * inv[p % i] % p) % p;
    }
    return inv;
}


/**
 * Returns modular inverses for every i in [1, n] using prefix products.
 * @param p Prime modulus with n < p.
 * Time: O(n + log p). Space: O(n).
 */
inline vector<ll> all_inverses(int n, ll p) {
    assert(n < p);
    vector<ll> pref(n+1), inv(n+1, 0);
    for (int i = 1; i <= n; i++) pref[i] = (i128)pref[i-1] * i % p;

    ll x = pow_mod(pref[n], p-2, p); // (n!)^{-1}

    for (int i = n; i >= 1; i--) {
        inv[i] = (i128)x * pref[i-1] % p; // i^{-1}
        x = (i128)x * i % p;              // ((i-1)!)^{-1}
    }
    return inv; // inv[1..n]
}

} // namespace nt
