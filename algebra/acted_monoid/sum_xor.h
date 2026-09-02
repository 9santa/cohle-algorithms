#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

#include "../monoid/sum.h"
#include "../monoid/xor.h"

/** Acted monoid for range xor updates and range bit-sum queries. Space: O(B). */
template<int B = 20>
struct ActedMonoid_Sum_Xor {
    // Data is an array of bit counts
    using Monoid_X = Monoid_Sum<std::array<ll, B>>;
    // Lazy tags
    using Monoid_A = Monoid_Xor<int>;
    using X = typename Monoid_X::value_type; // std::array<ll, B>
    using A = typename Monoid_A::value_type; // int

    /** Applies xor mask a to bit counts x for size elements. Time: O(B). */
    static X act(const X& x, const A& a, const ll &size) {
        if (a == 0) return x;
        X res = x;
        for (int i = 0; i < B; i++) {
            if ((a >> i) & 1) {
                // if bit i is flipped, new count of 1s is (total size - old count)
                res[i] = size - res[i];
            }
        }
        return res;
    }

    /** Composes xor tags. Time: O(1). */
    static A compose(const A& f, const A& g) {
        return f ^ g;
    }

    /** Converts a scalar to a bit-count array. Time: O(B). */
    static X lift(ll val) {
        X res{};
        for (int i = 0; i < B; i++) {
            res[i] = (val >> i) & 1;
        }
        return res;
    }

    /** Converts bit counts to their numeric sum. Time: O(B). */
    static ll sum(const X& x) {
        ll s = 0;
        for (int i = 0; i < B; i++) {
            s += x[i] * (1ll << i);
        }
        return s;
    }

    /** Returns the zero bit-count aggregate. Time: O(B). */
    static constexpr X id() { return X{}; }
};
