#pragma once

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


#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;

inline gp_hash_table<int, int, custom_hash> hm;
