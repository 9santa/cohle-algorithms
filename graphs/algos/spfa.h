#include "../base.h"

template<class Cost>
struct SPFARes {
    V<Cost> dist;
    bool has_negative_cycle = false; // reachable from src
};

// Directed graph expected
template<class Cost>
SPFARes<Cost> spfa(const Graph<Cost, true>& G, int src) {
    const int N = G.N;
    const Cost INF = numeric_limits<Cost>::max() / 4;

    SPFARes<Cost> res;
    res.dist.assign(N, INF);

    V<char> inq(N, 0);
    vi relax_cnt(N, 0);
    queue<int> q;

    res.dist[src] = 0;
    q.push(src);
    inq[src] = 1;

    while (!q.empty()) {
        int v = q.front(); q.pop();
        inq[v] = 0;

        for (auto& e : G[v]) {
            int to = e.to;
            if (res.dist[v] == INF) continue;
            Cost nd = res.dist[v] + e.cost;
            if (nd < res.dist[to]) {
                res.dist[to] = nd;
                if (!inq[to]) {
                    q.push(to);
                    inq[to] = 1;
                    if (++relax_cnt[to] >= N) {
                        res.has_negative_cycle = true;
                        return res;
                    }
                }
            }
        }
    }
    return res;
}
