#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Monoid of affine functions f(x) = ax + b under composition. Space: O(1). */
template<typename E>
struct Monoid_Affine {
    using value_type = pair<E, E>;
    using X = value_type;
    /** Composes affine functions x then y. Time: O(1). */
    static constexpr X op(const X& x, const X& y) {
        return X({x.first * y.first, x.second * y.first + y.second});
    }
    /** Returns the inverse affine function. Time: O(1). */
    static constexpr X inverse(const X& x) {
        auto& [a, b] = x;
        a = E(1) / a;
        return {a, a * (-b)};
    }
    /** Evaluates affine function f at x. Time: O(1). */
    static constexpr E eval(const X& f, E x) {
        return f.first * x + f.second;
    }
    /** Returns the identity function. Time: O(1). */
    static constexpr X id() { return {E(1), E(0)}; }
    static constexpr bool commute = false;
};
