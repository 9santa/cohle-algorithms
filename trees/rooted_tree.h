#pragma once
#include "../graphs/base.h"

struct RootedTree {
    vi parent;      // parent[v] (root has parent[root] = -1)
    vi parent_eid;  // edge-id connecting v to parent
    vi depth;       // depth in edges from root
    vi order;       // traversal order
};

// DFS-rooting (iterative). Good when you ant a traversal order for subtree DP
template<class Cost>
RootedTree tree_dfs_root(const Graph<Cost, false>& G, int root = 0) {
    int N = G.N;
    RootedTree info;
    info.parent.assign(N, -1);
    info.parent_eid.assign(N, -1);
    info.depth.assign(N, -1);
    info.order.clear();
    info.order.reserve(N);

    vector<int> stk = {root};
    info.parent[root] = -1;
    info.depth[root] = 0;

    while (!stk.empty()) {
        int v = stk.back(); stk.pop_back();
        info.order.push_back(v);
        for (auto& e : G[v]) {
            int to = e.to;
            if (info.depth[to] != -1) continue;
            info.depth[to] = info.depth[v] + 1;
            info.parent[to] = v;
            info.parent_eid[to] = e.id;
            stk.push_back(to);
        }
    }
    return info;
}

// BFS-rooting. Depth is shortest-path distance in number of edges from root
template<class Cost>
RootedTree tree_bfs_root(const Graph<Cost, false>& G, int root = 0) {
    int N = G.N;
    RootedTree info;
    info.parent.assign(N, -1);
    info.parent_eid.assign(N, -1);
    info.depth.assign(N, -1);
    info.order.clear();
    info.order.reserve(N);

    queue<int> q;
    q.push(root);
    info.parent[root] = -1;
    info.depth[root] = 0;

    while (!q.empty()) {
        int v = q.front(); q.pop();
        info.order.push_back(v);
        for (auto& e : G[v]) {
            int to = e.to;
            if (info.depth[to] != -1) continue;
            info.depth[to] = info.depth[v] + 1;
            info.parent[to] = v;
            info.parent_eid[to] = e.id;
            q.push(to);
        }
    }
    return info;
}


// Make a directed tree from graph. Returns Graph<T, directed=true>
template<class T, class UnirectedGraph>
auto make_directed_tree(const UnirectedGraph& g, const RootedTree& t) {
    Graph<T, true> tr(g.N);
    for (int v : t.order) {
        for (auto& e : g[v]) {
            int to = e.to;
            if (t.parent[to] == v) {
                tr.add_edge(v, to, e.cost, e.id);
            }
        }
    }
    tr.build();
    return tr;
}
