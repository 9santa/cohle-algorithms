#include "../header.h"

// You are given a DAG, count the paths ending at each node

constexpr int MOD = 1e9 + 7;
// g = 0-indexed adjacency list
vector<ll> solve(const vector<vector<int>>& g) {
    int n = (int)g.size();
    vector<ll> dp(n, 0);

    vector<ll> topo;
    topo.reserve(n);
    vector<bool> used(n, false);
    auto topsort = [&](auto&& self, int v) -> void {
        used[v] = true;
        for (auto u : g[v]) {
            if (!used[u]) {
                self(self, u);
            }
        }
        topo.pb(v);
    };

    for (int i = 0; i < n; i++) {
        if (!used[i]) topsort(topsort, i);
    }
    reverse(all(topo));

    for (auto v : topo) {
        for (auto u : g[v]) {
            dp[u] = (dp[u] + dp[v] + 1) % MOD;
        }
    }

    return dp;
}

// Same but count mod 2
vector<ll> xor_solve(const vector<vector<int>>& g) {
    int n = (int)g.size();
    vector<ll> dp(n, 1);

    vector<ll> topo;
    topo.reserve(n);
    vector<bool> used(n, false);
    auto topsort = [&](auto&& self, int v) -> void {
        used[v] = true;
        for (auto u : g[v]) {
            if (!used[u]) {
                self(self, u);
            }
        }
        topo.pb(v);
    };

    for (int i = 0; i < n; i++) {
        if (!used[i]) topsort(topsort, i);
    }
    reverse(all(topo));

    for (auto v : topo) {
        for (auto u : g[v]) {
            dp[u] ^= dp[v];
        }
    }

    return dp;
}
