#pragma once
#include "core.h"
#include "trial_factor.h"
#include "mod_arith.h"
#include "../random/core.h"

namespace nt {

inline int primitive_root(int p) {
    auto pf = factorize_trial(p-1);
    auto is_root = [&](int g) -> bool {
        for (auto& [q, e] : pf) {
            if (pow_mod(g, (p-1) / q, p) == 1) return false;
        }
        return true;
    };
    while (1) {
        int g = RNG(1, p);
        if (is_root(g)) return g;
    }
    return -1;
}

inline ll primitive_root_ll(ll p) {
    auto pf = factorize_trial(p-1);
    auto is_root = [&](ll g) -> bool {
        for (auto& [q, e] : pf) {
            if (pow_mod(g, (p-1) / q, p) == 1) return false;
        }
        return true;
    };
    while (1) {
        ll g = RNG(1, p);
        if (is_root(g)) return g;
    }
    return -1;
}

} // namespace nt
