#pragma once
#include <bits/stdc++.h>
using namespace std;

using i128 = __int128;
using u64 = unsigned long long;
using i64 = long long;
using ll = long long;
#define sz(x) int((x).size())

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

/**
 * Solves a^x = b (mod m) using BSGS when gcd(a, m) = 1.
 * @return Smallest x >= 0, or -1 if no solution is found.
 * Time: O(sqrt m) expected. Space: O(sqrt m).
 */
inline ll bsgs_coprime(u64 a, u64 b, u64 m) {
    a %= m, b %= m;
    if (m == 1) return 0;
    if (b == 1 % m) return 0;

    ll n = (ll)std::sqrt((long double)m) + 1;

    // an = a^n mod m
    u64 an = 1 % m;
    for (ll i = 0; i < n; i++) an = mul_mod(an, a, m);

    // baby steps: store b * a^q (q=0..n)
    // want: (a^n)^p = b * a^q => a^(pn-q) = b
    unordered_map<u64, ll> mp;
    mp.reserve(n+5);
    mp.max_load_factor(0.7f);

    u64 cur = b;
    for (ll q = 0; q <= n; q++) {
        if (!mp.count(cur)) mp[cur] = q; // keep smallest q
        cur = mul_mod(cur, a, m);
    }

    // giant steps: cur = (a^n)^p
    cur = 1 % m;
    for (ll p = 1; p <= n+1; p++) {
        cur = mul_mod(cur, an, m);
        auto it = mp.find(cur);
        if (it != mp.end()) {
            ll q = it->second;
            ll x = p * n - q;
            if (x >= 0) return x;
        }
    }
    return -1;
}


/**
 * Solves a^x = b (mod m) using extended BSGS for non-coprime cases.
 * @return Smallest x >= 0, or -1 if no solution exists.
 * Time: O(sqrt m) expected. Space: O(sqrt m).
 */
inline ll exbsgs(u64 a, u64 b, u64 m) {
    assert(gcd(a, m) != 1);
    a = safe_mod(a, m);
    b = safe_mod(b, m);
    if (m == 1) return 0;

    ll cnt = 0;
    ll k = 1;

    while (true) {
        ll g = gcd_ll(a, m);
        if (g == 1) break;
        if (b % g != 0) return -1; // need g | b

        b /= g;
        m /= g;
        k = (i128)((i128)k * (a / g) % m);
        cnt++;

        if (k == b) return cnt; // can return early: k*a^0 = k = b' (mod m')
    }

    ll inv_k = mod_inv(k, m);
    if (inv_k == -1) return -1;

    ll target = (i64)((i128)b * inv_k % m);
    ll y = bsgs_coprime(a, target, m);
    if (y == -1) return -1;
    return y + cnt;
}

} // namespace nt
