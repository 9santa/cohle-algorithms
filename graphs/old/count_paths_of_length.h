#include "../Math/Matrix.h"

ll solve(const vector<vector<int>>& g, int k) {
    int n = (int)g.size();
    Matrix A(n, vector<Z>(n, 0));
    for (int i = 0; i < n; i++) {
        for (auto v : g[i]) A[i][v] = 1;
    }

    Matrix Ak = mpow(A, k);

    Z ans = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ans += Ak[i][j];
        }
    }

    return ans.val();
}
