#pragma once
#include "core.h"

namespace nt {

/**
 * Returns C(n, k) using dynamic programming.
 * @param n Total number of items.
 * @param k Number of chosen items.
 * Time: O(nk). Space: O(nk).
 */
inline ll binom_dp(int n, int k) {
    if (k < 0 || k > n) return 0;
    vector<vector<ll>> dp(n+1, vector<ll>(k+1, 0));
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= min(i, k); j++) {
            // C(n, 0) = C(n, n) = 1
            if (j == 0 || j == i) dp[i][j] = 1;
            // include or exclude current element
            else dp[i][j] = dp[i-1][j-1] + dp[i-1][j];
        }
    }
    return dp[n][k];
}

} // namespace nt
