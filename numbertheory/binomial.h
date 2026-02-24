
ll binomial(ll n, ll k) {
    vector<vector<ll>> dp(n+1, vector<ll>(k+1, 0));

    for (ll i = 0; i <= n; i++) {
        for (ll j = 0; j <= min(i, k); j++) {
            // C(n, 0) = C(n, n) = 1
            if (j == 0 || j == i) dp[i][j] = 1;
            // include or exclude current element
            else dp[i][j] = dp[i-1][j-1] + dp[i-1][j];
        }
    }

    return dp[n][k];
}
