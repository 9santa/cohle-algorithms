
template<typename E>
struct Monoid_Xor {
    using value_type = E;
    using X = value_type;
    static X op(X x, X y) { return x ^ y; }
    static constexpr X inverse(const X& x) { return x; }
    static constexpr X multiply(const X& x, ll b) {
        return (b & 1 ? x : 0);
    }
    static constexpr X id() { return X(0); }
    static constexpr bool commute = true;
};
