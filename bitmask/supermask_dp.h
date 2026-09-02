#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/*
dp[mask] = sum over supersets f[sup] that include mask
mnemonic: if (mask lacks bit) dp[mask] += dp[mask | bit]
*/

/** Demonstrates superset-sum DP. Time: O(B 2^B). Space: O(2^B). */
inline void solve() {
    int B;
    vector<ll> dp(1<<B, 0);
    for (int i = 0; i < B; i++) {
        for (int mask = 0; mask < (1<<B); mask++) {
            if ((mask & (1<<i)) == 0) {         // if bit i if OFF
                dp[mask] += dp[mask | (1<<i)];  // add superset with bit i ON
            }
        }
    }
}
