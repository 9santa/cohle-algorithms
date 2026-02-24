// #include "fft.h"
#include "convolution_fft.h"


inline vi mul_bool(const vi& A, const vi& B, int S) {
    auto C = convolution(A, B);
    if (sz(C) > S+1) C.resize(S+1);
    for (auto& x : C) x = (x != 0);
    return C;
}

inline vi self_conv(vi base, ll b, ll S) {
    vi res(1, 1);
    while (b) {
        if (b&1) res = mul_bool(res, base, S);
        b >>= 1;
        if (b) base = mul_bool(base, base, S);
    }
    if (sz(res) > S+1) res.resize(S+1);
    for (auto& x : res) x = (x != 0);
    return res;
}
