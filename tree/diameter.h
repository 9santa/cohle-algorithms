#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Farthest vertex and distance in an unweighted tree. */
pair<int, int> farthest_from(const vector<vector<int>>& g, int src) {
    int n = (int)g.size();
    vector<int> dist(n, -1);
    queue<int> q;
    dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (int to : g[v]) {
            if (dist[to] != -1) continue;
            dist[to] = dist[v] + 1;
            q.push(to);
        }
    }

    int best = src;
    for (int v = 0; v < n; ++v) if (dist[v] > dist[best]) best = v;
    return {best, dist[best]};
}

int tree_diameter(const vector<vector<int>>& g) {
    auto [a, _] = farthest_from(g, 0);
    return farthest_from(g, a).second;
}

/** Weighted adjacency entries are (to, weight). */
template<class Cost>
pair<int, Cost> farthest_from(const vector<vector<pair<int, Cost>>>& g, int src) {
    int n = (int)g.size();
    vector<Cost> dist(n);
    vector<int> parent(n, -1);
    vector<int> st = {src};
    parent[src] = src;

    while (!st.empty()) {
        int v = st.back();
        st.pop_back();
        for (auto [to, w] : g[v]) {
            if (parent[to] != -1) continue;
            parent[to] = v;
            dist[to] = dist[v] + w;
            st.push_back(to);
        }
    }

    int best = src;
    for (int v = 0; v < n; ++v) if (dist[v] > dist[best]) best = v;
    return {best, dist[best]};
}

template<class Cost>
Cost tree_diameter(const vector<vector<pair<int, Cost>>>& g) {
    auto [a, _] = farthest_from(g, 0);
    return farthest_from(g, a).second;
}
