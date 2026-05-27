#include "../core.h"
// Typical usage: RMQ
/** Comparator-style monoid helper for maximum value with leftmost index. Space: O(1). */
template<typename E>
struct ArgMax {
    using X = pair<E, int>; // (value, index)
    /** Returns whether a should be ordered before b. Time: O(1). */
    static bool op(const X& a, const X& b) {
        if (a.first != b.first) return a.first > b.first;
        return a.second < b.second;
    }
    /** Returns the neutral sentinel. Time: O(1). */
    static X id() { return {std::numeric_limits<E>::lowest(), INT_MAX}; }
};
