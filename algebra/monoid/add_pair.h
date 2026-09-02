#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Component-wise additive monoid over pairs. Space: O(1). */
template<typename E>
struct Monoid_Add_Pair {
    using value_type = pair<E, E>;
    using X = value_type;
    /** Returns component-wise x + y. Time: O(1). */
    static constexpr X op(const X &x, const X& y) {
        return {x.first + y.first, x.second + y.second};
    }
    /** Returns component-wise negation. Time: O(1). */
    static constexpr X inverse(const X &x) { return {-x.first, -x.second}; }
    /** Returns the zero pair. Time: O(1). */
    static constexpr X id() { return {0, 0}; }
    static constexpr bool commute = true;
};
