#include "../header.h"
#include <complex>

using cmpl = complex<double>;
const double PI = atan2(0, -1);

/*
    len = current recursion n
    half = n / 2
    inner j is rec i
*/
inline void fft_iter(vector<cmpl>& a) {
    int n = sz(a); // assumes n is power of two
    // 1) bit reversal permutation
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;                       // start at the highest bit
        for (; j & bit; bit >>= 1) j ^= bit;    // while this bit is 1
        j ^= bit;                               // flip it to 0 and carry to next bit
        if (i < j) swap(a[i], a[j]);    // flip first 0-bit to 1 (stop carry)
    }

    // 2) butterflies
    for (int len = 2; len <= n; len <<= 1) {
        int half = len >> 1;
        for (int i = 0; i < n; i += len) {
            for (int j = 0; j < half; j++) {
                double ang = 2 * PI * j / len;
                cmpl w{cos(ang), sin(ang)};
                cmpl u = a[i + j];
                cmpl v = a[i + j + half] * w;
                a[i + j] = u + v;
                a[i + j + half] = u - v;
            }
        }
    }
}

struct FFT {
    int n = 0;
    vi rev;
    vector<cmpl> roots; // roots[i] = cos(2pi*i/n) + im*sin(2pi*i/n)

    void init(int N) {
        n = N;
        rev.assign(n, 0);
        int lg = __builtin_ctz(n);
        for (int i = 0; i < n; i++) {
            rev[i] = 0;
            for (int b = 0; b < lg; b++) {
                if (i & (1<<b)) rev[i] |= 1 << (lg - 1 - b);
            }
        }
        roots.resize(n);
        for (int i = 0; i < n; i++) {
            double ang = 2 * PI * i / n;
            roots[i] = cmpl(cos(ang), sin(ang));
        }
    }

    inline void fft(vector<cmpl>& a, const FFT& C) {
        int n = sz(a);
        for (int i = 0; i < n; i++) if (i < C.rev[i]) swap(a[i], a[C.rev[i]]);

        for (int len = 2; len <= n; len <<= 1) {
            int half = len >> 1;
            int step = n / len;
            for (int i = 0; i < n; i += len) {
                for (int j = 0; j < half; j++) {
                    cmpl w = C.roots[j * step];
                    cmpl u = a[i + j];
                    cmpl v = a[i + j + half] * w;
                    a[i + j] = u + v;
                    a[i + j + half] = u - v;
                }
            }
        }
    }

    inline void ifft(vector<cmpl>& a, const FFT& C) {
        int n = sz(a);
        for (auto& z : a) z = std::conj(z);
        fft(a, C);
        for (auto& z : a) z = std::conj(z) / (double)n;
    }
};

