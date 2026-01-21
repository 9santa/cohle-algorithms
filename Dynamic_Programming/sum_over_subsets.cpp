#include "../header.h"

/*
f[mask]: array of all masks 0..(2^n - 1)
We want to compute: for each mask, sum f[sub] over all submasks of mask
dp[i][mask] = sum of f[sub] where sub is a submask of mask, and sub differs only in first i bits
Common use-cases: Subset sum over bitmasks, Inclusion–exclusion optimization, DP on subsets,
Graph problems (cliques, independent sets), Frequency convolution over subsets
*/

static void solve() {
    int n; cin >> n;
    vector<int> a(n); cin >> a;
    int N = 1 << n;
    vector<ll> dp(N);

    // input f[mask]
    for (auto x : a) dp[x]++;

    // SOS DP
    for (int i = 0; i < n; i++) {
        for (int mask = 0; mask < N; mask++) {
            if (mask & (1 << i)) {
                dp[mask] += dp[mask ^ (1 << i)];
            }
        }
    }

    // dp[mask] = sum of f[sub] for all submasks of mask
}
