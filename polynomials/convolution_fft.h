#include "fft.h"

inline vl convolution(const vi& A, const vi& B) {
    int n = 1;
    while (n < sz(A) + sz(B) - 1) n *= 2;
    vector<cmpl> acmpl(n, 0);
    vector<cmpl> bcmpl(n, 0);
    for (int j = 0; j < sz(A); j++) acmpl[j] = (double)A[j];
    for (int j = 0; j < sz(B); j++) bcmpl[j] = (double)B[j];
    FFT f; f.init(n);
    f.fft(acmpl, f);
    f.fft(bcmpl, f);
    vector<cmpl> rval(n);
    for (int j = 0; j < n; j++) {
        rval[j] = acmpl[j] * bcmpl[j];
    }
    f.ifft(rval, f);
    int need = sz(A) + sz(B) - 1;
    vl res(need);
    for (int j = 0; j < need; j++) {
        res[j] = llround(rval[j].real());
    }
    return res;
}
