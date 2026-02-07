#include "../base.h"

template<class Cost>
struct BridgeRes {
    vi tin, low;
    V<char> is_bridge_by_eid; // size = G.M
    vi bridges_id;
};

template<class Cost>
BridgeRes<Cost> bridges(const Graph<Cost, false>& G) {
    const int N = G.N;
    int timer = 0;

    BridgeRes<Cost> res;
    res.tin.assign(N, -1);
    res.low.assign(N, -1);

    res.is_bridge_by_eid.assign(G.M, 0);

    vi parent_edge(N, -1);

    auto dfs = [&](auto&& self, int v) -> void {
        res.tin[v] = res.low[v] = timer++;

        for (auto& e : G[v]) {
            int to = e.to;
            int eid = e.id;
            if (eid == parent_edge[v]) continue;

            if (res.tin[to] == -1) {
                parent_edge[to] = eid;
                self(self, to);
                res.low[v] = min(res.low[v], res.low[to]);

                if (res.low[to] > res.tin[v]) {
                    if (0 <= eid && eid < sz(res.is_bridge_by_eid)) res.is_bridge_by_eid = 1;
                    res.bridges_id.push_back(eid);
                }
            } else {
                res.low[v] = min(res.low[v], res.tin[to]);
            }
        }
    };

    for (int v = 0; v < N; v++) {
        if (res.tin[v] == -1) dfs(dfs, v);
    }
    return res;
}

// 2-edge-connected components: components after removing bridges
// Returns comp_id per vertex, and number of components
template<class Cost>
pair<int, vi> two_edge_connected_components(const Graph<Cost, false>& G) {
    auto br = bridges(G);
    const int N = G.N;
    vi comp(N, -1);
    int cid = 0;

    auto dfs = [&](auto&& self, int v) -> void {
        comp[v] = cid;
        for (auto& e : G[v]) {
            int to = e.to;
            int eid = e.id;
            if (eid >= 0 && eid < sz(br.is_bridge_by_eid) && br.is_bridge_by_eid[eid]) continue;
            if (comp[to] == -1) self(self, to);
        }
    };

    for (int v = 0; v < N; v++) {
        if (comp[v] == -1) {
            dfs(dfs, v);
            ++cid;
        }
    }
    return {cid, comp};
}
