#include "../core.h"
#include "../monoid/add.h"

/** Acted monoid for range add and range sum. Space: O(1). */
template<typename E>
struct ActedMonoid_Sum_Add {
    using Monoid_X = Monoid_Add<E>;
    using Monoid_A = Monoid_Add<E>;
    using X = typename Monoid_X::value_type;
    using A = typename Monoid_A::value_type;
    /** Applies an add tag to a sum aggregate of length size. Time: O(1). */
    static constexpr X act(const X& x, const A &a, const ll &size) {
        return x + a * E(size);
    }
};
