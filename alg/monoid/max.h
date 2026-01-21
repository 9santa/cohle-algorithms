#pragma once
#include "numeric"

template<typename X>
struct Monoid_Max {
    using value_type = X;
    static X id() { return std::numeric_limits<X>::lowest(); }
    static X op(const X& a, const X& b) { return max(a, b); }
};
