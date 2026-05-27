#pragma once
#include "../core.h"
#include "../monoid/bidir_monoid.h"

/** Wraps an acted monoid so products are tracked in forward and reverse order. Space: O(1). */
template<class AM>
struct ActedMonoid_Bidir {
    using Monoid_X = Monoid_Bidir<typename AM::Monoid_X>;
    using Monoid_A = typename AM::Monoid_A;
    using X = typename Monoid_X::value_type;
    using A = typename Monoid_A::value_type;
    using X0 = typename AM::Monoid_X::value_type;

    /** Applies action a to both directions of x. Time: O(AM::act). */
    static X act(const X& x, const A& a, const ll& siz) {
        X res;
        res.fwd = AM::act(x.fwd, a, siz);
        res.rev = AM::act(x.rev, a, siz);
        if constexpr (has_fail<X0>::value) {
            res.fail = res.fwd.fail || res.rev.fail;
        }
        return res;
    }
};
