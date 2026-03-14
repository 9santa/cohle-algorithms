#include "../header.h"
#include <complex>

using cmpl = complex<double>;
const double PI = atan2(0, -1);

inline vector<cmpl> fft(vector<cmpl> P) {
    size_t n = P.size();
    if (n == 1) return P;
    vector<cmpl> P_even(n/2), P_odd(n/2);
    for (int i = 0; i < n/2; i++) {
        P_even[i] = P[2 * i];
        P_odd[i] = P[2 * i + 1];
    }
    vector<cmpl> val_even = fft(P_even);
    vector<cmpl> val_odd = fft(P_odd);
    vector<cmpl> val_P(n);
    for (int i = 0; i < n/2; i++) {
        cmpl x{cos(2 * PI * i / n), sin(2 * PI * i / n)};
        val_P[i] = val_even[i] + x * val_odd[i];
        val_P[i + n/2] = val_even[i] - x * val_odd[i];
    }
    return val_P;
}

inline void fft_r(vector<cmpl>& a) {
    int n = sz(a);
    if (n == 1) return;
    vector<cmpl> even(n/2), odd(n/2);
    for (int j = 0; j < n/2; j++) {
        even[j] = a[2 * j];
        odd[j] = a[2 * j + 1];
    }
    fft_r(even), fft_r(odd);
    for (int i = 0; i < n/2; i++) {
        cmpl x{cos(2 * PI * i / n), sin(2 * PI * i / n)};
        a[i] = even[i] + x * odd[i];
        a[i + n/2] = even[i] - x * odd[i];
    }
}

inline vector<cmpl> ifft(vector<cmpl> val_P) {
    size_t n = val_P.size();
    if (n == 1) return val_P;
    vector<cmpl> val_even(n/2), val_odd(n/2);
    for (int i = n/2-1; i >= 0; i--) {
        cmpl x{cos(2 * PI * i / n), sin(2 * PI * i / n)};
        val_even[i] = (val_P[i] + val_P[i + n/2]) / 2.0;
        val_odd[i] = (val_P[i] - val_P[i + n/2]) / (2.0 * x);
    }
    vector<cmpl> P_even = ifft(val_even);
    vector<cmpl> P_odd = ifft(val_odd);
    vector<cmpl> P(n, 0);
    for (int i = n/2-1; i >= 0; i--) {
        P[2 * i] = P_even[i];
        P[2 * i + 1] = P_odd[i];
    }
    return P;
}
