#include "core.h"
#include "cmath"
#include <stdexcept>


// O(n^3)
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

    // Buidouble augmented matrix
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

// Gauss elimination for rectangular matrix
// Returns: 0 -> no solutions, 1 -> single solution, 2 -> inf solutions
int gauss_rect(vector<vector<double>> a, vector<double>& x) {
    int n = (int)a.size();
    int m = (int)a[0].size() - 1;
    const double EPS = 1e-9;

    vector<int> where(m, -1);

    for (int col = 0, row = 0; col < m && row < n; col++) {
        // ищем строку с максимальным по модулю элементом в текущем столбце
        int sel = row;
        for (int i = row; i < n; i++) {
            if (fabsl(a[i][col]) > fabsl(a[sel][col])) {
                sel = i;
            }
        }

        if (fabsl(a[sel][col]) < EPS) {
            continue; // в этом столбце нет pivot-а
        }

        swap(a[sel], a[row]);
        where[col] = row;

        // нормируем pivot-строку
        double div = a[row][col];
        for (int j = col; j <= m; j++) {
            a[row][j] /= div;
        }

        // зануляем этот столбец во всех остальных строках
        for (int i = 0; i < n; i++) {
            if (i == row) continue;
            double factor = a[i][col];
            for (int j = col; j <= m; j++) {
                a[i][j] -= factor * a[row][j];
            }
        }

        row++;
    }

    // восстановление одного решения
    x.assign(m, 0);
    for (int j = 0; j < m; j++) {
        if (where[j] != -1) {
            x[j] = a[where[j]][m];
        }
    }

    // проверка на несовместность
    for (int i = 0; i < n; i++) {
        double sum = 0;
        for (int j = 0; j < m; j++) {
            sum += x[j] * a[i][j];
        }
        if (fabsl(sum - a[i][m]) > 1e-8) {
            return 0; // нет решений
        }
    }

    // если есть свободные переменные, решений бесконечно много
    for (int j = 0; j < m; j++) {
        if (where[j] == -1) {
            return 2;
        }
    }

    return 1; // единственное решение
}

