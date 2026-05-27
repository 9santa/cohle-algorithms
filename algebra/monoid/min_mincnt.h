#include "../core.h"

/** Monoid storing {minimum, count of minimum}. Space: O(1). */
template<typename E>
struct Monoid_MinMincnt {
    using value_type = pair<E, E>;
    using X = value_type;
    /** Combines two {minimum, count} aggregates. Time: O(1). */
    static X op(X x, X y) {
        auto [xmin, xmincnt] = x;
        auto [ymin, ymincnt] = y;
        if (xmin > ymin) return y;
        if (ymin > xmin) return x;
        return {xmin, xmincnt + ymincnt};
    }
    /** Returns the neutral aggregate. Time: O(1). */
    static constexpr X id() { return {infty<E>, 0}; }
    static constexpr bool commute = true;
};
