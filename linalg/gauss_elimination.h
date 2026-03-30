#include "core.h"
#include "cmath"
#include <stdexcept>


bool gaussian_elimination(vector<vector<double>>& a, vector<double>& x) {
    int n = (int)a.size();    // # rows
    const double EPS = 1e-9;

    // forward pass
    for (int k = 0; k < n; k++) {
        // 1. choose pivot element
        int pivot = k;
        for (int i = k+1; i < n; i++) {
            if (fabs(a[i][k]) > fabs(a[pivot][k])) {
                pivot = i;
            }
        }

        if (fabs(a[pivot][k]) < EPS) return false;

        // 2. swap pivot row to the top
        if (pivot != k) {
            swap(a[pivot], a[k]);
        }

        // 3. zero-ing elements below
        for (int i = k+1; i < n; i++) {
            // want to make: a[i][k] = 0
            // factor - how many times we need to subtract row k from row i
            double factor = a[i][k] / a[k][k];
            a[i][k] = 0.0;

            // update the whole row
            for (int j = k+1; j <= n; j++) {
                a[i][j] -= factor * a[k][j];
            }
        }
    }

    // backward pass
    x.assign(n, 0.0);
    for (int i = n-1; i >= 0; i--) {
        double sum = a[i][n];
        for (int j = i+1; j < n; j++) {
            sum -= a[i][j] * x[j];
        }
        x[i] = sum / a[i][i];
    }

    return true;
}

// Gauss with partial pivoting
vector<double> gaussianElimination(vector<vector<double>> A, vector<double> b) {
    int n = A.size();
    const double EPS = 1e-12;

    // Build augmented matrix
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

    // Forward elimination with partial pivoting
    for (int col = 0; col < n; ++col) {
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

        for (int row = col + 1; row < n; ++row) {
            double factor = aug[row][col] / aug[col][col];
            for (int j = col; j <= n; ++j) {
                aug[row][j] -= factor * aug[col][j];
            }
        }
    }

    // Back substitution
    vector<double> x(n);
    for (int i = n - 1; i >= 0; --i) {
        double sum = aug[i][n];
        for (int j = i + 1; j < n; ++j) {
            sum -= aug[i][j] * x[j];
        }

        if (fabs(aug[i][i]) < EPS) {
            throw runtime_error("System has no unique solution.");
        }

        x[i] = sum / aug[i][i];
    }

    return x;
}
