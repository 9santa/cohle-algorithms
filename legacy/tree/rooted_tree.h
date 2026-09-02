#pragma once
#include <bits/stdc++.h>
using namespace std;

using vi = vector<int>;

/** Weighted edge in a graph. */
template<class T>
struct Edge {
    int frm, to;
    T cost;
    int id;
};

/** CSR-backed graph; undirected graphs store both adjacency directions after build. Space: O(n + m). */
template<class T = int, bool directed = false>
struct Graph {
    int N = 0;
    int M = 0; // number of added edges (undirected counts as 1)
    bool built = false;

    vector<Edge<T>> edges; // edge list before build

    vector<int> indptr; // size N + 1
    vector<Edge<T>> csr; // adjacency (size = indptr.back())

    Graph() {}
    Graph(int n) { build_empty(n); }

    /** Resets graph to n vertices and no edges. Time: O(n + m). */
    void build_empty(int n) {
        N = n, M = 0; built = false;
        edges.clear();
        indptr.clear();
        csr.clear();
    }

    // call before build
    /** Adds an edge before build. Time: O(1). */
    void add_edge(int frm, int to, T cost = 1, int id = -1) {
        assert(!built);
        assert(0 <= frm && frm < N);
        assert(0 <= to && to < N);
        if (id == -1) id = M;
        edges.push_back({frm, to, cost, id});
        M++;
    }

    /** Builds CSR adjacency from the edge list. Time: O(n + m). */
    void build() {
        built = true;
        indptr.assign(N+1, 0);
        for (auto& e : edges) {
            indptr[e.frm + 1]++;
            if (!directed) indptr[e.to + 1]++;
        }
        for (int v = 0; v < N; v++) indptr[v + 1] += indptr[v];

        auto counter = indptr;
        csr.resize(indptr.back());

        for (auto& e : edges) {
            csr[counter[e.frm]++] = e;
            if (!directed) csr[counter[e.to]++] = Edge<T>{e.to, e.frm, e.cost, e.id};
        }
    }

    /** Returns out-degree of v. Time: O(1). */
    int out_deg(int v) const {
        assert(0 <= v && v < N);
        return indptr[v+1] - indptr[v];
    }

    /** Lightweight adjacency range for one vertex. */
    struct Range {
        const Edge<T> *l;
        const Edge<T> *r;
        const Edge<T> *begin() const { return l; }
        const Edge<T> *end() const { return r; }
    };

    /** Returns adjacency range of v. Time: O(1). */
    Range operator[](int v) const {
        // call after build
        return Range{ &csr[indptr[v]], &csr[indptr[v+1]]};
    }

    /** Returns the reversed directed graph. Time: O(n + m). Space: O(n + m). */
    Graph<T, true> reverse_graph() const {
        static_assert(directed, "reverse_graph is for directed graphs only");
        Graph<T, true> R(N);
        for (auto& e : edges) R.add_edge(e.to, e.frm, e.cost, e.id);
        R.build();
        return R;
    }
};


/** Rooted-tree metadata from DFS/BFS rooting. Space: O(n). */
struct RootedTree {
    vi parent;      // parent[v] (root has parent[root] = -1)
    vi parent_eid;  // edge-id connecting v to parent
    vi depth;       // depth in edges from root
    vi order;       // traversal order
};

// DFS-rooting (iterative). Good when you want a traversal order for subtree DP
/** Roots a tree by iterative DFS. Time: O(n). Space: O(n). */
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
/** Roots a tree by BFS. Time: O(n). Space: O(n). */
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
/** Converts a rooted undirected tree into directed parent-to-child edges. Time: O(n). */
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
