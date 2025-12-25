#include "../header.h"

int LCS(const vector<int>& a, const vector<int>& b) {
    int n = sz(a), m = sz(b);
    V<vi> dp(n+1, vi(m+1, 0));
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
            if (a[i-1] == b[i-1]) dp[i][j] = max(dp[i][j], dp[i-1][j-1] + 1);
        }
    }
    return dp[n][m];
}
