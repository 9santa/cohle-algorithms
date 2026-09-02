#pragma once
#include <bits/stdc++.h>
using namespace std;

struct SCCRes {
    int n_comp = 0;
    vector<int> comp;
    vector<vector<int>> groups;
};

/** Kosaraju SCC decomposition of a directed adjacency list. Time: O(n + m). */
SCCRes scc_kosaraju(const vector<vector<int>>& g) {
    int n = (int)g.size();
    vector<vector<int>> rg(n);
    for (int v = 0; v < n; ++v) for (int to : g[v]) rg[to].push_back(v);

    vector<char> vis(n, false);
    vector<int> order;
    order.reserve(n);

    auto dfs1 = [&](auto&& self, int v) -> void {
        vis[v] = true;
        for (int to : g[v]) if (!vis[to]) self(self, to);
        order.push_back(v);
    };
    for (int v = 0; v < n; ++v) if (!vis[v]) dfs1(dfs1, v);

    vector<int> comp(n, -1);
    int cid = 0;
    auto dfs2 = [&](auto&& self, int v) -> void {
        comp[v] = cid;
        for (int to : rg[v]) if (comp[to] == -1) self(self, to);
    };

    reverse(order.begin(), order.end());
    for (int v : order) {
        if (comp[v] != -1) continue;
        dfs2(dfs2, v);
        ++cid;
    }

    SCCRes res;
    res.n_comp = cid;
    res.comp = move(comp);
    res.groups.assign(cid, {});
    for (int v = 0; v < n; ++v) res.groups[res.comp[v]].push_back(v);
    return res;
}
