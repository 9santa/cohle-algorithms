// #pragma GCC optimize("Ofast,inline,unroll-loops")
// #pragma GCC target("avx2,popcnt")

#include "../header.h"

constexpr int MOD = 1e9+7;

constexpr int MAXN = 100005;
vi g[MAXN];
int dfs(int u, int p, int& diam) {
    int max1 = 0, max2 = 0; // two max children depths
    for (int v : g[u]) {
        if (v == p) continue;
        int depth = dfs(v, u, diam) + 1;
        if (depth > max1) {
            max2 = max1;
            max1 = depth;
        } else if (depth > max2) {
            max2 = depth;
        }
    }
    diam = max(diam, max1 + max2); // diameter through current vertex
    return max1; // return max depth of this subtree
}
