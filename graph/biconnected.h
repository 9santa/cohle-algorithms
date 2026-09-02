#pragma once
#include <bits/stdc++.h>
using namespace std;

struct BiconnectedRes {
    vector<int> tin, low;
    vector<char> is_articulation;
    vector<vector<int>> edge_components; // input edge ids per block
};

/**
 * Vertex-biconnected edge blocks and articulation points of an undirected graph.
 * edges[id] = {u, v}. Time: O(n + m).
 */
BiconnectedRes biconnected_components(int n, const vector<pair<int, int>>& edges) {
    vector<vector<pair<int, int>>> g(n); // (to, edge id)
    for (int id = 0; id < (int)edges.size(); ++id) {
        auto [u, v] = edges[id];
        g[u].push_back({v, id});
        g[v].push_back({u, id});
    }

    BiconnectedRes res;
    res.tin.assign(n, -1);
    res.low.assign(n, -1);
    res.is_articulation.assign(n, false);

    vector<int> edge_stack;
    int timer = 0;

    auto dfs = [&](auto&& self, int v, int parent_v, int parent_edge) -> void {
        res.tin[v] = res.low[v] = timer++;
        int children = 0;

        for (auto [to, id] : g[v]) {
            if (id == parent_edge) continue;

            if (res.tin[to] == -1) {
                ++children;
                edge_stack.push_back(id);
                self(self, to, v, id);
                res.low[v] = min(res.low[v], res.low[to]);

                if (res.low[to] >= res.tin[v]) {
                    if (parent_v != -1) res.is_articulation[v] = true;
                    vector<int> block;
                    while (true) {
                        int e = edge_stack.back();
                        edge_stack.pop_back();
                        block.push_back(e);
                        if (e == id) break;
                    }
                    res.edge_components.push_back(move(block));
                }
            } else if (res.tin[to] < res.tin[v]) {
                edge_stack.push_back(id);
                res.low[v] = min(res.low[v], res.tin[to]);
            }
        }

        if (parent_v == -1 && children > 1) res.is_articulation[v] = true;
    };

    for (int v = 0; v < n; ++v) {
        if (res.tin[v] != -1) continue;
        dfs(dfs, v, -1, -1);
        edge_stack.clear();
    }
    return res;
}
