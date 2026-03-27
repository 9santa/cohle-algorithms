#pragma once
#include "core.h"

/* naive transition is "some invertible func over a range of previous states"
   shape: ndp[i] = sum(dp[j]) for j in [L(i), R(i)]
   optimized with pref sums: pref[k+1] = pref[k] + dp[k]
                             rangeSum(L, R) = pref[R+1] - pref[L]
   many hard DPs are just: "replace inner loop with prefix/range query" */

constexpr int MOD = 1e9 + 7;

vector<int> rangeTransitionDP(const vector<int>& dp, const vector<pair<int, int>>& ranges) {
    int n = sz(dp);
    vector<int> pref(n+1, 0);
    vector<int> ndp(n, 0);

    for (int i = 0; i < n; i++) {
        pref[i+1] = pref[i] + dp[i];
        if (pref[i+1] >= MOD) pref[i+1] -= MOD;
    }

    auto getSum = [&](int l, int r) -> int {
        if (l > r) return 0;
        int ans = pref[r+1] - pref[l];
        if (ans < 0) ans += MOD;
        return ans;
    };

    for (int i = 0; i < n; i++) {
        auto [l, r] = ranges[i];
        ndp[i] = getSum(l, r);
    }

    return ndp;
}
