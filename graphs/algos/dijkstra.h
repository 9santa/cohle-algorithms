#include "../base.h"

template<class Cost>
struct DijkstraRes {
    V<Cost> dist;
    vi parent; // prev vertex
};

template<class Cost, bool directed>
DijkstraRes<Cost> dijkstra(const Graph<Cost, directed>& G, int src) {
    const int N = G.N;
    const Cost INF = std::numeric_limits<Cost>::max() / 4;

    DijkstraRes<Cost> res;
    res.dist.assign(N, INF);
    res.parent.assign(N, -1);

    using P = pair<Cost, int>; // (dist, v)
    priority_queue<P, vector<P>, std::greater<P>> pq;

    res.dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, v] = pq.top(); pq.pop();
        if (d != res.dist[v]) continue; // outdated
        for (auto& e : G[v]) {
            int to = e.to;
            Cost nd = d + e.cost;
            if (nd < res.dist[to]) {
                res.dist[to] = nd;
                res.parent[to] = v;
                pq.push({nd, to});
            }
        }
    }
    return res;
}
