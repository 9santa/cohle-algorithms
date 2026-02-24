#include "ModInt.h"

using Matrix = vector<vector<Z>>;

Matrix multiply(const Matrix& a, const Matrix& b) {
    int n = sz(a);
    int m = sz(b[0]);
    int p = sz(b);
    Matrix c(n, vector<Z>(m, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < p; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return c;
}

Matrix fastmul(const Matrix& a, const Matrix& b) {
    int n = sz(a);
    Matrix c(n, vector<Z>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            if (a[i][k].val() == 0) continue;
            Z aik = a[i][k];
            for (int j = 0; j < n; j++) {
                c[i][j] += aik * b[k][j];
            }
        }
    }
    return c;
}

// Matrix Binary Exponentiation
Matrix mpow(Matrix mat, ll power) {
    int n = (int)mat.size();
    Matrix res(n, vector<Z>(n, 0));
    for (int i = 0; i < n; i++) res[i][i] = 1; // identity
    while (power > 0) {
        if (power & 1LL) res = fastmul(res, mat);
        mat = fastmul(mat, mat);
        power >>= 1;
    }
    return res;
}


// Test
signed main(void) {
    int n, m, k; re(n, m, k);
    Matrix a(n, vector<Z>(m, 0));
    Matrix b(m, vector<Z>(k, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> a[i][j];
        }
    }
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            cin >> b[i][j];
        }
    }

    auto res = multiply(a, b);
    for (int i = 0; i < sz(res); i++) {
        for (int j = 0; j < sz(res[0]); j++) {
            cout << res[i][j] << " ";
        }
        cout << "\n";
    }

    return 0;
}
