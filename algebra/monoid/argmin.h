#pragma once
#include <bits/stdc++.h>
using namespace std;

// Typical usage: RMQ
/** Comparator-style monoid helper for minimum value with leftmost index. Space: O(1). */
template<typename E>
struct ArgMin {
    using X = pair<E, int>; // (value, index)
    /** Returns whether a should be ordered before b. Time: O(1). */
    static bool op(const X& a, const X& b) {
        return a < b; // lexicographic => leftmost on ties
    }
    /** Returns the neutral sentinel. Time: O(1). */
    static X id() { return {std::numeric_limits<E>::max(), INT_MAX}; }
};
