#pragma once
#include <bits/stdc++.h>
using namespace std;

// Edmonds-Karp: Ford-Fulkerson using BFS shortest augmenting paths.
// O(V E^2).
//
// Standard representation:
//   adj[v]        = vertices joined to v by a residual edge
//   cap[u][v]     = current residual capacity u -> v
// Parallel directed edges are merged by summing their capacities.
template<class Cap = long long>
struct EdmondsKarp {
    int n;
    vector<vector<int>> adj;
    vector<vector<Cap>> cap;
    Cap max_cap = 0;

    explicit EdmondsKarp(int n)
        : n(n), adj(n), cap(n, vector<Cap>(n, 0)) {}

    void add_edge(int u, int v, Cap c) {
        assert(0 <= u && u < n && 0 <= v && v < n);
        assert(c >= 0);
        if (c == 0) return;

        // Add the residual adjacency only the first time this unordered pair appears.
        if (cap[u][v] == 0 && cap[v][u] == 0) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        cap[u][v] += c;
        max_cap = max(max_cap, cap[u][v]);
    }

    void add_undirected_edge(int u, int v, Cap c) {
        assert(0 <= u && u < n && 0 <= v && v < n);
        assert(c >= 0);
        if (c == 0) return;

        if (cap[u][v] == 0 && cap[v][u] == 0) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        cap[u][v] += c;
        cap[v][u] += c;
        max_cap = max({max_cap, cap[u][v], cap[v][u]});
    }

private:
    Cap augment(int s, int t, Cap delta) {
        vector<int> parent(n, -1);
        parent[s] = s;

        queue<int> q;
        q.push(s);

        while (!q.empty() && parent[t] == -1) {
            int v = q.front();
            q.pop();

            for (int to : adj[v]) {
                if (parent[to] != -1 || cap[v][to] < delta) continue;
                parent[to] = v;
                q.push(to);
                if (to == t) break;
            }
        }

        if (parent[t] == -1) return 0;

        Cap pushed = numeric_limits<Cap>::max();
        for (int v = t; v != s; v = parent[v])
            pushed = min(pushed, cap[parent[v]][v]);

        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            cap[u][v] -= pushed;
            cap[v][u] += pushed;
        }
        return pushed;
    }

public:
    Cap max_flow(int s, int t) {
        assert(0 <= s && s < n && 0 <= t && t < n);
        if (s == t) return 0;

        Cap flow = 0;
        while (Cap pushed = augment(s, t, Cap(1))) flow += pushed;
        return flow;
    }

    // Capacity-scaling variant. Call on a fresh residual network.
    Cap max_flow_scaling(int s, int t) {
        assert(0 <= s && s < n && 0 <= t && t < n);
        if (s == t || max_cap == 0) return 0;

        Cap delta = 1;
        while (delta <= max_cap / 2) delta <<= 1;

        Cap flow = 0;
        for (; delta > 0; delta >>= 1)
            while (Cap pushed = augment(s, t, delta)) flow += pushed;
        return flow;
    }
};
