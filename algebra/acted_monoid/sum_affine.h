#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

#include "../monoid/add.h"
#include "../monoid/affine.h"

/** Acted monoid for affine range updates and range-sum queries. Space: O(1). */
template<typename E>
struct ActedMonoid_Sum_Affine {
    using Monoid_X = Monoid_Add<E>;
    using Monoid_A = Monoid_Affine<E>;
    using X = typename Monoid_X::value_type;
    using A = typename Monoid_A::value_type;

    /** Applies f(v) = a*v + b to a sum aggregate of `size` elements. Time: O(1). */
    static constexpr X act(const X& x, const A& f, const ll& size) {
        return f.first * x + f.second * E(size);
    }
};
