#pragma once
#include "core.h"
#include "carmichael_lambda.h"

namespace nt {

// ---------- multiplicative order ord_n(a) ----------
// returns 0 if order does not exist (i.e. gcd(a,n)!=1 or n==0)
inline u64 multiplicative_order(u64 a, u64 n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    a %= n;
    if (gcd_u64(a, n) != 1) return 0; // requirement

    // since lambda(n) <= phi(n), this is actually better than via Euler's totient
    u64 ord = carmichael_lambda(n); // ord | lambda(n) | phi(n)
    auto fac = factorize_u64_powers(ord);

    for (auto& [p, e] : fac) {
        while (ord % p == 0 && pow_mod(a, ord / p, n) == 1) {
            ord /= p;
        }
    }
    return ord;
}

} // namespace nt
