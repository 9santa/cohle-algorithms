#include "../header.h"

constexpr int MAXN = 100005;

vector<int> g[MAXN];
int dp[MAXN][2];    // 0 - don't take u, 1 - take u
int weight[MAXN];

void dfs_mwis(int u, int p) {
    dp[u][1] = weight[u]; // take u
    dp[u][0] = 0;         // do not take u

    for (int v : g[u]) if (v != p) {
        dfs_mwis(v, u);
        dp[u][1] += dp[v][0]; // if we take u, can't take children
        dp[u][0] += max(dp[v][0], dp[v][1]);  // if we don't take u, can choose max from children
    }
}
