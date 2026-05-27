#pragma once
#include "core.h"
#include "mod_arith.h"

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
