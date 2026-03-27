#pragma once
#include "../header.h"

/* the graph is a tree and each subtree is a natural subproblem
   state: dp[u][0] = ans in subtree of u if u is not chosen
          dp[u][1] = ans in subtree of u if u is chosen
   trans: dp[u][1] *= dp[v][0]
          dp[u][0] *= (dp[v][0] + dp[v][1])
   many tree DPs are just this with a larger/more complex local state */

// https://atcoder.jp/contests/dp/tasks/dp_p

const ll MOD = 1e9 + 7;

vector<vector<int>> g;
vector<array<ll, 2>> dp;

void dfs(int u, int p) {
    dp[u][0] = 1; // u not chosen
    dp[u][1] = 1; // u chosen

    for (auto v : g[u]) {
        if (v == p) continue;
        dfs(v, u);

        ll notTake = dp[u][0] * ((dp[v][0] + dp[v][1]) % MOD) % MOD;
        ll take = dp[u][1] * dp[v][0] % MOD;

        dp[u][0] = notTake;
        dp[u][1] = take;
    }
}
