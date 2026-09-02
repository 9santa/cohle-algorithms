#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using vi = vector<int>;
using vl = vector<long long>;
#define sz(x) int((x).size())

using cmpl = complex<double>;
const double PI = atan2(0, -1);

/*
    len = current recursion n
    half = n / 2
    inner j is recursive i
*/
/** In-place iterative FFT. Time: O(n log n). */
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
        double ang = 2 * PI / len;
        cmpl wlen{cos(ang), sin(ang)};
        for (int i = 0; i < n; i += len) {
            cmpl w{1, 0};

            for (int j = 0; j < half; j++) {
                cmpl u = a[i + j];
                cmpl v = a[i + j + half] * w;
                a[i + j] = u + v;
                a[i + j + half] = u - v;
                w *= wlen;
            }
        }
    }
}

/** Iterative FFT helper with precomputed roots. Space: O(n). */
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


/** Iterative FFT integer convolution. Time: O(n log n). */
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


/** Converts decimal string to base B chunks of width k. Time: O(|s|). */
vi to_base(const string& s, int B, int k) {
    vi a;
    for (int i = sz(s); i > 0; i -= k) {
        int start = max(0, i - k);
        string st = s.substr(start, i - start);
        a.push_back(stoi(st));
    }
    while (!a.empty() && a.back() == 0) a.pop_back();
    return a;
}

string to_decimal(const vl& a, int chunk, bool neg) {
    string res = (neg ? "-" : "");
    res += to_string(a.back());
    for (int i = sz(a)-2; i >= 0; i--) {
        string part = to_string(a[i]);
        res += string(chunk - sz(part), '0') + part;
    }
    return res;
}

string mul_bigint_fft(string s1, string s2) {
    bool neg = false;
    if (!s1.empty() && s1.front() == '-') { neg = !neg; s1.erase(s1.begin()); }
    if (!s2.empty() && s2.front() == '-') { neg = !neg; s2.erase(s2.begin()); }
    auto is_all_zero = [](const string& s) {
        for (char c : s) if (c != '0') return false;
        return true;
    };
    if (is_all_zero(s1) || is_all_zero(s2)) return "0";

    const int base = 1e4;
    const int k = 4;
    auto a = to_base(s1, base, k);
    auto b = to_base(s2, base, k);

    int need = sz(a) + sz(b) - 1;
    int n = 1;
    while (n < need) n <<= 1;
    FFT cache; cache.init(n);
    auto c = convolution(a, b);

    ll carry = 0;
    for (int i = 0; i < sz(c); i++) {
        ll cur = c[i] + carry;
        c[i] = cur % base;
        carry = cur / base;
    }
    while (carry) {
        c.push_back(carry % base);
        carry /= base;
    }
    while (sz(c) > 1 && c.back() == 0) c.pop_back();

    auto res = to_decimal(c, k, neg);
    return res;
}
