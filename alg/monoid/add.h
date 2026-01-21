#pragma once

template<typename X>
struct Monoid_Sum {
    using value_type = X;
    static X id() { return 0; }
    static X op(const X& a, const X& b) { return a + b; }
};
