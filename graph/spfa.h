#pragma once
#include <bits/stdc++.h>
using namespace std;

template<class Cost>
struct SPFARes {
    vector<Cost> dist;
    bool has_negative_cycle = false; // reachable from src
};

/** Adjacency entries are (to, weight). Worst-case time: O(nm). */
template<class Cost>
SPFARes<Cost> spfa(const vector<vector<pair<int, Cost>>>& g, int src) {
    int n = (int)g.size();
    const Cost INF = numeric_limits<Cost>::max() / 4;

    SPFARes<Cost> res{vector<Cost>(n, INF), false};
    vector<char> in_queue(n, false);
    vector<int> push_count(n, 0);
    queue<int> q;

    res.dist[src] = 0;
    q.push(src);
    in_queue[src] = true;
    push_count[src] = 1;

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        in_queue[v] = false;

        for (auto [to, w] : g[v]) {
            if (res.dist[v] == INF) continue;
            Cost nd = res.dist[v] + w;
            if (nd >= res.dist[to]) continue;
            res.dist[to] = nd;

            if (!in_queue[to]) {
                q.push(to);
                in_queue[to] = true;
                if (++push_count[to] > n) {
                    res.has_negative_cycle = true;
                    return res;
                }
            }
        }
    }
    return res;
}
