#pragma once
#include "core.h"
#include "mod_arith.h"
#include "gcd.h"

namespace nt {

/*
Discrete logarithm (extended Baby-Steps-Giant-Steps):
Find smallest x >= 0 such that a^x ≡ b (mod m), or return -1 if none.

Handles non-coprime case by repeatedly factoring out g = gcd(a, m):
- If b is not divisible by g, no solution.
- Otherwise reduce (b, m) by g and track how many steps we removed.

After reduction we have gcd(a, m) = 1 and can run standard BSGS.

Time:  O(sqrt(m)) expected
Mem:   O(sqrt(m))
*/

// standard BSGS for gcd(a,m) = 1
inline ll bsgs_coprime(u64 a, u64 b, u64 m) {
    a %= m, b %= m;
    if (m == 1) return 0;
    if (b == 1 % m) return 0;

    ll n = (ll)std::sqrt((long double)m) + 1;

    // an = a^n mod m
    u64 an = 1 % m;
    for (ll i = 0; i < n; i++) an = mul_mod(an, a, m);

    // baby steps: store b * a^q (q=0..n)
    // want: (a^n)^p = b * a^q => a^(pn-q) = b
    unordered_map<u64, ll> mp;
    mp.reserve(n+5);
    mp.max_load_factor(0.7f);

    u64 cur = b;
    for (ll q = 0; q <= n; q++) {
        if (!mp.count(cur)) mp[cur] = q; // keep smallest q
        cur = mul_mod(cur, a, m);
    }

    // giant steps: cur = (a^n)^p
    cur = 1 % m;
    for (ll p = 1; p <= n+1; p++) {
        cur = mul_mod(cur, an, m);
        auto it = mp.find(cur);
        if (it != mp.end()) {
            ll q = it->second;
            ll x = p * n - q;
            if (x >= 0) return x;
        }
    }
    return -1;
}


// generalized exBSGS for gcd(a, m) != 1
inline ll exbsgs(u64 a, u64 b, u64 m) {
    assert(gcd(a, m) != 1);
    a = safe_mod(a, m);
    b = safe_mod(b, m);
    if (m == 1) return 0;

    ll cnt = 0;
    ll k = 1;

    while (true) {
        ll g = gcd_ll(a, m);
        if (g == 1) break;
        if (b % g != 0) return -1; // need g | b

        b /= g;
        m /= g;
        k = (i128)((i128)k * (a / g) % m);
        cnt++;

        if (k == b) return cnt; // can return early: k*a^0 = k = b' (mod m')
    }

    ll inv_k = mod_inv(k, m);
    if (inv_k == -1) return -1;

    ll target = (i64)((i128)b * inv_k % m);
    ll y = bsgs_coprime(a, target, m);
    if (y == -1) return -1;
    return y + cnt;
}

} // namespace nt
