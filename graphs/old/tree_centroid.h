#include "../header.h"

constexpr int MAXN = 100005;
vector<int> g[MAXN];
int subtree_size[MAXN];

void dfs(int v, int p) {
    subtree_size[v] = 1;
    for (auto u : g[v]) {
        if (u == p) continue;
        dfs(u, v);
        subtree_size[v] += subtree_size[u];
    }
}

int find_centroid(int v, int p, int tot) {
    for (int u : g[v]) {
        if (u == p) continue;
        if (subtree_size[u] > tot/2) {
            return find_centroid(u, v, tot);
        }
    }
    return v;
}
