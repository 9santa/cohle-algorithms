
/*
dp[mask] = sum over supersets f[sup] that include mask
mnemonic: if (mask lacks bit) dp[mask] += dp[mask | bit]
*/

static void solve() {
    int n;
    vector<ll> dp(1<<n, 0);
    for (int i = 0; i < n; i++) {
        for (int mask = 0; mask < (1<<n); mask++) {
            if ((mask & (1<<i)) == 0) {         // if bit i if OFF
                dp[mask] += dp[mask | (1<<i)];  // add superset with bit i ON
            }
        }
    }
}

