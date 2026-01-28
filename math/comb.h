#include "ModInt.h"

class Comb {
public:
    int n;
    vector<Z> fac;
    vector<Z> invfac;

    Comb() : n(0), fac{1}, invfac{1} {}
    Comb(int _n) : Comb() {
        init(_n);
    }

    // dynamic initialization, can expand effectively
    void init(int m) {
        if (m <= n) return;
        fac.resize(m+1);
        invfac.resize(m+1);

        for (int i = n+1; i <= m; i++) {
            fac[i] = fac[i-1] * i;
        }
        invfac[m] = fac[m].inv();
        for (int i = m; i > n; i--) {
            invfac[i-1] = invfac[i] * i;
        }
        n = m;
    }

    Z get_fac(int m) {
        if (m > n) init(2 * m);
        return fac[m];
    }

    Z get_invfac(int m) {
        if (m > n) init(2 * m);
        return invfac[m];
    }

    Z binom(int n, int k) {
        if (n < k || k < 0) return 0;
        return get_fac(n) * get_invfac(k) * get_invfac(n - k);
    }
};

