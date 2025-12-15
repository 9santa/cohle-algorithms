#include <bits/stdc++.h>
#include "../Number_Theory/binpow.h"
using namespace std;
using u64 = unsigned long long;
using u128 = unsigned __int128;

constexpr u64 mulmod(u64 a, u64 b, const u64 MOD) {
    return u128(a) * u128(b) % MOD;
}

bool miller_rabin(u64 n) {
    if (n < 2 || n % 6 % 4 != -1) return n-2 < 2;
    u64 A[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    u64 s = __builtin_ctzll(n-1), d = n >> s;
    for (auto& a : A) {
        u64 p = binpow(a,d,n), i = s;
        while (p != 1 && p != n-1 && a%n && i--) p = mulmod(p,p,n);
        if (p != n-1 && i != s) return 0;
    }
    return 1;
}
