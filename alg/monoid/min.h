
template<typename E>
struct Monoid_Min {
    using value_type = E;
    using X = value_type;
    static X id() { return std::numeric_limits<X>::max(); }
    static X op(const X& a, const X& b) { return min(a, b); }
};
