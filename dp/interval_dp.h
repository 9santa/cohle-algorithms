#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
template<class T> using V = vector<T>;
using vl = vector<long long>;
#define sz(x) int((x).size())
#ifndef COHLE_INFINITY_HELPER
#define COHLE_INFINITY_HELPER
template<class T> constexpr T infty = 0;
template<> constexpr int infty<int> = 1'010'000'000;
template<> constexpr long long infty<long long> = 2'020'000'000'000'000'000;
template<> constexpr double infty<double> = numeric_limits<double>::infinity();
template<> constexpr long double infty<long double> = numeric_limits<long double>::infinity();
#endif
template<class T> void print(const T& x) { cout << x << "\n"; }
template<class T, class... U> void print(const T& x, const U&... y) { cout << x << " "; print(y...); }

/* answer depends on a contiguous segment [l, r], often with a split point k
   build small segments first, then larger ones
   state: dp[l][r] = best to solve interval [l, r]
   trans: dp[l][r] = min/max/... over k in [l, r-1] of dp[l][k] + dp[k+1][r] + func(l, r) */

// https://atcoder.jp/contests/dp/tasks/dp_n
/** Solves interval merge DP for slimes/stones. Time: O(n^3). Space: O(n^2). */
ll solve(vl& a) {
    int n = sz(a);
    V<vl> dp(n, vl(n, infty<ll>));
    for (int i = 0; i < n; i++) dp[i][i] = 0;
    vl pref(n, 0);
    pref[0] = a[0];
    for (int i = 1; i < n; i++) {
        pref[i] = pref[i-1] + a[i];
    }

    for (int len = 2; len <= n; len++) {
        for (int l = 0; l < n-len+1; l++) {
            int r = l + len - 1;
            ll cost = pref[r] - (l ? pref[l-1] : 0ll);
            for (int split = l; split < r; split++) {
                dp[l][r] = min(dp[l][r], dp[l][split] + dp[split+1][r] + cost);
            }
        }
    }

    print(dp[0][n-1]);
}
