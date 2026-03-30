#include "core.h"
#include "cmath"
#include <stdexcept>


bool gauss_jordan_elimination(vector<vector<double>>& a, vector<double>& x) {
    int n = (int)a.size();      // # rows
    int m = (int)a[0].size();   // # cols
    const double EPS = 1e-9;

    vector<int> where(m, -1);

    for (int col = 0, row = 0; col < m && row < n; col++) {
        // 1. choose row with max absolute value in current column
        int sel = row;
        for (int i = row; i < n; i++) {
            if (fabs(a[i][col]) > fabs(a[sel][col])) sel = i;
        }

        // if the whole column is zero -> go to the next column
        if (fabs(a[sel][col]) < EPS) continue;

        // 2. swap found row with current to the top
        swap(a[sel], a[row]);
        where[col] = row;

        // 3. zero-ing elements below and above
        for (int i = 0; i < n; i++) {
            if (i != row) {
                double factor = a[i][col] / a[row][col];
                for (int j = col; j <= m; j++) {
                    a[i][j] -= factor * a[row][j];
                }
            }
        }

        row++;
    }

    // 4. reconstruct answer
    x.assign(m, 0);
    for (int i = 0; i < m; i++) {
        if (where[i] != -1) {
            x[i] = a[where[i]][m] / a[where[i]][i];
        }
    }

    // optional correctness check
    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        for (int j = 0; j < m; j++) {
            sum += x[j] * a[i][j];
        }
        if (fabs(sum - a[i][m]) > 1e-7) return false;
    }

    return true;
}

// Gauss-Jordan with partial pivoting
vector<double> gaussJordanElimination(vector<vector<double>> A, vector<double> b) {
    int n = A.size();
    const double EPS = 1e-12;

    // Build augmented matrix [A | b]
    vector<vector<double>> aug(n, vector<double>(n + 1));
    for (int i = 0; i < n; ++i) {
        if ((int)A[i].size() != n) {
            throw invalid_argument("Matrix A must be square.");
        }
        for (int j = 0; j < n; ++j) {
            aug[i][j] = A[i][j];
        }
        aug[i][n] = b[i];
    }

    // Gauss-Jordan elimination with partial pivoting
    for (int col = 0; col < n; ++col) {
        // Find pivot row
        int pivotRow = col;
        for (int row = col + 1; row < n; ++row) {
            if (fabs(aug[row][col]) > fabs(aug[pivotRow][col])) {
                pivotRow = row;
            }
        }

        if (fabs(aug[pivotRow][col]) < EPS) {
            throw runtime_error("System has no unique solution.");
        }

        swap(aug[col], aug[pivotRow]);

        // Normalize pivot row so pivot becomes 1
        double pivot = aug[col][col];
        for (int j = 0; j <= n; ++j) {
            aug[col][j] /= pivot;
        }

        // Eliminate this column in all other rows
        for (int row = 0; row < n; ++row) {
            if (row == col) continue;

            double factor = aug[row][col];
            for (int j = 0; j <= n; ++j) {
                aug[row][j] -= factor * aug[col][j];
            }
        }
    }

    // Now left side should be identity, solution is last column
    vector<double> x(n);
    for (int i = 0; i < n; ++i) {
        x[i] = aug[i][n];
    }

    return x;
}
