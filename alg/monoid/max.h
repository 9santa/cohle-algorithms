#pragma once
#include "numeric"

template<typename E>
struct Monoid_Max {
    using value_type = E;
    using X = value_type;
    static X id() { return std::numeric_limits<X>::lowest(); }
    static X op(const X& a, const X& b) { return max(a, b); }
};
