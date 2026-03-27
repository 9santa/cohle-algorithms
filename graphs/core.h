#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <deque>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

using u128 = unsigned __int128;
using i128 = __int128;
using u64  = unsigned long long;
using i64  = long long;
using ll   = long long;
using u32  = unsigned int;
using u16  = uint16_t;
using u8   = uint8_t;
using ld   = long double;

using pii = pair<int, int>;

template<typename T>
using V = vector<T>;
template<typename T, size_t SZ>
using AR = array<T, SZ>;
using vi = V<int>;
using vl = V<ll>;
using vb = V<bool>;
using vpii = V<pii>;

#ifndef sz
#define sz(x) int((x).size())
#endif

template<class T>
constexpr T infty = 0;
template<>
constexpr int infty<int> = 1'010'000'000;
template<>
constexpr ll infty<ll> = 2'020'000'000'000'000'000;
template<>
constexpr double infty<double> = numeric_limits<double>::infinity();
template<>
constexpr long double infty<long double> = numeric_limits<long double>::infinity();

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
