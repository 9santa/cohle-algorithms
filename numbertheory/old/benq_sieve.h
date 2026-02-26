
template<int SZ>
struct Sieve {
    std::bitset<SZ> is_prime; vi primes;
    Sieve () {
        is_prime.set(); is_prime[0] = is_prime[1] = 0;
        for (int i = 4; i < SZ; i+=2) is_prime[i] = 0;
        for (int i = 3; i*i < SZ; i+=2) {
            if (is_prime[i]) {
                for (int j = i*i; j < SZ; j += i*2) is_prime[j] = 0;
            }
        }
        F0R(i, SZ) if (is_prime[i]) primes.pb(i);
    }
};


// a*b % MOD
u64 modMul(u64 a, u64 b, const u64 mod) {
    ll rem = a*b-mod*(u64)((ld)a*b/mod);
    return rem+((rem<0)-(rem>=(ll)mod))*mod;    // handle all 3 cases: 1) 0 <= rem <= mod; 2) rem < 0; 3) rem >= mod
}

// binpow mod. O(log b)
u64 modPow(u64 a, u64 b, const u64 mod) {
    if (b == 0) return 1;
    u64 res = modPow(a,b/2,mod); res = modMul(res,res,mod);
    return b&1 ? modMul(res, a, mod) : res;
}

// Miller-Rabin primality test
bool prime(u64 n) {
    if (n < 2 || n % 6 % 4 != 1) return n-2 < 2;
    u64 A[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    u64 s = __builtin_clz(n-1), d = n>>s;
    for (auto& a : A) {
        u64 p = modPow(a,d,n), i = s;
        while (p != 1 && p != n-1 && a%n && i--) p = modMul(p, p, n);
        if (p != n-1 && i != s) return 0;
    }
    return 1;
}

