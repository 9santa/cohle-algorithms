#pragma once
#include <bits/stdc++.h>
using namespace std;

/**
 * Dial shortest paths for nonnegative integer weights in [0, C].
 * Adjacency entries are (to, weight). Time: O(m + Cn).
 */
template<class Cost>
vector<long long> dial_shortest_paths(
    const vector<vector<pair<int, Cost>>>& g,
    int src,
    int C
) {
    static_assert(is_integral_v<Cost>, "Dial expects integral weights");
    int n = (int)g.size();
    const long long INF = numeric_limits<long long>::max() / 4;
    int max_dist = max(0, C * max(0, n - 1));

    vector<long long> dist(n, INF);
    vector<vector<int>> bucket(max_dist + 1);
    dist[src] = 0;
    bucket[0].push_back(src);

    for (int d = 0; d <= max_dist; ++d) {
        while (!bucket[d].empty()) {
            int v = bucket[d].back();
            bucket[d].pop_back();
            if (dist[v] != d) continue;

            for (auto [to, w0] : g[v]) {
                int w = (int)w0;
                assert(0 <= w && w <= C);
                long long nd = dist[v] + w;
                if (nd < dist[to]) {
                    dist[to] = nd;
                    if (nd <= max_dist) bucket[nd].push_back(to);
                }
            }
        }
    }
    return dist;
}
