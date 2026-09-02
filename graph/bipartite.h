#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Returns {ok, color}; color[v] is 0/1 when the graph is bipartite. */
pair<bool, vector<int>> bipartite_coloring(const vector<vector<int>>& g) {
    int n = (int)g.size();
    vector<int> color(n, -1);

    for (int s = 0; s < n; ++s) {
        if (color[s] != -1) continue;
        queue<int> q;
        color[s] = 0;
        q.push(s);

        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int to : g[v]) {
                if (color[to] == -1) {
                    color[to] = color[v] ^ 1;
                    q.push(to);
                } else if (color[to] == color[v]) {
                    return {false, {}};
                }
            }
        }
    }
    return {true, color};
}
