#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
#ifndef COHLE_INFINITY_HELPER
#define COHLE_INFINITY_HELPER
template<class T> constexpr T infty = 0;
template<> constexpr int infty<int> = 1'010'000'000;
template<> constexpr long long infty<long long> = 2'020'000'000'000'000'000;
template<> constexpr double infty<double> = numeric_limits<double>::infinity();
template<> constexpr long double infty<long double> = numeric_limits<long double>::infinity();
#endif

#include "../monoid/sum_max.h"
#include "../monoid/add.h"

/** Acted monoid for range add with sum and maximum aggregates. Space: O(1). */
template<typename E>
struct ActedMonoid_SumMax_Add {
    using Monoid_X = Monoid_SumMax<E>;
    using Monoid_A = Monoid_Add<E>;
    using X = typename Monoid_X::value_type;
    using A = typename Monoid_A::value_type;
    /** Applies an add tag to {sum, max} over size elements. Time: O(1). */
    static constexpr X act(const X& x, const A& a, const ll& size) {
        auto [xs, xm] = x;
        xm = (xm == -infty<E> ? xm : xm + a);
        return {xs + E(size) * a, xm};
    }
};
