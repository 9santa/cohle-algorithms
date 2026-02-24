
template<typename E>
struct Monoid_Sum {
    using value_type = E;
    using X = value_type;
    static constexpr X op(const X& x, const X& y) { return x + y; }
    static constexpr X inverse(const X& x) { return -x; }
    static constexpr X multiply(const X& x, ll b) { return x * X(b); }
    static constexpr X id() { return 0; }
    static constexpr bool commute = true;
};
