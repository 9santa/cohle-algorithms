#include "../header.h"

int K;
constexpr int MAXN = 100005, MAXK = 100006;
vector<int> g[MAXN];
int dp[MAXN][MAXK]; // dp[u][d] = number of vertices with distance 'd' from 'u' in the subtree

void dfs(int u, int p) {
    dp[u][0] = 1;
    for (int v : g[u]) if (v != p) {
        dfs(v, u);
        for (int d = 0; d < K; d++) {
            if (d+1 < K) {
                dp[u][d+1] += dp[v][d];
            }
        }
    }
}
