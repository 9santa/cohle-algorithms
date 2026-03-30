#pragma once

#include <array>
#include <climits>
#include <cstdint>
#include <limits>
#include <numeric>
#include <string>
#include <type_traits>
#include <utility>

using namespace std;

using ll = long long;

#define sz(x) int((x).size())

template<class T>
constexpr T infty = 0;
template<>
constexpr int infty<int> = 1'010'000'000;
template<>
constexpr ll infty<ll> = 2'020'000'000'000'000'000;
template<>
constexpr double infty<double> = numeric_limits<double>::infinity();
template<>
constexpr long double infty<long double> = numeric_limits<long double>::infinity();
