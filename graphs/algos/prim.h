#include "../base.h"

template<class Cost>
struct PrimRes {
    Cost total_weight = 0;
    vi parent;      // parent[v] in MST (root parent[root] = root)
    vi parent_eid;  // edge-id used to connect v to parent
    bool is_connected = true;
};

template<class Cost>
PrimRes<Cost> prim_mst(const Graph<Cost, false>& G, int root = 0) {
    const int N = G.N;
    const Cost INF = numeric_limits<Cost>::max() / 4;

    PrimRes<Cost> res;
    res.parent.assign(N, -1);
    res.parent_eid.assign(N, -1);

    V<Cost> best(N, INF);
    V<char> used(N, 0);

    using P = pair<Cost, int>; // (weight, v)
    priority_queue<P, vector<P>, std::greater<P>> pq;

    best[root] = 0;
    res.parent[root] = root;
    pq.push({0, root});

    int taken = 0;
    while (!pq.empty()) {
        auto [key, v] = pq.top(); pq.pop();
        if (used[v]) continue;
        used[v] = 1;
        taken++;
        res.total_weight += key;

        for (auto& e : G[v]) {
            int to = e.to;
            if (used[to]) continue;
            if (e.cost < best[to]) {
                best[to] = e.cost;
                res.parent[to] = v;
                res.parent_eid[to] = e.id;
                pq.push({best[to], to});
            }
        }
    }

    res.is_connected = (taken == N);
    return res;
}
