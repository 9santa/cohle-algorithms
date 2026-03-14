#include "fft_recursive.h"

inline vector<ll> convolution(const vector<int>& P, const vector<int>& Q) {
    int n = 1;
    while (n <= sz(P) + sz(Q) - 1) n *= 2;
    vector<cmpl> P_cmpl(n, 0);
    vector<cmpl> Q_cmpl(n, 0);
    for (int j = 0; j < n; j++) P_cmpl[j] = (double)P[j];
    for (int j = 0; j < n; j++) Q_cmpl[j] = (double)Q[j];
    auto P_val = fft(P_cmpl);
    auto Q_val = fft(Q_cmpl);
    vector<cmpl> R_val(n);
    for (int i = 0; i < n; i++) {
        R_val[i] = P_val[i] * Q_val[i];
    }
    auto R_cmpl = ifft(R_val);
    vector<ll> R(n);
    for (int i = 0; i < n; i++) {
        R[i] = round(R_cmpl[i].real());
    }
    return R;
}
