#pragma once
#include "../header.h"
#include "../numbertheory/modint.h"

constexpr u32 MOD = 998244353;

namespace NTT {

using Z = nt::ModIntBase<u32, MOD>;
constexpr int G = 3; // primitive root modulo MOD

constexpr Z pow_z(Z a, ll e) {
    Z res = 1;
    while (e > 0) {
        if (e & 1) res *= a;
        a *= a;
        e >>= 1;
    }
    return res;
}

int base = 1; // how many levels or roots / rev are prepared
vector<Z> roots = {0, 1}; // roots[k + j] stores the j-th twiddle for stage block-size 2*k
vector<int> rev = {0, 1}; // rev stores bit-reversed indices

void ensure_base(int nbase) {
    if (nbase <= base) return;
    rev.resize(1 << nbase);
    roots.resize(1 << nbase);
    for (int i = 0; i < (1 << nbase); i++) {
        rev[i] = (rev[i >> 1] >> 1) + ((i & 1) << (nbase - 1));
    }
    while (base < nbase) {
        Z z = pow_z(Z(G), (Z::mod() - 1) >> (base + 1));
        for (int i = 1 << (base - 1); i < (1 << base); i++) {
            roots[i << 1] = roots[i];
            roots[(i << 1) | 1] = roots[i] * z;
        }
        base++;
    }
}

void ntt(vector<Z>& a) {
    int n = sz(a);
    assert(n > 0 && (n & (n - 1)) == 0); // power of two
    int lg = __builtin_ctz(n);
    ensure_base(lg);
    int shift = base - lg;

    for (int i = 0; i < n; i++) {
        int j = rev[i] >> shift;
        if (i < j) swap(a[i], a[j]);
    }

    for (int k = 1; k < n; k <<= 1) {
        for (int i = 0; i < n; i += 2 * k) {
            for (int j = 0; j < k; j++) {
                Z w = a[i + j + k] * roots[j + k];
                a[i + j + k] = a[i + j] - w;
                a[i + j] = a[i + j] + w;
            }
        }
    }
}

void intt(vector<Z>& a) {
    int n = sz(a);
    assert(n > 0 && (n & (n - 1)) == 0);
    if (n == 1) return;

    reverse(a.begin()+1, a.end());
    ntt(a);

    Z inv_n = Z(n).inv();
    for (Z& x : a) x *= inv_n;
}

}; // namespace NTT
