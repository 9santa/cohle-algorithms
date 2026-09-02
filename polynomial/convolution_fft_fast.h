#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using vl = vector<long long>;
#define sz(x) int((x).size())

namespace CFFT {
using real = double;

/** Lightweight complex number for FFT. Space: O(1). */
struct cmpl {
    real x, y;
    cmpl(real _x=0, real _y=0) : x(_x), y(_y) {}
    inline cmpl operator + (const cmpl& o) const { return cmpl(x+o.x, y+o.y); }
    inline cmpl operator - (const cmpl& o) const { return cmpl(x-o.x, y-o.y); }
    inline cmpl operator * (const cmpl& o) const { return cmpl(x*o.x - y*o.y, x*o.y + y*o.x); }
    inline cmpl operator * (real k)        const { return cmpl(x*k, y*k); }
    inline cmpl conj() const { return cmpl(x, -y); }
};

const real PI = acosl(-1.0);

int base = 1;
vector<cmpl> rts = {cmpl(0, 0), cmpl(1, 0)};
vector<int> rev = {0, 1};

/** Ensures roots/rev are prepared up to nbase. Time: O(2^nbase). */
void ensure_base(int nbase) {
    if (nbase <= base) return;
    rev.resize(1 << nbase);
    rts.resize(1 << nbase);
    for (int i = 0; i < (1 << nbase); i++) {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (nbase - 1));
    }
    while (base < nbase) {
        real angle = PI * 2.0 / (1 << (base + 1));
        for (int i = 1 << (base - 1); i < (1 << base); i++) {
            rts[i << 1] = rts[i];
            real angle_i = angle * (2 * i + 1 - (1 << base));
            rts[(i << 1) + 1] = cmpl(cos(angle_i), sin(angle_i));
        }
        ++base;
    }
}

/** In-place fast FFT. Time: O(n log n). */
void fft(vector<cmpl>& a) {
    int n = sz(a);
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
                cmpl w = a[i + j + k] * rts[j + k];
                a[i + j + k] = a[i + j] - w;
                a[i + j] = a[i + j] + w;
            }
        }
    }
}

/** In-place inverse FFT. Time: O(n log n). */
void ifft(vector<cmpl>& a) {
    int n = sz(a);
    for (auto& z : a) z = z.conj();
    fft(a);
    for (auto& z : a) { z = z.conj(); z.x /= n; z.y /= n; }
}

} // namespace CFFT


using poly = vector<int>;

/** Naive integer convolution. Time: O(nm). */
/** Chooses naive or FFT convolution by input size. Time: O(nm) or O(n log n). */
inline vl convolution_ll_naive(const poly& A, const poly& B) {
    int n = sz(A), m = sz(B);
    vl C(n + m - 1, 0);
    for (int i = 0; i < n; i++) if (A[i]) {
        for (int j = 0; j < m; j++) if (B[j]) {
            C[i + j] += 1LL * A[i] * B[j];
        }
    }
    return C;
}

/** Optimized FFT-backed integer convolution. Time: O(n log n). */
/** FFT-backed integer convolution. Time: O(n log n). */
inline vl convolution_ll_fft_opt(const poly& A, const poly& B) {
    int need = sz(A) + sz(B) - 1;
    int n = 1;
    while (n < need) n <<= 1;
    using namespace CFFT;

    vector<cmpl> P(n);
    for (int i = 0; i < n; i++) {
        P[i] = cmpl(i < sz(A) ? A[i] : 0, i < sz(B) ? B[i] : 0);
    }
    fft(P);
    for (int i = 0; i < n; i++) P[i] = P[i] * P[i];
    ifft(P);
    vl C(need);
    for (int i = 0; i < need; i++) {
        C[i] = (ll)llround(P[i].y / 2);
    }
    return C;
}

inline vl convolution_ll_fft(const poly& A, const poly& B) {
    int need = sz(A) + sz(B) - 1;
    int n = 1;
    while (n < need) n <<= 1;
    using namespace CFFT;

    static vector<cmpl> fa, fb;
    fa.assign(n, cmpl(0, 0));
    fb.assign(n, cmpl(0, 0));
    for (int i = 0; i < sz(A); i++) fa[i].x = (CFFT::real)A[i];
    for (int i = 0; i < sz(B); i++) fb[i].x = (CFFT::real)B[i];

    fft(fa);
    fft(fb);
    for (int i = 0; i < n; i++) fa[i] = fa[i] * fb[i];
    ifft(fa);

    vl C(need);
    for (int i = 0; i < need; i++) C[i] = (ll)llround(fa[i].x);
    return C;
}

inline vl convolution_ll(const poly& A, const poly& B) {
    if (A.empty() || B.empty()) return {};
    if (min(sz(A), sz(B)) <= 80) return convolution_ll_naive(A, B);
    return convolution_ll_fft(A, B);
}
