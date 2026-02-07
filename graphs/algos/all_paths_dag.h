#include "../base.h"

// Enumerate all paths from s to t in a DAG
// Number of paths can be exponential
template<class Cost>
V<vi> all_paths_dag(const Graph<Cost, true>& G, int s, int t) {
    V<vi> all;
    // stack state: (node, next_edge_index, current_path)
    struct State {
        int v;
        int it;
        vi path;
    };

    V<State> stk;
    stk.push_back({s, 0, {s}});

    while (!stk.empty()) {
        auto cur = std::move(stk.back());
        stk.pop_back();

        if (cur.v == t) {
            all.push_back(std::move(cur.path));
            continue;
        }

        // Expand all outgoing edges
        for (auto& e : G[cur.v]) {
            int to = e.to;
            auto np = cur.path;
            np.push_back(to);
            stk.push_back({to, 0, std::move(np)});
        }
    }
    return all;
}
