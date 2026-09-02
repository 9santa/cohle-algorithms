#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Subtree of v corresponds to euler[tin[v]..tout[v]). */
struct EulerTour {
    vector<int> tin, tout, euler;
};

EulerTour euler_tour(const vector<vector<int>>& g, int root = 0) {
    int n = (int)g.size();
    EulerTour et;
    et.tin.assign(n, -1);
    et.tout.assign(n, -1);
    et.euler.reserve(n);
    int timer = 0;

    auto dfs = [&](auto&& self, int v, int p) -> void {
        et.tin[v] = timer++;
        et.euler.push_back(v);
        for (int to : g[v]) {
            if (to != p) self(self, to, v);
        }
        et.tout[v] = timer;
    };

    if (n) dfs(dfs, root, -1);
    return et;
}
