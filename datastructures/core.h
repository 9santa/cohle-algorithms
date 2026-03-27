#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <new>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
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

#define mp make_pair
#define fi first
#define se second

#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#define UNIQUE(x) sort(all(x)), (x).erase(unique(all(x)), (x).end()), (x).shrink_to_fit()
#define sz(x) int((x).size())
#define pb push_back
#define eb emplace_back
#define rsz resize
#define ft front()
#define bk back()

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define F0R(i, a) FOR(i, 0, a)
#define ROF(i, a, b) for (int i = (b) - 1; i >= (a); --i)
#define R0F(i, a) ROF(i, 0, a)

template<typename T>
using V = vector<T>;
template<typename T, size_t SZ>
using AR = array<T, SZ>;
using vi = V<int>;
using vl = V<ll>;
using vb = V<bool>;
using vpii = V<pii>;

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

template<typename T>
using min_heap = priority_queue<T, vector<T>, greater<T>>;
template<typename T>
using max_heap = priority_queue<T, vector<T>, less<T>>;

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

namespace io {
    template<typename T, template<typename...> class Ref>
    struct is_specialization : false_type {};
    template<template<typename...> class Ref, typename... Args>
    struct is_specialization<Ref<Args...>, Ref> : true_type {};

    inline void setIn(string s) { freopen(s.c_str(), "r", stdin); }
    inline void setOut(string s) { freopen(s.c_str(), "w", stdout); }
    inline void setIO(string s = "") {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);
        if (s.length()) {
            setIn(s + ".in");
            setOut(s + ".out");
        }
    }

    inline void re(double& x) { string t; cin >> t; x = stod(t); }
    inline void re(long double& x) { string t; cin >> t; x = stold(t); }
    template<typename T>
    inline void re(T& x) { cin >> x; }
    template<typename Arg, typename... Args>
    inline void re(Arg& fi, Args&... rest) { re(fi); re(rest...); }
    template<typename T>
    inline istream& operator>>(istream& is, vector<T>& a) {
        F0R(i, sz(a)) is >> a[i];
        return is;
    }

    template<typename T>
    inline void print(const vector<T>& a) { for (const auto& x : a) cout << x << " "; cout << "\n"; }
    template<typename T>
    inline void print(const set<T>& a) { for (const auto& x : a) cout << x << " "; cout << "\n"; }
    template<typename T>
    inline void print(const unordered_set<T>& a) { for (const auto& x : a) cout << x << " "; cout << "\n"; }
    template<typename T>
    inline void print(T&& x) { cout << x << "\n"; }
    template<typename T, typename... S>
    inline void print(T&& x, S&&... y) { cout << x << " "; print(std::forward<S>(y)...); }
}

using namespace io;

struct cmp {
    bool operator()(int l, int r) const { return l > r; }
};

namespace bitty {
inline int popcnt(int x) { return __builtin_popcount(x); }
inline int popcnt(u32 x) { return __builtin_popcount(x); }
inline int popcnt(ll x) { return __builtin_popcountll(x); }
inline int popcnt(u64 x) { return __builtin_popcountll(x); }
inline int popcnt_sign(int x) { return (__builtin_parity(unsigned(x)) & 1 ? -1 : 1); }
inline int popcnt_sign(u32 x) { return (__builtin_parity(x) & 1 ? -1 : 1); }
inline int popcnt_sign(ll x) { return (__builtin_parityll(x) & 1 ? -1 : 1); }
inline int popcnt_sign(u64 x) { return (__builtin_parityll(x) & 1 ? -1 : 1); }
inline int topbit(int x) { return (x == 0 ? -1 : 31 - __builtin_clz(x)); }
inline int topbit(u32 x) { return (x == 0 ? -1 : 31 - __builtin_clz(x)); }
inline int topbit(ll x) { return (x == 0 ? -1 : 63 - __builtin_clzll(x)); }
inline int topbit(u64 x) { return (x == 0 ? -1 : 63 - __builtin_clzll(x)); }
inline int lowbit(int x) { return (x == 0 ? -1 : __builtin_ctz(x)); }
inline int lowbit(u32 x) { return (x == 0 ? -1 : __builtin_ctz(x)); }
inline int lowbit(ll x) { return (x == 0 ? -1 : __builtin_ctzll(x)); }
inline int lowbit(u64 x) { return (x == 0 ? -1 : __builtin_ctzll(x)); }
template<typename T>
T kth_bit(int k) { return T(1) << k; }
template<typename T>
bool has_kth_bit(T x, int k) { return (x >> k) & 1; }
} // namespace bitty

using namespace bitty;
