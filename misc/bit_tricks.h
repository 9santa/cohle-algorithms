#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using u32 = uint32_t;
using u64 = uint64_t;

inline int popcnt(int x) { return __builtin_popcount((unsigned)x); }
inline int popcnt(u32 x) { return __builtin_popcount(x); }
inline int popcnt(ll x) { return __builtin_popcountll((u64)x); }
inline int popcnt(u64 x) { return __builtin_popcountll(x); }

inline int popcnt_sign(int x) { return (__builtin_parity((unsigned)x) ? -1 : 1); }
inline int popcnt_sign(u32 x) { return (__builtin_parity(x) ? -1 : 1); }
inline int popcnt_sign(ll x) { return (__builtin_parityll((u64)x) ? -1 : 1); }
inline int popcnt_sign(u64 x) { return (__builtin_parityll(x) ? -1 : 1); }

inline int topbit(u32 x) { return x ? 31 - __builtin_clz(x) : -1; }
inline int topbit(u64 x) { return x ? 63 - __builtin_clzll(x) : -1; }
inline int topbit(int x) { return topbit((u32)x); }
inline int topbit(ll x) { return topbit((u64)x); }

inline int lowbit(u32 x) { return x ? __builtin_ctz(x) : -1; }
inline int lowbit(u64 x) { return x ? __builtin_ctzll(x) : -1; }
inline int lowbit(int x) { return lowbit((u32)x); }
inline int lowbit(ll x) { return lowbit((u64)x); }

/** Returns the least-significant set-bit mask (0 for x=0). */
template<class T>
constexpr T lowest_set_bit(T x) { return x & -x; }

/** Returns the most-significant set-bit mask (0 for x=0). */
inline u64 highest_set_bit(u64 x) {
    return x ? (u64(1) << topbit(x)) : 0;
}

/** C++20 equivalent of topbit for unsigned values. */
inline int topbit_std(u64 x) { return x ? int(std::bit_width(x)) - 1 : -1; }
