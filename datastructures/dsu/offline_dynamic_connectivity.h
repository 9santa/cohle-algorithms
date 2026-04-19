#pragma once
#include "../core.h"
#include "dsu_rollback.h"

/* Time complexity: O(m log q)*log n + q*log n)
n = number of vertices
q = number of operations total
m = number of edge-active intervals
Each interval is isnerted into seg in O(log q) nodes.
So if there are m active intervals total, number of stored edge copies is: O(m log q)
During the DFS, number of unite() calls is O(m log q) -> O((m log q) * log n)
Query answering -> two find() calls -> O(log n), for q operations -> O(q*log n)
Total = Edge preprocessing + Query answering = O((m log q)*log n + q*log n) */

struct Edge {
    int u, v;
};

struct Query {
    char type; // '+', '-', '?'
    int u, v;
};

struct OfflineDynamicConnectivity {
    int n, q;
    vector<Query> queries;
    vector<vector<Edge>> seg;
    vector<string> answer;
    RollbackDSU dsu;

    OfflineDynamicConnectivity(int n_, const vector<Query>& queries)
        : n(n_), q((int)queries.size()-1), queries(queries),
          seg(4 * (int)queries.size()), answer(queries.size()), dsu(n+1) {}

    static pair<int, int> normalize(int u, int v) {
        if (u > v) swap(u, v);
        return {u, v};
    }

    void add_interval(int v, int l, int r, int ql, int qr, Edge e) {
        if (qr < l || r < ql) return;
        if (ql <= l || r <= qr) {
            seg[v].push_back(e);
            return;
        }
        int mid = (l + r) / 2;
        add_interval(v*2, l, mid, ql, qr, e);
        add_interval(v*2+1, mid+1, r, ql, qr, e);
    }

    void build_intervals() {
        map<pair<int, int>, int> start_time;

        for (int t = 1; t <= q; t++) {
            auto [u, v] = normalize(queries[t].u, queries[t].v);
            if (queries[t].type == '+') {
                start_time[{u, v}] = t;
            } else if (queries[t].type == '-') {
                int start = start_time[{u, v}];
                start_time.erase({u, v});
                add_interval(1, 1, q, start, t-1, {u, v});
            }
        }

        for (auto& [e, start] : start_time) {
            add_interval(1, 1, q, start, q, {e.first, e.second});
        }
    }

    void dfs(int v, int l, int r) {
        int snap = dsu.snapshot();

        for (auto& e : seg[v]) {
            dsu.unionSets(e.u, e.v);
        }

        if (l == r) {
            if (queries[l].type == '?') {
                answer[l] = (dsu.isConnected(queries[l].u, queries[l].v) ? "YES" : "NO");
            }
        } else {
            int mid = (l + r) / 2;
            dfs(v*2, l, mid);
            dfs(v*2+1, mid+1, r);
        }

        dsu.rollback(snap);
    }

    vector<string> solve() {
        build_intervals();
        dfs(1, 1, q);
        vector<string> res;
        for (int t = 1; t <= q; t++) {
            if (queries[t].type == '?') {
                res.push_back(answer[t]);
            }
        }
        return res;
    }
};
