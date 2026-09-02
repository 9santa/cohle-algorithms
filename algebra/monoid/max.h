#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Maximum monoid. Space: O(1). */
template<typename E>
struct Monoid_Max {
    using value_type = E;
    using X = value_type;
    /** Returns the maximum identity. Time: O(1). */
    static X id() { return std::numeric_limits<X>::lowest(); }
    /** Returns max(a, b). Time: O(1). */
    static X op(const X& a, const X& b) { return max(a, b); }
};
