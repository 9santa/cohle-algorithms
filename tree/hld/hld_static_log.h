#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * HLD variant with weighted-centroid BSTs over heavy paths.
 * Supports point updates and O(log n) path-sum queries on static trees.
 *
 * Uses a plain undirected adjacency list. Edge values, when needed, are stored
 * on the child endpoint of the rooted edge.
 */
struct StaticLogHLD {
    const vector<vector<int>>& g;
    int n = 0, root = 0;

    vector<int> parent, depth, order;

    // binary lifting for LCA
    int LOG = 0;
    vector<vector<int>> up;

    // heavy-light decomposition
    vector<int> siz, heavy, head, path_id, idx_in_path;
    vector<vector<int>> paths;
    vector<int> path_head;

    // light-subtree weight used to build the auxiliary BST on each heavy path
    vector<int> bsize;

    // weighted-centroid BST over each heavy path
    vector<int> aux_parent, aux_left, aux_right;
    vector<ll> aux_sum;
    vector<ll> val;

    explicit StaticLogHLD(const vector<vector<int>>& graph, int r = 0)
        : g(graph), n((int)graph.size()), root(r) {
        parent.assign(n, -1);
        depth.assign(n, 0);
        siz.assign(n, 0);
        heavy.assign(n, -1);
        head.assign(n, 0);
        path_id.assign(n, -1);
        idx_in_path.assign(n, -1);
        bsize.assign(n, 1);
        aux_parent.assign(n, -1);
        aux_left.assign(n, -1);
        aux_right.assign(n, -1);
        aux_sum.assign(n, 0);
        val.assign(n, 0);
    }

    void build(const vector<ll>& a) {
        assert((int)a.size() == n);
        val = a;
        build_parent_order();
        build_lca();
        build_heavy();
        build_paths();
        build_all_aux();
    }

private:
    void build_parent_order() {
        parent.assign(n, -1);
        depth.assign(n, 0);
        order.clear();
        order.reserve(n);

        vector<int> st = {root};
        parent[root] = root;

        while (!st.empty()) {
            int v = st.back();
            st.pop_back();
            order.push_back(v);

            for (int to : g[v]) {
                if (to == parent[v]) continue;
                parent[to] = v;
                depth[to] = depth[v] + 1;
                st.push_back(to);
            }
        }
    }

    void build_lca() {
        LOG = 1;
        while ((1 << LOG) <= max(1, n)) ++LOG;
        up.assign(LOG, vector<int>(n, root));
        up[0] = parent;

        for (int j = 1; j < LOG; ++j) {
            for (int v = 0; v < n; ++v) {
                up[j][v] = up[j - 1][up[j - 1][v]];
            }
        }
    }

    void build_heavy() {
        fill(siz.begin(), siz.end(), 1);
        fill(heavy.begin(), heavy.end(), -1);

        for (int i = n - 1; i >= 0; --i) {
            int v = order[i];
            int best_size = 0;
            for (int to : g[v]) {
                if (parent[to] != v) continue;
                siz[v] += siz[to];
                if (siz[to] > best_size) {
                    best_size = siz[to];
                    heavy[v] = to;
                }
            }
        }

        for (int v = 0; v < n; ++v) {
            bsize[v] = siz[v] - (heavy[v] == -1 ? 0 : siz[heavy[v]]);
        }
    }

    void build_paths() {
        paths.clear();
        path_head.clear();
        fill(path_id.begin(), path_id.end(), -1);

        vector<int> st = {root};
        while (!st.empty()) {
            int h = st.back();
            st.pop_back();

            int pid = (int)paths.size();
            paths.push_back({});
            path_head.push_back(h);

            for (int v = h; v != -1; v = heavy[v]) {
                head[v] = h;
                path_id[v] = pid;
                idx_in_path[v] = (int)paths[pid].size();
                paths[pid].push_back(v);

                for (int to : g[v]) {
                    if (parent[to] != v || to == heavy[v]) continue;
                    st.push_back(to);
                }
            }
        }
    }

