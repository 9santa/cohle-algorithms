#pragma once
#include <bits/stdc++.h>
using namespace std;

#ifndef COHLE_INFINITY_HELPER
#define COHLE_INFINITY_HELPER
template<class T> constexpr T infty = 0;
template<> constexpr int infty<int> = 1'010'000'000;
template<> constexpr long long infty<long long> = 2'020'000'000'000'000'000;
template<> constexpr double infty<double> = numeric_limits<double>::infinity();
template<> constexpr long double infty<long double> = numeric_limits<long double>::infinity();
#endif

/** Monoid storing {sum, maximum}. Space: O(1). */
template<typename E>
struct Monoid_SumMax {
    using value_type = pair<E, E>;
    using X = value_type;
    /** Combines adjacent {sum, max} aggregates. Time: O(1). */
    static X op(X x, X y) { return {x.first + y.first, max(x.second, y.second)}; }
    /** Converts one element to an aggregate. Time: O(1). */
    static X from_element(E e) { return {e, e}; }
    /** Returns the neutral aggregate. Time: O(1). */
    static constexpr X id() { return {E(0), -infty<E>}; }
    static constexpr bool commute = 1;
};
