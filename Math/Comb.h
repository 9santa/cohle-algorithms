#include "../Number_Theory/binpow.h"

constexpr int MAXN = 1'000'000;
const ll MOD = 1'000'000'007;

ll fact[MAXN], invfact[MAXN];

void init_fact() {
    fact[0] = 1;
    for (int i = 1; i < MAXN; i++) {
        fact[i] = fact[i-1] * i % MOD;
    }

    invfact[MAXN-1] = binpow(fact[MAXN-1], MOD-2, MOD);
    for (int i = MAXN-2; i >= 0; i--) {
        invfact[i] = invfact[i+1] * (i+1) % MOD;
    }
}

ll C(int n, int k) {
    if (k < 0 || k > n) return 0;
    return fact[n] * invfact[k] % MOD * invfact[n-k] % MOD;
}
