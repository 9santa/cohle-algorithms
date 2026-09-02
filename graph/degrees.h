#pragma once
#include <bits/stdc++.h>
using namespace std;

vector<int> out_degrees(const vector<vector<int>>& g) {
    vector<int> out(g.size());
    for (int v = 0; v < (int)g.size(); ++v) out[v] = g[v].size();
    return out;
}

vector<int> in_degrees(const vector<vector<int>>& g) {
    vector<int> in(g.size(), 0);
    for (int v = 0; v < (int)g.size(); ++v) {
        for (int to : g[v]) ++in[to];
    }
    return in;
}
