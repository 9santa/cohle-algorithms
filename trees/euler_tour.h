#include "rooted_tree.h"

struct EulerTour {
    vi tin;   // time in
    vi tout;  // time out
    vi euler; // euler order, subtree of v is the range: [tin[v], tout[v]]
};

template<class G>
EulerTour euler_tour(const G& g, const RootedTree& t, int root = 0) {
    int N = g.N;
    EulerTour et;
    et.tin.assign(N, -1);
    et.tout.assign(N, -1);
    et.euler.clear();
    et.euler.reserve(N);

    // iterative DFS using parent info
    vi it(N, 0);
    vi stk = {root};

    int timer = 0;
    while (!stk.empty()) {
        int v = stk.back();
        if (et.tin[v] == -1) {
            et.tin[v] = timer++;
            et.euler.push_back(v);
        }
        bool pushed = false;
        for (int &i = it[v]; i < g.out_deg(v); i++) {
            auto& e = g.csr[g.indptr[v] + i];
            int to = e.to;
            if (t.parent[to] != v) continue;
            stk.push_back(to);
            pushed = true;
            break;
        }
        if (!pushed) {
            et.tout[v] = timer;
            stk.pop_back();
        }
    }
    return et;
}
