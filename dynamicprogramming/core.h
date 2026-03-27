#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdint>
#include <deque>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
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

template<typename T>
using V = vector<T>;
using vi = V<int>;
using vl = V<ll>;
using vb = V<bool>;

#define all(x) (x).begin(), (x).end()
#define sz(x) int((x).size())
#define pb push_back
#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define F0R(i, a) FOR(i, 0, a)

template<class T>
constexpr T infty = 0;
template<>
constexpr int infty<int> = 1'010'000'000;
template<>
constexpr ll infty<ll> = 2'020'000'000'000'000'000;
template<>
constexpr i128 infty<i128> = i128(infty<ll>) * 2'020'000'000'000'000'000;
template<>
constexpr double infty<double> = numeric_limits<double>::infinity();
template<>
constexpr long double infty<long double> = numeric_limits<long double>::infinity();

template<typename T, typename S>
constexpr bool setmin(T& x, const S& y) {
    if (y < x) {
        x = y;
        return true;
    }
    return false;
}

template<typename T, typename S>
constexpr bool setmax(T& x, const S& y) {
    if (y > x) {
        x = y;
        return true;
    }
    return false;
}

template<typename T>
inline istream& operator>>(istream& is, vector<T>& a) {
    for (int i = 0; i < sz(a); i++) is >> a[i];
    return is;
}

template<typename T>
inline void re(T& x) { cin >> x; }
template<typename Arg, typename... Args>
inline void re(Arg& fi, Args&... rest) { re(fi); re(rest...); }

template<typename T>
inline void print(const vector<T>& a) { for (const auto& x : a) cout << x << " "; cout << "\n"; }
template<typename T>
inline void print(T&& x) { cout << x << "\n"; }
template<typename T, typename... S>
inline void print(T&& x, S&&... y) { cout << x << " "; print(std::forward<S>(y)...); }
