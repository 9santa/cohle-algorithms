#include "../../header.h"
#include "../monoid/sum.h"
#include "../monoid/xor.h"

template<int B = 20>
struct ActedMonoid_Sum_Xor {
    // Data is an array of bit counts
    using Monoid_X = Monoid_Sum<std::array<ll, B>>;
    // Lazy tags
    using Monoid_A = Monoid_Xor<int>;
    using X = typename Monoid_X::value_type; // std::array<ll, B>
    using A = typename Monoid_A::value_type; // int

    // Apply the XOR mask 'a' to the bit counts 'x'
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

    static A compose(const A& f, const A& g) {
        return f ^ g;
    }

    // Convert input number to bit-count array
    static X lift(ll val) {
        X res{};
        for (int i = 0; i < B; i++) {
            res[i] = (val >> i) & 1;
        }
        return res;
    }

    // Convert bit-count array back to num
    static ll sum(const X& x) {
        ll s = 0;
        for (int i = 0; i < B; i++) {
            s += x[i] * (1ll << i);
        }
        return s;
    }

    static constexpr X id() { return X{}; }
};
