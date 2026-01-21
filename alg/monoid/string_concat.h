#pragma once
#include "string"

struct Monoid_StringConcat {
    using X = std::string;
    using value_type = X;
    static X id() { return ""; }
    static X op(const X& a, const X& b) { return a + b; }
};
