#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Factorial/inverse-factorial table for any field-like type Z with inv(). */
template<class Z>
class Comb {
public:
    int n = 0;
    vector<Z> fac{Z(1)};
    vector<Z> invfac{Z(1)};

    Comb() = default;
    explicit Comb(int m) { init(m); }

    void init(int m) {
        if (m <= n) return;
        fac.resize(m + 1);
        invfac.resize(m + 1);
        for (int i = n + 1; i <= m; ++i) fac[i] = fac[i - 1] * Z(i);
        invfac[m] = fac[m].inv();
        for (int i = m; i > n; --i) invfac[i - 1] = invfac[i] * Z(i);
        n = m;
    }

    Z get_fac(int m) { if (m > n) init(max(m, 2 * max(1, n))); return fac[m]; }
    Z get_invfac(int m) { if (m > n) init(max(m, 2 * max(1, n))); return invfac[m]; }
    Z binom(int n_, int k) {
        if (k < 0 || n_ < k) return Z(0);
        return get_fac(n_) * get_invfac(k) * get_invfac(n_ - k);
    }
};
