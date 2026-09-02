#pragma once
#include <bits/stdc++.h>
using namespace std;

/** GCD monoid over integral values. Space: O(1). */
template<typename E>
struct Monoid_GCD {
    using value_type = E;
    using X = value_type;
    /** Returns the gcd identity. Time: O(1). */
    static X id() { return 0; }
    /** Returns gcd(a, b). Time: O(log min(a, b)). */
    static X op(const X& a, const X& b) { return gcd(a, b); }
};
