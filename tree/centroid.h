#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Returns a centroid of a tree. Time: O(n). */
int tree_centroid(const vector<vector<int>>& g) {
    int n = (int)g.size();
    vector<int> sub(n);

    auto dfs = [&](auto&& self, int v, int p) -> void {
        sub[v] = 1;
        for (int to : g[v]) {
            if (to == p) continue;
            self(self, to, v);
            sub[v] += sub[to];
        }
    };
    dfs(dfs, 0, -1);

    int v = 0, p = -1;
    while (true) {
        int next = -1;
        for (int to : g[v]) {
            if (to == p) continue;
            if (sub[to] > n / 2) {
                next = to;
                break;
            }
        }
        if (next == -1) return v;
        p = v;
        v = next;
    }
}
