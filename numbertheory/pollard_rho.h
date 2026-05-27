#pragma once
#include "core.h"
#include "mod_arith.h"
#include "gcd.h"
#include "primality.h"

namespace nt {

/** Polynomial step function used by Pollard Rho. Time: O(1). Space: O(1). */
inline u64 rho_f(u64 x, u64 c, u64 mod) {
    return (mul_mod(x, x, mod) + c) % mod;
}

/**
 * Returns a non-trivial factor of composite n using Pollard Rho.
 * Time: expected sublinear, heuristic. Space: O(1).
 */
inline u64 pollard_rho(u64 n) {
    if ((n & 1ULL) == 0) return 2;
    if (n % 3ULL == 0) return 3;

    static std::mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<u64> dist(2, n-2);

    while (true) {
        u64 x = dist(rng);
        u64 y = x;
        u64 c = dist(rng);
        if (c >= n) c %= n;
        u64 d = 1;

        while (d == 1) {
            x = rho_f(x, c, n);
            y = rho_f(rho_f(y, c, n), c, n);
            u64 diff = (x > y ? x - y : y - x);
            d = (u64)gcd_ll((i64)diff, (i64)n);
        }

        if (d != n) return d;
        // else retry
    }
}

/** Recursively appends prime factors of n to fac. Time: heuristic. Space: O(log n) recursion. */
inline void factor_rec(u64 n, vector<u64>& fac) {
    if (n == 1) return;
    if (is_prime_u64(n)) { fac.pb(n); return; }
    u64 d = pollard_rho(n);
    factor_rec(d, fac);
    factor_rec(n / d, fac);
}

/** Returns all prime factors of n with multiplicity. Time: heuristic. Space: O(number of factors). */
inline vector<u64> factorize_u64(u64 n) {
    vector<u64> fac;
    if (n == 0) return fac;
    factor_rec(n, fac);
    sort(all(fac));
    return fac;
}

/** Returns factorization of n as {prime, exponent}. Time: heuristic. Space: O(number of distinct factors). */
inline vector<pair<u64, int>> factorize_u64_powers(u64 n) {
    auto fac = factorize_u64(n);
    vector<pair<u64, int>> res;
    for (auto p : fac) {
        if (res.empty() || res.back().first != p) res.pb({p, 1});
        else res.back().second++;
    }
    return res;
}

} // namespace nt
