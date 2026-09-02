#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Prim MST result. */
template<class Cost>
struct PrimRes {
    Cost total_weight = 0;
    vector<int> parent;
    bool is_connected = true;
};

/** Adjacency entries are (to, weight). The graph must be undirected. */
template<class Cost>
PrimRes<Cost> prim_mst(const vector<vector<pair<int, Cost>>>& g, int root = 0) {
    int n = (int)g.size();
    const Cost INF = numeric_limits<Cost>::max() / 4;

    PrimRes<Cost> res;
    res.parent.assign(n, -1);
    vector<Cost> best(n, INF);
    vector<char> used(n, false);

    using State = pair<Cost, int>;
    priority_queue<State, vector<State>, greater<State>> pq;
    best[root] = 0;
    res.parent[root] = root;
    pq.push({0, root});

    int taken = 0;
    while (!pq.empty()) {
        auto [w, v] = pq.top();
        pq.pop();
        if (used[v]) continue;
        used[v] = true;
        ++taken;
        res.total_weight += w;

        for (auto [to, cost] : g[v]) {
            if (!used[to] && cost < best[to]) {
                best[to] = cost;
                res.parent[to] = v;
                pq.push({cost, to});
            }
        }
    }

    res.is_connected = (taken == n);
    return res;
}
