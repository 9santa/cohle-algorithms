#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
template<class T> using V = vector<T>;
using vi = vector<int>;
using vl = vector<long long>;
#ifndef COHLE_INFINITY_HELPER
#define COHLE_INFINITY_HELPER
template<class T> constexpr T infty = 0;
template<> constexpr int infty<int> = 1'010'000'000;
template<> constexpr long long infty<long long> = 2'020'000'000'000'000'000;
template<> constexpr double infty<double> = numeric_limits<double>::infinity();
template<> constexpr long double infty<long double> = numeric_limits<long double>::infinity();
#endif

// Нужна монотонность точки разреза на префиксе (dp на префиксе)
// Time: O(N * K * log N)


/* partition first i items into g groups, with transition: dp[g][i] = min over j < i of dp[g-1][j] + cost(j+1, i)
   and the optimal split point is monotone */

namespace {

vector<ll> prevdp;
vector<ll> curdp;

// Define for your problem
// j is split point, computing segment (j+1 ... i)
/** Problem-specific segment cost. Time: depends on implementation. */
ll cost(int j, int i);

/** Computes one divide-and-conquer DP layer on [l, r]. Time: O((r-l+1) * opt range). */
void compute(int l, int r, int optL, int optR) {
    if (l > r) return;
    int mid = (l+1) >> 1;

    pair<ll, int> best = {infty<ll>, -1};
    for (int j = optL; j <= min(mid-1, optR); j++) {
        ll cand = prevdp[j] + cost(j, mid);
        if (cand < best.first) best = {cand, j};
    }

    curdp[mid] = best.first;
    int opt = best.second;
    compute(l, mid-1, optL, opt);
    compute(mid+1, r, opt, optR);

    // Typical outer loop (kept outside this helper):
    // for (int group = 1; group <= K; ++group) {
    //     compute(1, n, 0, n - 1);
    //     swap(prevdp, curdp);
    // }
}

} // namespace


namespace not_optimized_mem {   // O(N * K) memory (full dp table)
constexpr int MAXN = 100005;
int N, K; // N - number of elements, K - number of groups or partitions
ll pref[MAXN];
V<vl> dp;
V<vi> opt;

/** Returns squared segment-sum cost for [l, r]. Time: O(1). */
ll cost(int l, int r) {
    ll s = pref[r] - pref[l-1];
    return s * s;
}

/** Computes DP layer k on [l, r]. Time: O((r-l+1) * opt range). */
void compute(int k, int l, int r, int optL, int optR) {
    if (l > r) return;

    int mid = l + (r-l)/2;
    dp[k][mid] = infty<ll>;
    opt[k][mid] = -1;

    for (int j = optL; j <= min(mid-1, optR); j++) {
        ll val = dp[k-1][j] + cost(j+1, mid);
        if (val < dp[k][mid]) {
            dp[k][mid] = val;
            opt[k][mid] = j;
        }
    }

    int best = opt[k][mid];

    compute(k, l, mid-1, optL, best);
    compute(k, mid+1, r, best, optR);
}

/** Solves the full-table divide-and-conquer DP example. Time: O(KN log N). */
ll solve() {
    dp.assign(K+1, vector<ll>(N+1, infty<ll>));
    opt.assign(K+1, vector<int>(N+1, -1));

    for (int i = 1; i <= N; i++) {
        cin >> pref[i];
        pref[i] += pref[i-1];
    }

    // base case
    dp[0][0] = 0;
    for (int i = 1; i <= N; i++) dp[0][i] = infty<ll>; // can't partition positive length into 0 groups

    // k = 1
    for (int i = 1; i <= N; i++) {
        dp[1][i] = cost(1, i);
        opt[1][i] = 0;
    }

    // k >= 2
    for (int k = 2; k <= K; k++) {
        compute(k, 1, N, 0, N-1);
    }

    return dp[K][N];

}
} // namespace not_optimized_mem

namespace optimized_mem {   // O(N) memory (only prev and curr dp)
constexpr int MAXN = 100005;
int N, K;
ll pref[MAXN];
vector<ll> dp_prev, dp_curr;

/** Returns squared segment-sum cost for (l, r]. Time: O(1). */
ll cost(int l, int r) { // [l+1, r]
    ll s = pref[r] - pref[l];
    return s * s;
}

/** Computes one optimized-memory DP layer. Time: O((r-l+1) * opt range). */
void compute(int l, int r, int optL, int optR) {
    if (l > r) return;
    int mid = l + (r-l)/2;
    pair<ll, int> best = {infty<ll>, -1};

    for (int j = optL; j <= min(mid-1, optR); j++) {
        ll val = dp_prev[j] + cost(j, mid);
        if (val < best.first) best = {val, j};
    }

    dp_curr[mid] = best.first;
    int opt = best.second;
    compute(l, mid-1, optL, opt);
    compute(mid+1, r, opt, optR);
}

/** Solves the optimized-memory divide-and-conquer DP example. Time: O(KN log N). */
ll solve() {
    cin >> N >> K;
    pref[0] = 0;
    for (int i = 1; i <= N; i++) {
        cin >> pref[i];
        pref[i] += pref[i-1];
    }
    dp_prev.assign(N+1, infty<ll>);
    dp_curr.assign(N+1, infty<ll>);

    // base case: k = 1 (1 group)
    dp_prev[0] = 0;
    for (int i = 1; i <= N; i++) dp_prev[i] = cost(0, i);

    for (int k = 2; k <= K; k++) {
        fill(dp_curr.begin(), dp_curr.end(), infty<ll>);
        compute(1, N, 0, N-1);
        dp_prev.swap(dp_curr);
    }

    return dp_prev[N];
}
} // namespace optimized_mem
