#include "../base.h"

template<class Cost>
vi dag_paths_ending_at_mod(const Graph<Cost, true>& G, int MOD) {
    auto topo_opt = toposort_kahn(G);
    if (!topo_opt) return {}; // not a DAG
    const auto& topo = *topo_opt;

    const int N = G.N;
    vi dp(N, 0);

    for (int v : topo) {
        for (auto& e : G[v]) {
            int to = e.to;
            ll add = (ll)dp[v] + 1; // +1 counts the apath consisting of just this edge extension
            dp[to] = (dp[to] + (int)(add % MOD)) % MOD;
        }
    }
    return dp;
}

// XOR Parity (mod 2) variant
template<class Cost>
vi dag_paths_ending_at_xor(const Graph<Cost, true>& G, int MOD) {
    auto topo_opt = toposort_kahn(G);
    if (!topo_opt) return {}; // not a DAG
    const auto& topo = *topo_opt;

    const int N = G.N;
    vi dp(N, 1);

    for (int v : topo) {
        for (auto& e : G[v]) {
            int to = e.to;
            dp[to] ^= dp[v];
        }
    }
    return dp;
}
