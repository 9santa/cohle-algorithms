#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Rollback DSU for offline dynamic connectivity. Space: O(n + number of unions). */
struct RollbackDSU {
private:
    struct Change {
        int a, b;
        int size_a_before;
        int sets_before;
    };

    int n;
    int set_count;
    vector<int> parent;
    vector<int> set_size;
    vector<Change> history;

public:
    RollbackDSU(int size) {
        build(size);
    }

    /** Initializes n singleton sets. Time: O(n). */
    void build(int size) {
        parent.resize(size);
        iota(parent.begin(), parent.end(), 0);
        set_size.assign(n, 1);
        set_count = size;
    }

    /** Returns the representative of x without path compression. Time: O(log n) with union by size. */
    int find(int x) const {
        while (parent[x] != x) x = parent[x];
        return x;
    }

    /** Merges sets and stores enough information to rollback. Time: O(log n). */
    bool unionSets(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            history.push_back({-1, -1, -1, set_count});
            return false;
        }

        if (set_size[x] < set_size[y]) swap(x, y);
        history.push_back({x, y, set_size[x], set_count});

        parent[y] = x;
        set_size[x] += set_size[y];
        set_count--;
        return true;
    }

    /** Reverts the most recent union attempt. Time: O(1). */
    void rollback() {
        auto ch = history.back();
        history.pop_back();
        if (ch.a == -1) return; // dummy change, nothing united

        parent[ch.b] = ch.b;
        set_size[ch.a] = ch.size_a_before;
        set_count = ch.sets_before;
    }

    /** Returns the current rollback stack size. Time: O(1). */
    int snapshot() const {
        return (int)history.size();
    }

    /** Rolls back to a previous snapshot. Time: O(number of reverted operations). */
    void rollback(int snap) {
        while ((int)history.size() > snap) {
            rollback();
        }
    }

    /** Returns whether x and y are connected. Time: O(log n). */
    bool isConnected(int x, int y) const {
        return find(x) == find(y);
    }

    /** Returns the size of the set containing x. Time: O(log n). */
    int size(int x) { return set_size[find(x)]; }
};


/** Undirected edge event for offline dynamic connectivity. */
struct Edge {
    int u, v;
};

/** Query event: add edge, remove edge, or ask connectivity. */
struct Query {
    char type; // '+', '-', '?'
    int u, v;
};

/**
* Offline dynamic connectivity with rollback DSU and a segment tree over time.
* Time: O((m log q) log n + q log n), where m is active edge intervals.
* Space: O(n + m log q + q).
*/
struct OfflineDynamicConnectivity {
    int n, q;
    vector<Query> queries;
    vector<vector<Edge>> seg;
    vector<string> answer;
    RollbackDSU dsu;

    OfflineDynamicConnectivity(int n_, const vector<Query>& queries)
        : n(n_), q((int)queries.size()-1), queries(queries),
          seg(4 * (int)queries.size()), answer(queries.size()), dsu(n+1) {}

    /** Returns an undirected edge key with smaller endpoint first. Time: O(1). */
    static pair<int, int> normalize(int u, int v) {
        if (u > v) swap(u, v);
        return {u, v};
    }

    /** Adds edge e to all segment-tree nodes covering [ql, qr]. Time: O(log q). */
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

    /** Converts add/remove events into active intervals. Time: O(q log q). */
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

    /** DFS over the time segment tree while rolling DSU state back. Time: O((m log q + q) log n). */
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

    /** Answers all connectivity queries in input order. Time: O((m log q + q) log n). */
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
