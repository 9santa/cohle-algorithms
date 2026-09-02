#pragma once
#include <bits/stdc++.h>
using namespace std;

struct BridgeRes {
    vector<int> tin, low;
    vector<char> is_bridge; // indexed by input edge id
    vector<int> bridge_ids;
};

/**
 * Finds bridges in an undirected graph given as an edge list.
 * edges[id] = {u, v}. Parallel edges are handled correctly.
 * Time: O(n + m).
 */
BridgeRes bridges(int n, const vector<pair<int, int>>& edges) {
    vector<vector<pair<int, int>>> g(n); // (to, edge id)
    for (int id = 0; id < (int)edges.size(); ++id) {
        auto [u, v] = edges[id];
        g[u].push_back({v, id});
        g[v].push_back({u, id});
    }

    BridgeRes res;
    res.tin.assign(n, -1);
    res.low.assign(n, -1);
    res.is_bridge.assign(edges.size(), false);
    int timer = 0;

    auto dfs = [&](auto&& self, int v, int parent_edge) -> void {
        res.tin[v] = res.low[v] = timer++;
        for (auto [to, id] : g[v]) {
            if (id == parent_edge) continue;
            if (res.tin[to] == -1) {
                self(self, to, id);
                res.low[v] = min(res.low[v], res.low[to]);
                if (res.low[to] > res.tin[v]) {
                    res.is_bridge[id] = true;
                    res.bridge_ids.push_back(id);
                }
            } else {
                res.low[v] = min(res.low[v], res.tin[to]);
            }
        }
    };

    for (int v = 0; v < n; ++v) if (res.tin[v] == -1) dfs(dfs, v, -1);
    return res;
}

/** Components after removing all bridges. */
pair<int, vector<int>> two_edge_connected_components(
    int n,
    const vector<pair<int, int>>& edges
) {
    auto br = bridges(n, edges);
    vector<vector<pair<int, int>>> g(n);
    for (int id = 0; id < (int)edges.size(); ++id) {
        auto [u, v] = edges[id];
        g[u].push_back({v, id});
        g[v].push_back({u, id});
    }

    vector<int> comp(n, -1);
    int cid = 0;
    for (int s = 0; s < n; ++s) {
        if (comp[s] != -1) continue;
        stack<int> st;
        st.push(s);
        comp[s] = cid;
        while (!st.empty()) {
            int v = st.top();
            st.pop();
            for (auto [to, id] : g[v]) {
                if (br.is_bridge[id] || comp[to] != -1) continue;
                comp[to] = cid;
                st.push(to);
            }
        }
        ++cid;
    }
    return {cid, comp};
}
