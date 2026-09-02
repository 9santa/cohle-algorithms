#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Fixed-capacity, runtime-dimension matrix.
Storage is MAXN*MAXN no heap allocs. Flat 1D array
Actual active dimension is n (<= MAXN)
Multiplication loops only over [0..n) */
template<int MAXN, class ModInt>
struct MatDyn {
    using Z = ModInt;
    int n = 0;  // active dimension
    array<Z, MAXN*MAXN> a{}; // row-major storage
    MatDyn() = default;
    explicit MatDyn(int n_) : n(n_) {}

    // 2D to 1D Access through () operator
    inline Z& operator()(int i, int j) { return a[i * MAXN + j]; }
    inline const Z& operator()(int i, int j) const { return a[i * MAXN + j]; }

    /** Returns the n x n identity matrix. Time: O(n). */
    static MatDyn identity(int n) {
        MatDyn m(n);
        for (int i = 0; i < n; i++) m(i, i) = 1;
        return m;
    }

    /** Multiplies two active n x n matrices. Time: O(n^3). */
    friend MatDyn operator*(const MatDyn& A, const MatDyn& B) {
        const int n = A.n;
        MatDyn C(n);
        for (int i = 0; i < n; i++) {
            for (int k = 0; k < n; k++) {
                Z aik = A(i, k);
                if (aik.val() == 0) continue;
                for (int j = 0; j < n; j++) {
                    C.a[i * MAXN + j] += aik * B.a[k * MAXN + j];
                }
            }
        }
        return C;
    }
};


/** Matrix multiplication monoid for dynamic matrices. Space: O(1). */
template<int MAXN, class ModInt>
struct Monoid_MatDyn {
    using value_type = MatDyn<MAXN, ModInt>;
    using X = value_type;
    int n = 0;
    Monoid_MatDyn() = default;
    explicit Monoid_MatDyn(int n_) : n(n_) {}
    /** Multiplies two matrices. Time: O(n^3). */
    static X op(const X& x, const X& y) { return x * y; }
    /** Returns the identity matrix for the active dimension. Time: O(n). */
    X id() const { return X::identity(n); }
};
