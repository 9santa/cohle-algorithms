#include "../base.h"

/*
   Each bucket stores nodes with current dist = index of that bucket (mod C+1)
   C -> max edge weight, small integer, if C = 1 -> its just 0-1 BFS
   O(V + E + C)
*/
template<class Cost, bool directed>
vl dial_shortest_paths(const Graph<Cost, directed>& G, int src, int C) {
    static_assert(std::is_integral<Cost>::value, "Dial expects integral costs");
    const int N = G.N;
    vl dist(N, infty<ll>);
    V<deque<int>> bucket(C+1);

    dist[src] = 0;
    bucket[0].push_back(src);

    ll idx = 0;
    int processed = 0;

    while (processed < N) {
        while (bucket[idx % (C+1)].empty()) ++idx;

        int v = bucket[idx % (C+1)].front();
        bucket[idx % (C+1)].pop_front();

        if (dist[v] < idx) continue; // outdated entry
        ++processed;

        for (auto& e : G[v]) {
            int to = e.to;
            int w = (int)e.cost;
            assert(w >= 0 && w <= C);
            if (dist[to] > dist[v] + w) {
                dist[to] = dist[v] + w;
                bucket[dist[to] % (C+1)].push_back(to);
            }
        }
    }
    return dist;
}
