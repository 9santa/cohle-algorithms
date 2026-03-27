#pragma once

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

using u64 = unsigned long long;
using u32 = unsigned int;
using u8 = uint8_t;
using ll = long long;

template<typename T>
using V = vector<T>;
using vi = V<int>;

#define all(x) (x).begin(), (x).end()
#define sz(x) int((x).size())

template<typename T>
inline istream& operator>>(istream& is, vector<T>& a) {
    for (int i = 0; i < sz(a); i++) is >> a[i];
    return is;
}

template<typename T>
inline void re(T& x) { cin >> x; }
template<typename Arg, typename... Args>
inline void re(Arg& fi, Args&... rest) { re(fi); re(rest...); }
