#pragma once

template<typename E>
struct Monoid_GCD {
    using value_type = E;
    using X = value_type;
    static X id() { return 0; }
    static X op(const X& a, const X& b) { return gcd(a, b); }
};
