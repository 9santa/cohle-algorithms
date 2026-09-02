#pragma once
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
using namespace std;

using i128 = __int128;
using u64 = unsigned long long;
using i64 = long long;
using ll = long long;
using u32 = unsigned int;
using vl = vector<long long>;

namespace nt {

/** Returns prime factors of n with multiplicity. Time: O(sqrt n). Space: O(number of factors). */
inline vl prime_factors_trial(ll n) {
    vl f;
    if (n < 0) n = -n;
    for (ll p = 2; p * p <= n; p += (p == 2 ? 1 : 2)) {
        while (n % p == 0) {
            f.push_back(p);
            n /= p;
        }
    }
    if (n > 1) f.push_back(n);
    return f;
}

/** Returns factorization of n as {prime, exponent}. Time: O(sqrt n). Space: O(number of distinct factors). */
inline vector<pair<ll, int>> factorize_trial(ll n) {
    vector<pair<ll, int>> res;
    if (n < 0) n = -n;
    for (ll p = 2; p * p <= n; p += (p == 2 ? 1 : 2)) {
        if (n % p != 0) continue;
        int e = 0;
        while (n % p == 0) n /= p, e++;
        res.push_back({p, e});
    }
    if (n > 1) res.push_back({n, 1});
    return res;
}

} // namespace nt


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


/** Returns a pseudo-random 64-bit value. Time: O(1). */
u64 RNG_64() {
  static u64 x_ = u64(chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count()) * 10150724397891781847ULL;
  x_ ^= x_ << 7;
  return x_ ^= x_ >> 9;
}

/** Returns a pseudo-random value in [0, lim). Time: O(1). */
u64 RNG(u64 lim) { return RNG_64() % lim; }

/** Returns a pseudo-random value in [l, r). Time: O(1). */
ll RNG(ll l, ll r) { return l + RNG_64() % (r - l); }

// pair hashing
/** Simple pair hash for integer pairs. Space: O(1). */
struct hash_pair {
    size_t operator()(const pair<int, int> &p) const { return p.first ^ p.second; }
};

// std::hash specialization for pii
namespace std {
template<> struct hash<pair<int, int>> {
        size_t operator()(const pair<int, int> &p) const { return p.first ^ p.second; }
    };
};

// --- General custom hash functions ---
// splitmix64 custom hash function
// multiple xor-shifts + multiplications
/** SplitMix64-based hash for unordered containers. Space: O(1). */
struct custom_hash {
    /** Mixes a 64-bit value. Time: O(1). */
    static u64 splitmix64(u64 x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }

    /** Hashes x with a process-local random offset. Time: O(1). */
    size_t operator()(u64 x) const {
        static const u64 STATIC_RANDOM = chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + STATIC_RANDOM);
    }
};

// usaco/benq hash function
// multiply by odd constant + byte swap
/** Byte-swap hash using a random multiplier. Space: O(1). */
struct benq_hash {
    const double PI = acos(-1.0);
    // any random-ish large odd number will do
    const u64 C = u64(2e18 + PI) + 71;
    // random 32-bit number
    const u32 RANDOM = chrono::steady_clock::now().time_since_epoch().count();
    /** Hashes x. Time: O(1). */
    size_t operator()(u64 x) const {
        return __builtin_bswap64((x ^ RANDOM) * C);
    }
};


using namespace __gnu_pbds;

inline gp_hash_table<int, int, custom_hash> hm;


namespace nt {

/**
 * Returns a primitive root modulo prime p.
 * @param p Prime modulus.
 * Time: expected O(number of factors of p-1 * log p). Space: O(number of factors).
 */
inline int primitive_root(int p) {
    auto pf = factorize_trial(p-1);
    auto is_root = [&](int g) -> bool {
        for (auto& [q, e] : pf) {
            if (pow_mod(g, (p-1) / q, p) == 1) return false;
        }
        return true;
    };
    while (1) {
        int g = RNG(1, p);
        if (is_root(g)) return g;
    }
    return -1;
}

/**
 * Returns a primitive root modulo prime p.
 * @param p Prime modulus.
 * Time: expected O(number of factors of p-1 * log p). Space: O(number of factors).
 */
inline ll primitive_root_ll(ll p) {
    auto pf = factorize_trial(p-1);
    auto is_root = [&](ll g) -> bool {
        for (auto& [q, e] : pf) {
            if (pow_mod(g, (p-1) / q, p) == 1) return false;
        }
        return true;
    };
    while (1) {
        ll g = RNG(1, p);
        if (is_root(g)) return g;
    }
    return -1;
}

} // namespace nt
