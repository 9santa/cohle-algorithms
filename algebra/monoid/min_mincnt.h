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

/** Monoid storing {minimum, count of minimum}. Space: O(1). */
template<typename E>
struct Monoid_MinMincnt {
    using value_type = pair<E, E>;
    using X = value_type;
    /** Combines two {minimum, count} aggregates. Time: O(1). */
    static X op(X x, X y) {
        auto [xmin, xmincnt] = x;
        auto [ymin, ymincnt] = y;
        if (xmin > ymin) return y;
        if (ymin > xmin) return x;
        return {xmin, xmincnt + ymincnt};
    }
    /** Returns the neutral aggregate. Time: O(1). */
    static constexpr X id() { return {infty<E>, 0}; }
    static constexpr bool commute = true;
};
