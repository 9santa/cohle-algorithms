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
 * Returns the Legendre symbol (a | p).
 * @param p Odd prime modulus.
 * Time: O(log p). Space: O(1).
 */
inline int legendre_symbol(ll a, ll p) {
    a %= p;
    if (a < 0) a += p;
    if (a == 0) return 0;
    ll t = pow_mod(a, (p-1)/2, p);
    if (t == 1) return 1;
    if (t == p - 1) return -1;
    return 0; // should not happen for prime p
}

/**
 * Returns the Jacobi symbol (a | n).
 * @param n Positive odd integer.
 * Time: O(log n). Space: O(1).
 */
inline int jacobi_symbol(ll a, ll n) {
    if (n <= 0 || (n % 2 == 0)) return 0;
    a %= n;
    if (a < 0) a += n;

    int ans = 1;
    while (a != 0) {
        while ((a & 1) == 0) {
            a >>= 1;
            int r = n % 8;
            if (r == 3 || r == 5) ans = -ans;
        }
        swap(a, n);
        if ((a % 4 == 3) && (n % 4 == 3)) ans = -ans;
        a %= n;
    }
    return (n == 1 ? ans : 0);
}

} // namespace nt
