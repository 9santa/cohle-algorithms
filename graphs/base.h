#pragma once
#include "../header.h"

template<class T>
struct Edge {
    int frm, to;
    T cost;
    int id;
};

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

    void build_empty(int n) {
        N = n, M = 0; built = false;
        edges.clear();
        indptr.clear();
        csr.clear();
    }

    // call before build
    void add_edge(int frm, int to, T cost = 1, int id = -1) {
        assert(!built);
        assert(0 <= frm && frm < N);
        assert(0 <= to && to < N);
        if (id == -1) id = M;
        edges.push_back({frm, to, cost, id});
        M++;
    }

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

    int out_deg(int v) const {
        assert(0 <= v && v < N);
        return indptr[v+1] - indptr[v];
    }

    struct Range {
        const Edge<T> *l;
        const Edge<T> *r;
        const Edge<T> *begin() const { return l; }
        const Edge<T> *end() const { return r; }
    };

    Range operator[](int v) const {
        // call after build
        return Range{ &csr[indptr[v]], &csr[indptr[v+1]]};
    }

    Graph<T, true> reverse_graph() const {
        static_assert(directed, "reverse_graph is for directed graphs only");
        Graph<T, true> R(N);
        for (auto& e : edges) R.add_edge(e.to, e.frm, e.cost, e.id);
        R.build();
        return R;
    }
};
