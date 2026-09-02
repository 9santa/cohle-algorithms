#pragma once
#include <bits/stdc++.h>
using namespace std;

/** DFS topological order without cycle checking. */
vector<int> toposort(const vector<vector<int>>& g) {
    int n = (int)g.size();
    vector<char> vis(n, false);
    vector<int> order;
    order.reserve(n);

    auto dfs = [&](auto&& self, int v) -> void {
        vis[v] = true;
        for (int to : g[v]) if (!vis[to]) self(self, to);
        order.push_back(v);
    };

    for (int v = 0; v < n; ++v) if (!vis[v]) dfs(dfs, v);
    reverse(order.begin(), order.end());
    return order;
}

/** Kahn topological order, or nullopt if the graph contains a cycle. */
optional<vector<int>> toposort_kahn(const vector<vector<int>>& g) {
    int n = (int)g.size();
    vector<int> indeg(n, 0);
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

/** DFS topological order with cycle detection. */
optional<vector<int>> toposort_dfs(const vector<vector<int>>& g) {
    int n = (int)g.size();
    vector<char> state(n, 0); // 0 = new, 1 = active, 2 = done
    vector<int> order;
    order.reserve(n);

    auto dfs = [&](auto&& self, int v) -> bool {
        state[v] = 1;
        for (int to : g[v]) {
            if (state[to] == 1) return false;
            if (state[to] == 0 && !self(self, to)) return false;
        }
        state[v] = 2;
        order.push_back(v);
        return true;
    };

    for (int v = 0; v < n; ++v) {
        if (state[v] == 0 && !dfs(dfs, v)) return nullopt;
    }
    reverse(order.begin(), order.end());
    return order;
}
