#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;

using u128 = unsigned __int128;
using i128 = __int128;
using u64  = unsigned long long;
using i64  = long long;
using ll   = long long;
using u32  = unsigned int;
using u16  = uint16_t;
using u8   = uint8_t;
using ld   = long double;

#define sz(x) int((x).size())

namespace io {
    template<typename T>
    inline void re(T& x) { cin >> x; }
    template<typename Arg, typename... Args>
    inline void re(Arg& fi, Args&... rest) { re(fi); re(rest...); }
}

using namespace io;

namespace bitty {
inline int popcnt(int x) { return __builtin_popcount(x); }
inline int popcnt(u32 x) { return __builtin_popcount(x); }
inline int popcnt(ll x) { return __builtin_popcountll(x); }
inline int popcnt(u64 x) { return __builtin_popcountll(x); }
}

using namespace bitty;
