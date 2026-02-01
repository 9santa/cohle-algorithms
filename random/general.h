#include "../header.h"

// pair hashing
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
struct custom_hash {
    static u64 splitmix64(u64 x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }

    size_t operator()(u64 x) const {
        static const u64 STATIC_RANDOM = chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + STATIC_RANDOM);
    }
};

// usaco/benq hash function
// multiply by odd constant + byte swap
struct benq_hash {
    const double PI = acos(-1.0);
    // any random-ish large odd number will do
    const u64 C = u64(2e18 + PI) + 71;
    // random 32-bit number
    const u32 RANDOM = chrono::steady_clock::now().time_since_epoch().count();
    size_t operator()(u64 x) const {
        return __builtin_bswap64((x ^ RANDOM) * C);
    }
};


#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;

inline gp_hash_table<int, int, custom_hash> hm;
