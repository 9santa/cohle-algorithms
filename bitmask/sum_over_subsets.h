#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/*
f[mask]: array of all masks 0..(2^n - 1)
We want to compute: for each mask, sum f[sub] over all submasks of mask
dp[i][mask] = sum of f[sub] where sub is a submask of mask, and sub differs only in first i bits
Common use-cases: Subset sum over bitmasks, Inclusion–exclusion optimization, DP on subsets,
Graph problems (cliques, independent sets), Frequency convolution over subsets
*/

/** Demonstrates SOS DP over submasks. Time: O(B 2^B). Space: O(2^B). */
static void solve() {
    int n; cin >> n;
    vector<int> a(n);
    for (int& x : a) cin >> x;
    int B = 0;
    for (auto x : a) B = max(B, 32 - __builtin_clz(x));
    int N = 1 << B;
    vector<ll> dp(N);

    // input f[mask]
    for (auto x : a) dp[x]++;  // frequency version
    // for (auto x : a) dp[x] += x; // sum of values version
    // for (auto x : a) dp[x] += weight(x); // any weight version

    // SOS DP
    for (int i = 0; i < B; i++) {
        for (int mask = 0; mask < N; mask++) {
            if (mask & (1 << i)) {
                dp[mask] += dp[mask ^ (1 << i)];
            }
        }
    }

    // dp[mask] = sum of f[sub] for all submasks of mask
}
