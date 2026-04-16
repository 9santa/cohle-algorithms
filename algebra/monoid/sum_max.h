#include "../core.h"

template<typename E>
struct Monoid_SumMax {
    using value_type = pair<E, E>;
    using X = value_type;
    static X op(X x, X y) { return {x.first + y.first, max(x.second, y.second)}; }
    static X from_element(E e) { return {e, e}; }
    static constexpr X id() { return {E(0), -infty<E>}; }
    static constexpr bool commute = 1;
};
