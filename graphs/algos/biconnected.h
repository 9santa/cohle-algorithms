#include "../base.h"

// Vertex-biconnected components + acticulation points
// Returns edge-blocks (components of edges)

template<class Cost>
struct BiconnectedRes {
    vi tin, low;
    V<char> is_articulation;
    V<vi> edge_components; // each comp is a list of edge-ids
};

template<class Cost>
BiconnectedRes<Cost> biconnected_components(const Graph<Cost, false>& G) {
    const int N = G.N;
    int timer = 0;

    BiconnectedRes<Cost> res;
    res.tin.assign(N, -1);
    res.low.assign(N, -1);
    res.is_articulation.assign(N, 0);

    vi stk_edges; // stack of edge ids

    auto dfs = [&](auto&& self, int v, int parent_v, int parent_eid) -> void {
        res.tin[v] = res.low[v] = timer++;
        int child_count = 0;

        for (auto& e : G[v]) {
            int to = e.to;
            int eid = e.id;
            if (eid == parent_eid) continue;

            if (res.tin[to] == -1) {
                stk_edges.push_back(eid);
                ++child_count;

                self(self, to, v, eid);
                res.low[v] = min(res.low[v], res.low[to]);

                // articulation / biconnected split condition
                if (res.low[to] >= res.tin[v]) {
                    if (parent_v != -1) res.is_articulation[v] = 1;

                    // pop edges until eid to form one biconnected component
                    vi comp;
                    while (!stk_edges.empty()) {
                        int x = stk_edges.back();
                        stk_edges.pop_back();
                        comp.push_back(x);
                        if (x == eid) break;
                    }
                    res.edge_components.push_back(std::move(comp));
                }
            } else if (res.tin[to] < res.tin[v]) {
                // back edge to an ancestor (push once; tin[to] < tin[v] avoids duplicates)
                stk_edges.push_back(eid);
                res.low[v] = min(res.low[v], res.tin[to]);
            }
        }

        // root articulation condition
        if (parent_v == -1 && child_count >= 2) res.is_articulation[v] = 1;
    };

    for (int v = 0; v < N; v++) {
        if (res.tin[v] == -1) {
            dfs(dfs, v, -1, -1);
            // flush any leftover edges (in case of weird input)
            if (!stk_edges.empty()) {
                res.edge_components.push_back(std::move(stk_edges));
                stk_edges.clear();
            }
        }
    }
    return res;
}
