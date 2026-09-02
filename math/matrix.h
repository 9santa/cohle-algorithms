#pragma once
#include <bits/stdc++.h>
using namespace std;

template<class T>
using Matrix = vector<vector<T>>;

template<class T>
Matrix<T> matrix_multiply(const Matrix<T>& a, const Matrix<T>& b) {
    const int n = (int)a.size();
    const int p = (int)b.size();
    const int m = p ? (int)b[0].size() : 0;
    Matrix<T> c(n, vector<T>(m, T(0)));
    for (int i = 0; i < n; ++i)
        for (int k = 0; k < p; ++k)
            for (int j = 0; j < m; ++j)
                c[i][j] += a[i][k] * b[k][j];
    return c;
}

template<class T>
Matrix<T> matrix_multiply_square(const Matrix<T>& a, const Matrix<T>& b) {
    const int n = (int)a.size();
    Matrix<T> c(n, vector<T>(n, T(0)));
    for (int i = 0; i < n; ++i)
        for (int k = 0; k < n; ++k) {
            if (a[i][k] == T(0)) continue;
            for (int j = 0; j < n; ++j) c[i][j] += a[i][k] * b[k][j];
        }
    return c;
}

template<class T, class E>
Matrix<T> matrix_power(Matrix<T> a, E e) {
    const int n = (int)a.size();
    Matrix<T> res(n, vector<T>(n, T(0)));
    for (int i = 0; i < n; ++i) res[i][i] = T(1);
    while (e > 0) {
        if (e & 1) res = matrix_multiply_square(res, a);
        a = matrix_multiply_square(a, a);
        e >>= 1;
    }
    return res;
}
