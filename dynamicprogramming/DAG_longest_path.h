#include "../header.h"

// with pseudo-topsort first
namespace first {
vi topo;
vb used;
V<vi> graph;

void dfs(int v) {
    used[v] = true;
    for (auto u : graph[v]) {
        if (!used[u]) {
            dfs(u);
        }
    }
    topo.pb(v);
}

void solve(void) {
    int n, m; cin >> n >> m;
    graph.resize(n);
    used.assign(n, false);
    while (m--) {
        int u, v; cin >> u >> v;
        u--, v--;
        graph[u].pb(v);
    }

    F0R(i, n) {
        if (!used[i]) dfs(i);
    }

    vi dp(n, 0);

    reverse(all(topo));
    for (auto u : topo) {
        for (auto v : graph[u]) {
            dp[v] = max(dp[v] , dp[u]+1);
        }
    }

    int ans = *max_element(all(dp));
    print(ans);

}
} // namespace first

// in 1 dfs without explicit topsort
namespace second {
constexpr int MAXN = 200005;
vi g[MAXN];
int dp[MAXN];
int parent[MAXN];

int dfs(int u) {
    if (dp[u] != -1) return dp[u];

    dp[u] = 1; // atleast 1 length path from this vertex (vertex itself)
    parent[u] = -1;

    for (int v : g[u]) {
        int candi = 1 + dfs(v);
        if (candi > dp[u]) {
            dp[u] = candi;
            parent[u] = v;
        }
    }
    return dp[u];
}

int main() {
    int n;
    memset(dp, -1, sizeof(dp));
    int max_length = 0;
    int startv = -1;
    for (int i = 1; i <= n; i++) {
        int length = dfs(i);
        if (length > max_length) {
            max_length = length;
            startv = i;
        }
    }
    return 0;
}

} // namespace second
