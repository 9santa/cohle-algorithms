#include "../base.h"

struct SCCRes {
    int n_comp = 0;
    vi comp;      // comp[v] in [0..n_comp) => which component vertex v belongs to
    V<vi> groups; // vertices per component
};

// Kosaraju: O(N+M). Requires directed graph
template<class Cost>
SCCRes scc_kosaraju(const Graph<Cost, true>& G) {
    const int N = G.N;
    vector<char> vis(N, 0);
    vi order;
    order.reserve(N);

    // First pass: get finishing order (iter DFS)
    for (int s = 0; s < N; s++) {
        if (vis[s]) continue;

        vi stk_v;
        vi stk_it;
        stk_v.push_back(s);
        stk_it.push_back(G.indptr[s]);
        vis[s] = 1;

        while (!stk_v.empty()) {
            int v = stk_v.back();
            int &it = stk_it.back();
            int end = G.indptr[v+1];

            if (it == end) {
                order.push_back(v);
                stk_v.pop_back();
                stk_it.pop_back();
                continue;
            }

            int to = G.csr[it].to;
            ++it;

            if (!vis[to]) {
                vis[to] = 1;
                stk_v.push_back(to);
                stk_it.push_back(G.indptr[to]);
            }
        }
    }

    // Second pass on reversed graph
    auto R = G.reverse_graph();

    vi comp(N, -1);
    int cid = 0;

    for (int i = N-1; i >= 0; i--) {
        int s = order[i];
        if (comp[s] != -1) continue;

        vi stk = {s};
        comp[s] = cid;

        while (!stk.empty()) {
            int v = stk.back(); stk.pop_back();
            for (auto& e : R[v]) {
                int to = e.to;
                if (comp[to] == -1) {
                    comp[to] = cid;
                    stk.push_back(to);
                }
            }
        }
        ++cid;
    }

    SCCRes res;
    res.n_comp = cid;
    res.comp = std::move(comp);
    res.groups.assign(res.n_comp, {});
    for (int v = 0; v < N; v++) res.groups[res.comp[v]].push_back(v);
    return res;
}
