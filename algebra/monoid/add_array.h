#include "../core.h"
/** Component-wise additive monoid over fixed-size arrays. Space: O(K). */
template<typename E, int K>
struct Monoid_Add_Array {
    using value_type = array<E, K>;
    using X = value_type;
    /** Returns component-wise x + y. Time: O(K). */
    static X op(X x, X y) {
        for (int i = 0; i < K; i++) {
            x[i] += y[i];
        }
        return x;
    }
    /** Returns the zero array. Time: O(K). */
    static constexpr X id() { return X{}; }
    /** Returns component-wise negation. Time: O(K). */
    static constexpr X inverse(X x) {
        for (auto& v : x) v = -v;
        return x;
    }
    /** Returns component-wise x multiplied by b. Time: O(K). */
    static constexpr X multiply(X x, ll b) {
        for (auto& v : x) v *= E(b);
        return x;
    }
    static constexpr bool commute = 1;
};
