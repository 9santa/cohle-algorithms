#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/** Additive monoid with inverse and scalar multiplication. Space: O(1). */
template<typename E>
struct Monoid_Add {
    using value_type = E;
    using X = value_type;
    /** Returns x + y. Time: O(1). */
    static constexpr X op(const X& x, const X& y) { return x + y; }
    /** Returns -x. Time: O(1). */
    static constexpr X inverse(const X& x) { return -x; }
    /** Returns x multiplied by b. Time: O(1). */
    static constexpr X multiply(const X& x, ll b) { return x * X(b); }
    /** Returns the additive identity. Time: O(1). */
    static constexpr X id() { return 0; }
    static constexpr bool commute = true;
};
