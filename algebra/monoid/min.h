#include "../core.h"
/** Minimum monoid. Space: O(1). */
template<typename E>
struct Monoid_Min {
    using value_type = E;
    using X = value_type;
    /** Returns the minimum identity. Time: O(1). */
    static X id() { return std::numeric_limits<X>::max(); }
    /** Returns min(a, b). Time: O(1). */
    static X op(const X& a, const X& b) { return min(a, b); }
};
