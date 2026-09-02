#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Centroid decomposition of an undirected tree. */
struct CentroidDecomposition {
    int n = 0;
    vector<vector<int>> g;
    vector<int> parent; // parent in centroid tree, -1 at the root
    vector<int> sub;
    vector<char> removed;

    void build(const vector<vector<int>>& graph) {
        g = graph;
        n = (int)g.size();
        parent.assign(n, -1);
        sub.assign(n, 0);
        removed.assign(n, false);
        if (n) decompose(0, -1);
    }

private:
    int calc_size(int v, int p) {
        sub[v] = 1;
        for (int to : g[v]) {
            if (to == p || removed[to]) continue;
            sub[v] += calc_size(to, v);
        }
        return sub[v];
    }

    int find_centroid(int v, int p, int total) {
        for (int to : g[v]) {
            if (to == p || removed[to]) continue;
            if (sub[to] > total / 2) return find_centroid(to, v, total);
        }
        return v;
    }

    void decompose(int entry, int p) {
        int total = calc_size(entry, -1);
        int c = find_centroid(entry, -1, total);
        parent[c] = p;
        removed[c] = true;

        for (int to : g[c]) {
            if (!removed[to]) decompose(to, c);
        }
    }
};
