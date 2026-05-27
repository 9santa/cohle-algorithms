#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <bit>
#include <compare>
#include <concepts>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <random>
#include <tuple>
#include <type_traits>
#include <unordered_map>
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

using pii = pair<int, int>;

#define all(x) (x).begin(), (x).end()
#define sz(x) int((x).size())
#define pb push_back

namespace nt {}
