#pragma once

template<typename X>
struct Monoid_GCD {
    using value_type = X;
    static X id() { return 0; }
    static X op(const X& a, const X& b) { return gcd(a, b); }
};
