#include "../header.h"

constexpr int MAXN = 100005;

int n;
vector<int> g[MAXN];
int subtree_size[MAXN];
vb used;
vi parent;

void dfs(int v, int p) {
    subtree_size[v] = 1;
    for (auto u : g[v]) {
        if (u == p || used[u]) continue;
        dfs(u, v);
        subtree_size[v] += subtree_size[u];
    }
}

int find_centroid(int v, int p, int tot) {
    for (int u : g[v]) {
        if (u == p || used[u]) continue;
        if (subtree_size[u] > tot/2) {
            return find_centroid(u, v, tot);
        }
    }
    return v;
}

void build(int v, int p) {
    dfs(v, -1);
    int c = find_centroid(v, -1, subtree_size[v]);

    parent[c] = p;  // p = -1 for root of decomposition
    used[c] = true;

    for (int u : g[c]) {
        if (!used[u]) build(u, c);
    }
}
