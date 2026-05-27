#include "../core.h"
#include "../monoid/add.h"
#include "../monoid/max.h"

/** Acted monoid for range add and range maximum. Space: O(1). */
template<typename E>
struct ActedMonoid_Max_Add {
    using Monoid_X = Monoid_Max<E>;
    using Monoid_A = Monoid_Add<E>;
    using X = typename Monoid_X::value_type;
    using A = typename Monoid_A::value_type;
    /** Applies an add tag to a maximum aggregate. Time: O(1). */
    static constexpr X act(const X& x, const A& a, const ll& size) {
        if (x == -infty<E>) return x;
        return x + a;
    }
};
