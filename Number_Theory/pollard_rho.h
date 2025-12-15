#include <bits/stdc++.h>
#include "./miller_rabin.cpp"
#include "./binary_gcd.h"

u64 f(u64 x, u64 c, u64 mod) {
    return (mulmod(x, x, mod) + c) % mod;
}

// pollard rho
u64 pollard(u64 n) {
    if (n%2 == 0) return 2;
    if (n%3 == 0) return 3;

    u64 x = 2, y = 2, c = rand() % (n-1) + 1;
    u64 d = 1;

    while (d == 1) {
        x = f(x, c, n);
        y = f(f(y, c, n), c, n);
        u64 diff = x > y ? x-y : y-x;
        d = bgcd(diff, n);
    }

    if (d == n) return pollard(n);
    return d;
}

// factorize n
vector<u64> factors;
void factor(u64 n) {
    if (n==1) return;
    if (miller_rabin(n)) factors.push_back(n);
    else {
        u64 d = pollard(n);
        factor(d);
        factor(n/d);
    }
}
