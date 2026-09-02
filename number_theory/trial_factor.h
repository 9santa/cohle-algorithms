#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using vl = vector<long long>;

namespace nt {

/** Returns prime factors of n with multiplicity. Time: O(sqrt n). Space: O(number of factors). */
inline vl prime_factors_trial(ll n) {
    vl f;
    if (n < 0) n = -n;
    for (ll p = 2; p * p <= n; p += (p == 2 ? 1 : 2)) {
        while (n % p == 0) {
            f.push_back(p);
            n /= p;
        }
    }
    if (n > 1) f.push_back(n);
    return f;
}

/** Returns factorization of n as {prime, exponent}. Time: O(sqrt n). Space: O(number of distinct factors). */
inline vector<pair<ll, int>> factorize_trial(ll n) {
    vector<pair<ll, int>> res;
    if (n < 0) n = -n;
    for (ll p = 2; p * p <= n; p += (p == 2 ? 1 : 2)) {
        if (n % p != 0) continue;
        int e = 0;
        while (n % p == 0) n /= p, e++;
        res.push_back({p, e});
    }
    if (n > 1) res.push_back({n, 1});
    return res;
}

} // namespace nt
