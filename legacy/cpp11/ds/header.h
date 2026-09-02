#pragma once
// C++11-local copy of the project header for datastructures/cpp11.
// Replaced newer root-header features with C++11 equivalents.

#include <bits/stdc++.h>
using namespace std;

#define debug(x) std::cout << __FUNCTION__ << ":" << __LINE__ << " " << #x << " = " << x << std::endl

typedef unsigned __int128 u128;
typedef __int128 i128;
typedef unsigned long long u64;
typedef long long i64;
typedef long long ll;
typedef unsigned int u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef long double ld;

typedef std::pair<int, int> pii;
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
using V = std::vector<T>;
template<typename T, size_t SZ>
using AR = std::array<T, SZ>;
typedef V<int> vi;
typedef V<ll> vl;
typedef V<bool> vb;
typedef V<pii> vpii;

template<typename T> struct infinity_value;
template<> struct infinity_value<int> { static const int value = 1010000000; };
template<> struct infinity_value<ll> { static const ll value = 2020000000000000000LL; };
template<> struct infinity_value<i128> { static const i128 value; };
template<> struct infinity_value<double> { static double value() { return numeric_limits<double>::infinity(); } };
template<> struct infinity_value<long double> { static long double value() { return numeric_limits<long double>::infinity(); } };

const i128 infinity_value<i128>::value =
    i128(infinity_value<ll>::value) * 2020000000000000000LL;

template<typename T, typename S> inline bool setmin(T& x, const S& y) {
    if (y < x) {
        x = y;
        return true;
    }
    return false;
}

template<typename T, typename S> inline bool setmax(T& x, const S& y) {
    if (y > x) {
        x = y;
        return true;
    }
    return false;
}

namespace io {
    template<typename T, template<typename...> class Ref>
    struct is_specialization : std::false_type {};
    template<template<typename...> class Ref, typename... Args>
    struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

    inline void setIn(std::string s) { freopen(s.c_str(), "r", stdin); }
    inline void setOut(std::string s) { freopen(s.c_str(), "w", stdout); }
    inline void setIO(std::string s = "") {
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);
        if (s.length()) {
            setIn(s + ".in");
            setOut(s + ".out");
        }
    }

    inline void re(double& x) { std::string t; std::cin >> t; x = std::stod(t); }
    inline void re(long double& x) { std::string t; std::cin >> t; x = std::stold(t); }
    template<typename T> inline void re(T& x) { std::cin >> x; }
    template<typename Arg, typename... Args> inline void re(Arg& fi, Args&... rest) { re(fi); re(rest...); }
    template<typename T> inline std::istream& operator>>(std::istream& is, std::vector<T>& a) {
        F0R(i, sz(a)) is >> a[i];
        return is;
    }

    template<typename T> inline void print(const std::vector<T>& a) { for (typename std::vector<T>::const_iterator it = a.begin(); it != a.end(); ++it) std::cout << *it << " "; std::cout << "\n"; }
    template<typename T> inline void print(const std::set<T>& a) { for (typename std::set<T>::const_iterator it = a.begin(); it != a.end(); ++it) std::cout << *it << " "; std::cout << "\n"; }
    template<typename T> inline void print(const std::unordered_set<T>& a) { for (typename std::unordered_set<T>::const_iterator it = a.begin(); it != a.end(); ++it) std::cout << *it << " "; std::cout << "\n"; }
    template<typename T> inline void print(T&& x) { std::cout << x << "\n"; }
    template<typename T, typename... S> inline void print(T&& x, S&&... y) { std::cout << x << " "; print(std::forward<S>(y)...); }
}

using namespace io;

struct cmp { bool operator()(int l, int r) const { return l > r; } };

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
template<typename T> inline T kth_bit(int k) { return T(1) << k; }
template<typename T> inline bool has_kth_bit(T x, int k) { return (x >> k) & 1; }
} // namespace bitty
using namespace bitty;
