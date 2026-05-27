#include "core.h"
#include "gcd.h"

namespace nt {

/** Montgomery multiplication helper for an odd 64-bit modulus. Space: O(1). */
struct Montgomery64 {
    u64 mod; // odd modulus
    u64 inv; // inv = (-mod)^-1 mod R
    u128 R;
    u64 R2;

    /** Initializes constants for modulus m. Requires m odd. Time: O(log m). */
    Montgomery64(u64 m) : mod(m) {
        assert(mod & 1);

        u128 R = (u128(1) << 64) % mod;
        inv = mod_inv(-mod, R);
        R2 = (u64)(R * R % mod);
    }

    /** Converts x to Montgomery space. Time: O(1). */
    u64 init(u64 x) const {
        return reduce((u128)x * R2);
    }

    /** Performs Montgomery reduction. Time: O(1). */
    u64 reduce(u128 T) const {
        u64 m = (u64)T * inv;
        u64 t = (T + (u128)m * mod) >> 64; // the whole point, bit ops instead of division and %
        if (t >= mod) t -= mod;
        return t;
    }

    /** Multiplies two Montgomery-space values. Time: O(1). */
    u64 mul(u64 a, u64 b) const {
        return reduce((u128)a * b);
    }

    /** Converts a Montgomery-space value back to normal representation. Time: O(1). */
    u64 get(u64 a) const {
        return reduce(a);
    }

    /** Returns a^e modulo mod. Time: O(log e). */
    u64 pow(u64 a, u64 e) const {
        u64 x = init(a);
        u64 res = init(1);
        while (e) {
            if (e & 1) res = mul(res, x);
            x = mul(x, x);
            e >>= 1;
        }
        return get(res);
    }
};

} // namespace nt
