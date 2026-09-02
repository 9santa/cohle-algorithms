#pragma once
#include <bits/stdc++.h>
using namespace std;

using u64 = unsigned long long;
using ll = long long;

using cmpl = complex<long double>;
const long double PI = acosl(-1.0);

void fft(vector<cmpl>& a, bool invert) {
    int n = (int)a.size();

    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }

    for (int len = 2; len <= n; len <<= 1) {
        int half = len >> 1;
        long double ang = 2 * PI / len * (invert ? -1 : 1);
        cmpl wlen{cos(ang), sin(ang)};
        // i loop merges each len-sized block
        // [ u | v ] -> [ u + v | u - v ]
        for (int i = 0; i < n; i += len) {
            cmpl w{1, 0}; // w = 1 = w^0

            // j is the power w^j
            for (int j = 0; j < half; j++) {
                cmpl u = a[i + j]; // Aeven(w^2j)
                cmpl v = a[i + j + half] * w; // w^j * Aodd(w^2j)
                a[i + j] = u + v;
                a[i + j + half] = u - v;
                w *= wlen;
            }
        }
    }

    if (invert) {
        for (cmpl& x : a) {
            x /= n;
        }
    }
}

vector<ll> convolution_fft(vector<ll> a, vector<ll> b) {
    if (a.empty() || b.empty()) return {};

    int need = (int)a.size() + (int)b.size() - 1;
    int n = 1;
    while (n < need) n <<= 1;

    vector<cmpl> fa(a.begin(), a.end());
    vector<cmpl> fb(b.begin(), b.end());
    fa.resize(n);
    fb.resize(n);

    fft(fa, false);
    fft(fb, false);

    for (int i = 0; i < n; i++) {
        fa[i] *= fb[i];
    }

    fft(fa, true);

    vector<ll> res(need);
    for (int i = 0; i < need; i++) {
        res[i] = llroundl(fa[i].real());
    }

    return res;
}

vector<u64> convolution_mod_2_64(const vector<u64>& a, const vector<u64>& b) {
    if (a.empty() || b.empty()) return {};

    int n = (int)a.size();
    int m = (int)b.size();
    int need = n + m - 1;

    constexpr int BASE_BITS = 15;
    constexpr int K = 5;
    constexpr u64 MASK = (1ULL << BASE_BITS) - 1;

    vector<ll> A[K], B[K];
    for (int t = 0; t < K; t++) {
        A[t].resize(n);
        B[t].resize(m);
    }

    for (int i = 0; i < n; i++) {
        for (int t = 0; t < K; t++) {
            A[t][i] = (a[i] >> (BASE_BITS * t)) & MASK;
        }
    }

    for (int i = 0; i < m; i++) {
        for (int t = 0; t < K; t++) {
            B[t][i] = (b[i] >> (BASE_BITS * t)) & MASK;
        }
    }

    vector<ll> level[K];

    for (int t = 0; t < K; t++) {
        level[t].assign(need, 0);
    }

    for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
            if (i + j >= K) continue;

            vector<ll> cur = convolution_fft(A[i], B[j]);

            for (int k = 0; k < need; k++) {
                level[i + j][k] += cur[k];
            }
        }
    }

    vector<u64> res(need);

    for (int k = 0; k < need; k++) {
        u64 x = 0;

        for (int t = 0; t < K; t++) {
            x += (u64)level[t][k] << (BASE_BITS * t);
        }

        res[k] = x;
    }

    return res;
}

void solve(void) {
    int N, M; cin >> N >> M;
    vector<u64> a(N), b(M);
    for (int i = 0; i < N; i++) cin >> a[i];
    for (int i = 0; i < M; i++) cin >> b[i];

    auto res = convolution_mod_2_64(a, b);

    for (auto& x : res) cout << x << " ";

}
