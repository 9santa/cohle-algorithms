#pragma once
#include <bits/stdc++.h>
using namespace std;

optional<vector<int>> dag_toposort(const vector<vector<int>>& g) {
    int n = (int)g.size();
    vector<int> indeg(n);
    for (int v = 0; v < n; ++v) for (int to : g[v]) ++indeg[to];

    queue<int> q;
    for (int v = 0; v < n; ++v) if (indeg[v] == 0) q.push(v);

    vector<int> order;
    order.reserve(n);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        order.push_back(v);
        for (int to : g[v]) if (--indeg[to] == 0) q.push(to);
    }
    if ((int)order.size() != n) return nullopt;
    return order;
}

/** Counts non-empty directed paths ending at each vertex modulo MOD. */
vector<int> dag_paths_ending_at_mod(const vector<vector<int>>& g, int MOD) {
    auto topo = dag_toposort(g);
    if (!topo) return {};

    vector<int> dp(g.size(), 0);
    for (int v : *topo) {
        for (int to : g[v]) {
            long long add = (long long)dp[v] + 1;
            dp[to] = (dp[to] + add) % MOD;
        }
    }
    return dp;
}

/** Parity (mod 2) of the number of paths ending at each vertex. */
vector<int> dag_paths_ending_at_xor(const vector<vector<int>>& g) {
    auto topo = dag_toposort(g);
    if (!topo) return {};

    vector<int> dp(g.size(), 1); // includes the length-0 path at each vertex
    for (int v : *topo) {
        for (int to : g[v]) dp[to] ^= dp[v];
    }
    return dp;
}