    int build_aux(const vector<int>& path, const vector<ll>& pref, int l, int r, int par) {
        if (l > r) return -1;

        ll total = pref[r + 1] - pref[l];
        ll half = (total + 1) / 2;

        int lo = l, hi = r, k = r;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (pref[mid + 1] - pref[l] >= half) {
                k = mid;
                hi = mid - 1;
            } else {
                lo = mid + 1;
            }
        }

        int v = path[k];
        aux_parent[v] = par;
        aux_left[v] = build_aux(path, pref, l, k - 1, v);
        aux_right[v] = build_aux(path, pref, k + 1, r, v);

        aux_sum[v] = val[v];
        if (aux_left[v] != -1) aux_sum[v] += aux_sum[aux_left[v]];
        if (aux_right[v] != -1) aux_sum[v] += aux_sum[aux_right[v]];
        return v;
    }

    void build_all_aux() {
        fill(aux_parent.begin(), aux_parent.end(), -1);
        fill(aux_left.begin(), aux_left.end(), -1);
        fill(aux_right.begin(), aux_right.end(), -1);
        aux_sum = val;

        for (const auto& path : paths) {
            int m = (int)path.size();
            vector<ll> pref(m + 1, 0);
            for (int i = 0; i < m; ++i) pref[i + 1] = pref[i] + bsize[path[i]];
            build_aux(path, pref, 0, m - 1, -1);
        }
    }

    ll left_sum(int v) const {
        return aux_left[v] == -1 ? 0LL : aux_sum[aux_left[v]];
    }

public:
    int kth_ancestor(int v, int k) const {
        for (int j = 0; j < LOG; ++j) if (k >> j & 1) v = up[j][v];
        return v;
    }

    int lca(int a, int b) const {
        if (depth[a] < depth[b]) swap(a, b);
        a = kth_ancestor(a, depth[a] - depth[b]);
        if (a == b) return a;

        for (int j = LOG - 1; j >= 0; --j) {
            if (up[j][a] != up[j][b]) {
                a = up[j][a];
                b = up[j][b];
            }
        }
        return parent[a];
    }

    /** Sum from the head of v's heavy path through v, inclusive. */
    ll prefix_sum_in_path(int v) const {
        ll res = left_sum(v) + val[v];
        int cur = v;
        while (aux_parent[cur] != -1) {
            int p = aux_parent[cur];
            if (aux_right[p] == cur) res += left_sum(p) + val[p];
            cur = p;
        }
        return res;
    }

    /** Vertex-value sum from root through v, inclusive. */
    ll root_sum(int v) const {
        ll res = 0;
        while (true) {
            res += prefix_sum_in_path(v);
            int h = head[v];
            if (h == root) break;
            v = parent[h];
        }
        return res;
    }

    void point_set_vertex(int v, ll x) {
        ll delta = x - val[v];
        val[v] = x;
        for (int cur = v; cur != -1; cur = aux_parent[cur]) aux_sum[cur] += delta;
    }

    void point_add_vertex(int v, ll delta) {
        val[v] += delta;
        for (int cur = v; cur != -1; cur = aux_parent[cur]) aux_sum[cur] += delta;
    }

    ll path_sum_vertex(int u, int v) const {
        int w = lca(u, v);
        return root_sum(u) + root_sum(v) - 2LL * root_sum(w) + val[w];
    }

    /**
     * Edge mode: store the edge (parent[v], v) at vertex v; keep val[root] = 0.
     * These helpers update an edge by its child endpoint rather than by edge id.
     */
    void point_set_edge_to_parent(int child, ll x) {
        assert(child != root);
        point_set_vertex(child, x);
    }

    void point_add_edge_to_parent(int child, ll delta) {
        assert(child != root);
        point_add_vertex(child, delta);
    }

    ll path_sum_edge(int u, int v) const {
        int w = lca(u, v);
        return root_sum(u) + root_sum(v) - 2LL * root_sum(w);
    }
};
