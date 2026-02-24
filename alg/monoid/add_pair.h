
template<typename E>
struct Monoid_Add_Pair {
    using value_type = pair<E, E>;
    using X = value_type;
    static constexpr X op(const X &x, const X& y) {
        return {x.first + y.first, x.second + y.second};
    }
    static constexpr X inverse(const X &x) { return {-x.first, -x.second}; }
    static constexpr X id() { return {0, 0}; }
    static constexpr bool commute = true;
};
