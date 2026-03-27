#pragma once
#include "../header.h"

/* n is small, usually <= 20, and the state is a subset.
   state: dp[mask] = number of ways / best value for chosen subset mask
   trans: let i = popcount(mask) - next person/item index
          try adding one more available j
   Ex: dp[mask][last] for TSP / Hamilton path */

// https://atcoder.jp/contests/dp/tasks/dp_o

ll countMatchings(vector<vector<int>>& ok) {
    int n = sz(ok);
    vector<ll> dp(1 << n, 0);
    dp[0] = 1;

    for (int mask = 0; mask < (1 << n); mask++) {
        int i = __builtin_popcount((unsigned)mask);
        if (i >= n) continue;

        for (int j = 0; j < n; j++) {
            if ((mask >> j) & 1) continue;
            if (!ok[i][j]) continue;
            dp[mask | (1 << j)] += dp[mask];
        }
    }

    return dp[(1 << n) - 1];
}
