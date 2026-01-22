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

// Longest Common String Subsequnce + reconstruction
string solve(const string& s, const string& t) {
    int n = sz(s), m = sz(t);
    V<vi> dp(n+1, vi(m+1));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (s[i] == t[j]) dp[i+1][j+1] = dp[i][j] + 1;
            else dp[i+1][j+1] = max(dp[i][j+1], dp[i+1][j]);
        }
    }

    string res;
    int i = n, j = m;
    while (i > 0 && j > 0) {
        if (s[i-1] == t[j-1]) {
            res.pb(s[i-1]);
            i--; j--;
        } else if (dp[i-1][j] >= dp[i][j-1]) {
            i--;
        } else j--;
    }

    reverse(all(res));
    return res;
}
