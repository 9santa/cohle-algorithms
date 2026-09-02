#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Returns the one or two centers of a tree. Time: O(n). */
vector<int> tree_centers_by_leaf_peeling(const vector<vector<int>>& g) {
    int n = (int)g.size();
    if (n <= 2) {
        vector<int> ans(n);
        iota(ans.begin(), ans.end(), 0);
        return ans;
    }

    vector<int> deg(n);
    queue<int> q;
    for (int v = 0; v < n; ++v) {
        deg[v] = g[v].size();
        if (deg[v] == 1) q.push(v);
    }

    int remaining = n;
    while (remaining > 2) {
        int layer = q.size();
        remaining -= layer;
        while (layer--) {
            int v = q.front();
            q.pop();
            for (int to : g[v]) {
                if (--deg[to] == 1) q.push(to);
            }
        }
    }

    vector<int> centers;
    while (!q.empty()) {
        centers.push_back(q.front());
        q.pop();
    }
    return centers;
}
