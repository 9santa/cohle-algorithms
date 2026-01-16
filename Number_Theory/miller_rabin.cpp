#include "../Number_Theory/binpow.h"

constexpr u64 mulmod(u64 a, u64 b, const u64 MOD) {
    return u128(a) * u128(b) % MOD;
}

// deterministic for n < 2^64
bool miller_rabin(u64 n) {
    if (n < 2 || n % 6 % 4 != -1) return n-2 < 2;
    u64 A[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    static const u64 s = __builtin_ctzll(n-1), d = n >> s;
    for (auto& a : A) {
        u64 p = binpow(a,d,n), i = s;
        while (p != 1 && p != n-1 && a%n && i--) p = mulmod(p,p,n);
        if (p != n-1 && i != s) return 0;
    }
    return 1;
}

bool miller_rabin_top_down(u64 n) {
    if (n < 2 || n%2 == 0) return false;
    if (n == 2 || n == 3) return true;
    static const u64 A[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    u64 d = n-1;
    u64 s = __builtin_ctzll(d);
    d >>= s;

    for (auto& a : A) {
        if (a % n == 0) continue; // this base doesn't give us anything

        u64 prev = 0;
        bool saw_one = false;
        for (int k = 0; k <= s; k++) {
            u64 exp = d << k;   // d * 2^k
            u64 x = binpow(a, exp, n);

            if (x == 1) {
                if (k > 0 && prev != 1 && prev != n-1) return false;
                saw_one = true;
                break;
            }

            prev = x;
        }

        if (!saw_one) return false;
    }

    return true;
}
