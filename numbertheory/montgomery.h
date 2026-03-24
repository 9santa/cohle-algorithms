#include "core.h"
#include "gcd.h"

namespace nt {

struct Montgomery64 {
    u64 mod; // odd modulus
    u64 inv; // inv = (-mod)^-1 mod R
    u128 R;
    u64 R2;

    Montgomery64(u64 m) : mod(m) {
        assert(mod & 1);

        u128 R = (u128(1) << 64) % mod;
        inv = mod_inv(-mod, R);
        R2 = (u64)(R * R % mod);
    }

    // Convert x to Montgomery space
    u64 init(u64 x) const {
        return reduce((u128)x * R2);
    }

    u64 reduce(u128 T) const {
        u64 m = (u64)T * inv;
        u64 t = (T + (u128)m * mod) >> 64; // the whole point, bit ops instead of division and %
        if (t >= mod) t -= mod;
        return t;
    }

    u64 mul(u64 a, u64 b) const {
        return reduce((u128)a * b);
    }

    u64 get(u64 a) const {
        return reduce(a);
    }

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
