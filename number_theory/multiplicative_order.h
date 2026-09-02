#pragma once
#include <bits/stdc++.h>
using namespace std;

using i128 = __int128;
using u64 = unsigned long long;
using i64 = long long;
#define all(x) (x).begin(), (x).end()
#define sz(x) int((x).size())
#define pb push_back

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

/** Returns gcd(a, b). Time: O(log min(|a|, |b|)). Space: O(1). */
inline i64 gcd_ll(i64 a, i64 b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) {
        i64 t = a % b;
        a = b;
        b = t;
    }
    return a;
}

/** Returns gcd(a, b). Time: O(log min(a, b)). Space: O(1). */
inline u64 gcd_u64(u64 a, u64 b) {
    while (b) {
        u64 t = a % b;
        a = b;
        b = t;
    }
    return a;
}

/** Returns lcm(a, b), or 0 if either value is 0. Time: O(log min(|a|, |b|)). Space: O(1). */
inline i64 lcm_ll(i64 a, i64 b) {
    if (a == 0 || b == 0) return 0;
    return a / gcd_ll(a, b) * b; // no overflow
}

/** Returns lcm(a, b). Time: O(log min(a, b)). Space: O(1). */
inline u64 lcm_u64(u64 a, u64 b) {
    return a / gcd_u64(a, b) * b;
}

/** Returns gcd(a, b) using Stein's binary gcd algorithm. Time: O(log max(|a|, |b|)). Space: O(1). */
inline i64 binary_gcd(i64 a, i64 b) {
    if (a == 0 || b == 0) return a | b;
    if (a < 0) a = -a;
    if (b < 0) b = -a;
    unsigned shift = __builtin_ctzll((u64)(a | b));
    a >>= __builtin_ctzll((u64)a);
    do {
        b >>= __builtin_ctzll((u64)b);
        if (a > b) swap(a, b);
        b -= a;
    } while (b);
    return a << shift;
}

/** Returns {g, x, y} such that ax + by = g = gcd(a, b). Time: O(log min(|a|, |b|)). */
inline tuple<i64, i64, i64> ext_gcd(i64 a, i64 b) {
    i64 x = 1, y = 0, x1 = 0, y1 = 0;
    while (b != 0) {
        i64 q = a / b;
        tie(a, b) = make_pair(b, a - q * b);
        tie(x, x1) = make_pair(x1, x - q * x1);
        tie(y, y1) = make_pair(y1, y - q * y1);
    }
    // a is gcd, (x,y) are coefficients
    return {a, x, y};
}

/**
 * Returns g = gcd(a, b) and writes coefficients satisfying ax + by = g.
 * Time: O(log min(|a|, |b|)). Space: O(log min(|a|, |b|)).
 */
