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

// f(x) = max(x, a) + b
/*
Composition: f(x) = max(x + a, c), g(x) = max(x + d, f)
h(x) = g(f(x)) = max(max(x+a, c) + d, f)
h.first = a + d
h.second = max(c + d, f)
*/
/** Monoid of functions f(x) = max(x + add, lower). Space: O(1). */
template<typename E>
struct Monoid_Add_Chmax {
    using value_type = pair<E, E>; // first -> how much to add, second -> lower bound after adding
    using X = value_type;
    /** Composes two functions. Time: O(1). */
    static X op(X x, X y) {
        auto& [a, c] = x;
        auto& [d, f] = y;
        a = a + d;
        c = (c == -infty<E> ? c : c + d);
        c = max(c, f);
        return {a, c};
    }
    /** Evaluates function f at a. Time: O(1). */
    static E eval(X f, E a) {
        return max(a + f.first, f.second);
    }
    /** Creates an add function. Time: O(1). */
    static X add(E a) { return {a, -infty<E>}; }
    /** Creates a chmax function. Time: O(1). */
    static X chmax(E a) { return {0, a}; }
    /** Returns the identity function. Time: O(1). */
    static constexpr X id() { return {0, -infty<E>}; }
    static constexpr bool commute = false;
};
