#pragma once
#include "mod_arith.h"
#include "pollard_rho.h"
#include "gcd.h"

namespace nt {


inline u64 carmichael_prime_power(u64 p, int e) {
    if (p == 2) {
        if (e == 1) return 1;   // lambda(2)=1
        if (e == 2) return 2;   // lambda(4)=2
        return 1ULL << (e - 2); // lambda(2^e)=2^(e-2), e>=3
    }

    // lambda(p^e) = phi(p^e) = p^(e-1) * (p-1) for odd prime p
    u64 pe_1 = 1;
    for (int i = 0; i < e-1; i++) pe_1 *= p;
    return pe_1 * (p - 1);
}

inline u64 carmichael_lambda(u64 n) {
    if (n == 0) return 0; // technically undefined
    if (n == 1) return 1;

    auto fac = factorize_u64_powers(n);
    u64 ans = 1;
    for (auto& [p, e] : fac) {
        ans = lcm_u64(ans, carmichael_prime_power(p, e));
    }
    return ans;
}

} // namespace nt
