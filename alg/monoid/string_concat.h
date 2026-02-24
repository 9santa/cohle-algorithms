
struct Monoid_StringConcat {
    using value_type = std::string;
    using X = value_type;
    static X id() { return ""; }
    static X op(const X& a, const X& b) { return a + b; }
};
