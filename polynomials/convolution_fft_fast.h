#include "fast_fft.h"

using poly = vector<int>;

static vl convolution_ll_naive(const poly& A, const poly& B) {
    int n = sz(A), m = sz(B);
    vl C(n + m - 1, 0);
    for (int i = 0; i < n; i++) if (A[i]) {
        for (int j = 0; j < m; j++) if (B[j]) {
            C[i + j] += 1LL * A[i] * B[j];
        }
    }
    return C;
}

static vl convolution_ll_fft(const poly& A, const poly& B) {
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

static vl convolution_ll(const poly& A, const poly& B) {
    if (A.empty() || B.empty()) return {};
    if (min(sz(A), sz(B)) <= 80) return convolution_ll_naive(A, B);
    return convolution_ll_fft(A, B);
}
