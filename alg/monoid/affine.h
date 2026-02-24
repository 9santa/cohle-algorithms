#include "../../header.h"

template<typename E>
struct Monoid_Affine {
    using value_type = pair<E, E>;
    using X = value_type;
    static constexpr X op(const X& x, const X& y) {
        return X({x.first * y.first, x.second * y.first + y.second});
    }
    static constexpr X inverse(const X& x) {
        auto& [a, b] = x;
        a = E(1) / a;
        return {a, a * (-b)};
    }
    static constexpr E eval(const X& f, E x) {
        return f.first * x + f.second;
    }
    static constexpr X id() { return {E(1), E(0)}; }
    static constexpr bool commute = false;
};
