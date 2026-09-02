#pragma once
#include <bits/stdc++.h>
using namespace std;

using i128 = __int128;
using u64 = unsigned long long;
using i64 = long long;

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

/** Returns whether n is prime using deterministic Miller-Rabin for u64. Time: O(log n). Space: O(1). */
inline bool is_prime_u64(u64 n) {
    if (n < 2) return false;
    // quick trial division by some small primes (fast)
    static const u64 A_small[] = {2ULL,3ULL,5ULL,7ULL,11ULL,13ULL,17ULL,19ULL,23ULL,29ULL,31ULL,37ULL};
    for (auto p : A_small) {
        if (n == p) return true;
        if (n % p == 0) return false;
    }

    u64 d = n-1;
    int s = __builtin_ctzll(d);
    d >>= s;

    auto witness = [&](u64 a) -> bool {
        if (a % n == 0) return false; // if a is multiple of n, it gives no info
        u64 x = pow_mod(a, d, n);
        if (x == 1 || x == n-1) return false; // the criteria for a base to pass
        for (int r = 1; r < s; r++) {
            x = mul_mod(x, x, n);
            if (x == n-1) return false; // passes for this base
        }
        return true; // never hit -1 => def composite
    };

    // deterministic for 64-bit
    static const u64 A[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    for (auto a : A) {
        if (witness(a)) return false;
    }

    return true;
}

} // namespace nt