inline i64 ext_gcd_rec(i64 a, i64 b, i64 &x, i64 &y) {
    if (b == 0) { x = 1; y = 0; return a; }
    i64 x1, y1;
    i64 g = ext_gcd_rec(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

/** Finds any integer solution (x0, y0) to ax + by = c. Time: O(log min(|a|, |b|)). */
inline pair<bool, pair<i64, i64>> solve_diophantine(i64 a, i64 b, i64 c) {
    auto [g, x, y] = ext_gcd(a, b);
    if (c % g != 0) return {false, {0, 0}}; // no solution
    i64 mul = c / g;
    i64 x0 = x * mul;
    i64 y0 = y * mul;
    return {true, {x0, y0}};
}

/**
 * Combines two congruences with the generalized CRT.
 * @param r1 Remainder of the first congruence.
 * @param m1 Modulus of the first congruence.
 * @param r2 Remainder of the second congruence.
 * @param m2 Modulus of the second congruence.
 * @return {r, m} meaning x = r (mod m), or {0, 0} if inconsistent.
 * Time: O(log min(m1, m2)). Space: O(1).
 */
inline pair<i64, i64> crt_pair(i64 r1, i64 m1, i64 r2, i64 m2) {
    if (m1 < 0) m1 = -m1;
    if (m2 < 0) m2 = -m2;
    if (m1 == 0 || m2 == 0) return {0, 0};
    r1 = safe_mod(r1, m1);
    r2 = safe_mod(r2, m2);

    auto [g, x, y] = ext_gcd(m1, m2); // x*m1 + y*m2 = g
    i64 diff = r2 - r1;
    if (diff % g != 0) return {0, 0}; // no solution

    // reduce
    i64 m1p = m1 / g;
    i64 m2p = m2 / g;
    // need t = (diff/g) * inv(m1p) (mod m2p)
    i64 inv_m1p = safe_mod(x, m2p);
    i64 dg = diff / g;
    i64 t = (i64)((i128)safe_mod(dg, m2p) * inv_m1p % m2p);

    // combine
    i128 r = (i128)r1 + (i128)m1 * t;
    i64 lcm = (i64)((i128)m1p * m2); // = m1/g * m2 = lcm(m1, m2)

    i64 res = (i64)(r % lcm);
    if (res < 0) res += lcm;
    return {res, lcm};
}

/**
 * Combines many congruences x = r[i] (mod m[i]).
 * @return {r, m} meaning x = r (mod m), or {0, 0} if inconsistent.
 * Time: O(k log M), where k is the number of congruences. Space: O(1).
 */
inline pair<i64, i64> crt_many(const vector<i64>& r, const vector<i64>& m) {
    pair<i64, i64> cur = {0, 1}; // x = 0 (mod 1)
    for (int i = 0; i < sz(r); i++) {
        cur = crt_pair(cur.first, cur.second, r[i], m[i]);
        if (cur.second == 0) return {0, 0};
    }
    return cur;
}

/**
 * Returns the modular inverse of a modulo mod, or -1 if it does not exist.
 * @param mod Positive modulus, not necessarily prime.
 * Time: O(log mod). Space: O(1).
 */
inline i64 mod_inv(i64 a, i64 mod) {
    auto [g, x, y] = ext_gcd(a, mod);
    if (g != 1 && g != -1) return -1;
    return safe_mod(x, mod);
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


namespace nt {


/**
 * Returns Carmichael's lambda for p^e.
 * @param p Prime base.
 * @param e Positive exponent.
 * Time: O(e). Space: O(1).
 */
inline u64 carmichael_prime_power(u64 p, int e) {
    if (p == 2) {
        if (e == 1) return 1;   // lambda(2)=1
        if (e == 2) return 2;   // lambda(4)=2
        return 1ULL << (e - 2); // lambda(2^e)=2^(e-2), e>=3
    }

    // lambda(p^e) = phi(p^e) = p^(e-1) * (p-1) for odd prime p
    u64 pe_1 = 1;
    for (int i = 0; i < e-1; i++) pe_1 *= p;
    return pe_1 * (p - 1);
}

/**
 * Returns Carmichael's lambda function lambda(n).
 * Time: dominated by factorization. Space: O(number of prime factors).
 */
inline u64 carmichael_lambda(u64 n) {
    if (n == 0) return 0; // technically undefined
    if (n == 1) return 1;

    auto fac = factorize_u64_powers(n);
    u64 ans = 1;
    for (auto& [p, e] : fac) {
        ans = lcm_u64(ans, carmichael_prime_power(p, e));
    }
    return ans;
}

} // namespace nt


namespace nt {

/**
 * Returns the multiplicative order ord_n(a).
 * @return 0 if the order does not exist.
 * Time: dominated by factorization of lambda(n). Space: O(number of prime factors).
 */
inline u64 multiplicative_order(u64 a, u64 n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    a %= n;
    if (gcd_u64(a, n) != 1) return 0; // requirement

    // since lambda(n) <= phi(n), this is actually better than via Euler's totient
    u64 ord = carmichael_lambda(n); // ord | lambda(n) | phi(n)
    auto fac = factorize_u64_powers(ord);

    for (auto& [p, e] : fac) {
        while (ord % p == 0 && pow_mod(a, ord / p, n) == 1) {
            ord /= p;
        }
    }
    return ord;
}

} // namespace nt
