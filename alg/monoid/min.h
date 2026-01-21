#pragma once
#include "numeric"

template<typename X>
struct Monoid_Min {
    using value_type = X;
    static X id() { return std::numeric_limits<X>::max(); }
    static X op(const X& a, const X& b) { return min(a, b); }
};
