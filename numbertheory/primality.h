#pragma once
#include "core.h"
#include "mod_arith.h"

namespace nt {

/** Returns whether n is prime using deterministic Miller-Rabin for u64. Time: O(log n). Space: O(1). */
inline bool is_prime_u64(u64 n) {
    if (n < 2) return false;
    // quick trial division by some small primes (fast)
    static const u64 A_small[] = {2ULL,3ULL,5ULL,7ULL,11ULL,13ULL,17ULL,19ULL,23ULL,29ULL,31ULL,37ULL};
    for (auto p : A_small) {
        if (n == p) return true;
        if (n % p == 0) return false;
    }

    u64 d = n-1;
    int s = __builtin_ctzll(d);
    d >>= s;

    auto witness = [&](u64 a) -> bool {
        if (a % n == 0) return false; // if a is multiple of n, it gives no info
        u64 x = pow_mod(a, d, n);
        if (x == 1 || x == n-1) return false; // the criteria for a base to pass
        for (int r = 1; r < s; r++) {
            x = mul_mod(x, x, n);
            if (x == n-1) return false; // passes for this base
        }
        return true; // never hit -1 => def composite
    };

    // deterministic for 64-bit
    static const u64 A[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    for (auto a : A) {
        if (witness(a)) return false;
    }

    return true;
}

} // namespace nt
