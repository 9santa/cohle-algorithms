#include "../core.h"
/** Xor monoid with inverse and repeated operation helper. Space: O(1). */
template<typename E>
struct Monoid_Xor {
    using value_type = E;
    using X = value_type;
    /** Returns x xor y. Time: O(1). */
    static X op(X x, X y) { return x ^ y; }
    /** Returns the xor inverse of x, which is x itself. Time: O(1). */
    static constexpr X inverse(const X& x) { return x; }
    /** Returns x xor-ed with itself b times. Time: O(1). */
    static constexpr X multiply(const X& x, ll b) {
        return (b & 1 ? x : 0);
    }
    /** Returns the xor identity. Time: O(1). */
    static constexpr X id() { return X(0); }
    static constexpr bool commute = true;
};
