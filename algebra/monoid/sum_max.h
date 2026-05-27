#include "../core.h"

/** Monoid storing {sum, maximum}. Space: O(1). */
template<typename E>
struct Monoid_SumMax {
    using value_type = pair<E, E>;
    using X = value_type;
    /** Combines adjacent {sum, max} aggregates. Time: O(1). */
    static X op(X x, X y) { return {x.first + y.first, max(x.second, y.second)}; }
    /** Converts one element to an aggregate. Time: O(1). */
    static X from_element(E e) { return {e, e}; }
    /** Returns the neutral aggregate. Time: O(1). */
    static constexpr X id() { return {E(0), -infty<E>}; }
    static constexpr bool commute = 1;
};
