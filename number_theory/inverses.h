#pragma once
#include <bits/stdc++.h>
using namespace std;

using i128 = __int128;
using u64 = unsigned long long;
using i64 = long long;
using ll = long long;

namespace nt {

/**
 * Returns x modulo mod in [0, mod).
 * @param mod Positive modulus.
 * Time: O(1). Space: O(1).
 */
inline i64 safe_mod(i64 x, i64 mod) {
    x %= mod;
    if (x < 0) x += mod;
    return x;
}

/**
 * Returns a * b modulo mod without 64-bit overflow.
 * @param mod Positive modulus.
 * Time: O(1). Space: O(1).
 */
inline u64 mul_mod(u64 a, u64 b, u64 mod) {
    return (i128)a * b % mod;
}

/**
 * Returns a^e modulo mod.
 * @param mod Positive modulus.
 * Time: O(log e). Space: O(1).
 */
inline u64 pow_mod(u64 a, u64 e, u64 mod) {
    u64 res = 1 % mod;
    a %= mod;
    while (e) {
        if (e & 1) res = mul_mod(res, a, mod);
        a = mul_mod(a, a, mod);
        e >>= 1;
    }
    return res;
}

inline u64 inv_mod(u64 a, u64 MOD) {
    return pow_mod(a, MOD - 2, MOD);
}

} // namespace nt


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
