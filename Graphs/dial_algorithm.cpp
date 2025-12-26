#include "../header.h"

/*
   Each bucket stores nodes with current dist = index of that bucket (mod C+1)
   C -> max edge weight, small integer, if C = 1 -> its just 0-1 BFS
   O(V + E + C)
*/
vector<int> dial(const vector<vector<pair<int, int>>>& g, int C, int src) {
    int n = sz(g);

    vector<int> dist(n, infty<int>);
    vector<deque<int>> bucket(C+1);

    dist[src] = 0;
    bucket[0].push_back(src);

    int idx = 0, processed = 0;
    while (processed < n) {
        while (bucket[idx % (C+1)].empty()) idx++;  // find the correct bucket

        int u = bucket[idx % (C+1)].front();
        bucket[idx % (C+1)].pop_front();

        if (dist[u] < idx) continue;    // outdated
        processed++;

        for (auto [v, w] : g[u]) {
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                bucket[dist[v] % (C+1)].push_back(v);
            }
        }
    }

    return dist;
}
