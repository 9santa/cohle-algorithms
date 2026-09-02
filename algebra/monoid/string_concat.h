#pragma once
#include <bits/stdc++.h>
using namespace std;

/** String concatenation monoid. Space: O(total string length). */
struct Monoid_StringConcat {
    using value_type = std::string;
    using X = value_type;
    /** Returns the empty string. Time: O(1). */
    static X id() { return ""; }
    /** Returns a + b. Time: O(|a| + |b|). */
    static X op(const X& a, const X& b) { return a + b; }
};
