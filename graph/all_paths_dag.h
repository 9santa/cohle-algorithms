#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Enumerates all directed paths from s to t in a DAG. Time: O(total output length). */
vector<vector<int>> all_paths_dag(const vector<vector<int>>& g, int s, int t) {
    vector<vector<int>> ans;
    vector<int> path;

    auto dfs = [&](auto&& self, int v) -> void {
        path.push_back(v);
        if (v == t) {
            ans.push_back(path);
        } else {
            for (int to : g[v]) self(self, to);
        }
        path.pop_back();
    };

    dfs(dfs, s);
    return ans;
}
