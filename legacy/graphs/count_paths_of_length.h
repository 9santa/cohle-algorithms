#include "../Math/Matrix.h"

/*
Why A^2 counts walks of length 2
Take the adjacency matrix A.
A[i][j] means:
the number of ways to go from vertex i to vertex j in exactly 1 step.
So for length 2, we want to count all walks of the form
i -> t -> j
where t is the middle vertex.
Now look at the matrix product entry:
(A^2)[i][j] = sum over all t of A[i][t] * A[t][j]
What does one term mean?
A[i][t] = number of 1-step ways from i to t
A[t][j] = number of 1-step ways from t to j
So multiplying them gives the number of ways to go
i -> t -> j
using that specific middle vertex t.
*/

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
