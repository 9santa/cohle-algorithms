#pragma once
#include "ntt.h"

namespace NTT {

vector<Z> convolution(const vector<Z>& A, const vector<Z>& B) {
    if (A.empty() || B.empty()) return {};

    int need = sz(A) + sz(B) - 1;
    int n = 1;
    while (n < need) n <<= 1;

    vector<Z> FA(n, Z(0)), FB(n, Z(0));
    for (int i = 0; i < sz(A); i++) FA[i] = A[i];
    for (int i = 0; i < sz(B); i++) FB[i] = B[i];

    ntt(FA);
    ntt(FB);
    for (int i = 0; i < n; i++) FA[i] *= FB[i];
    intt(FA);

    FA.resize(need);
    return FA;
}

}; // namespace NTT
