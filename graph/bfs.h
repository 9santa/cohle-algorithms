#pragma once
#include <bits/stdc++.h>
using namespace std;

struct BFSRes {
    vector<int> dist;    // -1 = unreachable
    vector<int> parent;  // previous vertex on a shortest path
};

/** Runs BFS from one source on an unweighted graph. Time: O(n + m). */
BFSRes bfs(const vector<vector<int>>& g, int src) {
    int n = (int)g.size();
    BFSRes res{vector<int>(n, -1), vector<int>(n, -1)};

    queue<int> q;
    res.dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (int to : g[v]) {
            if (res.dist[to] != -1) continue;
            res.dist[to] = res.dist[v] + 1;
            res.parent[to] = v;
            q.push(to);
        }
    }
    return res;
}

/** BFS that does not expand vertices at depth max_depth. */
BFSRes bfs_limited_depth(const vector<vector<int>>& g, int src, int max_depth) {
    int n = (int)g.size();
    BFSRes res{vector<int>(n, -1), vector<int>(n, -1)};

    queue<int> q;
    res.dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        if (res.dist[v] == max_depth) continue;

        for (int to : g[v]) {
            if (res.dist[to] != -1) continue;
            res.dist[to] = res.dist[v] + 1;
            res.parent[to] = v;
            q.push(to);
        }
    }
    return res;
}

vector<int> restore_path(int target, const vector<int>& parent, const vector<int>& dist) {
    if (target < 0 || target >= (int)parent.size() || dist[target] == -1) return {};
    vector<int> path;
    for (int v = target; v != -1; v = parent[v]) path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}
