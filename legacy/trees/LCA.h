#include "../../header.h"

// binary lifting via parent & depth arrays
struct LCA {
    int n, LOG;
    vector<int> depth, parent;
    vector<vector<int>> up;

    LCA() : n(0), LOG(0) {}
    LCA(const vector<int>& par, const vector<int>& dep) { build(par, dep); }

    void build(const vector<int>& par, const vector<int>& dep) {
        parent = par;
        depth = dep;
        n = sz(parent);
        LOG = 32 - __builtin_clz(n);
        up.assign(LOG+1, vector<int>(n, -1)); // up[level][vertex]
        for (int v = 0; v < n; v++) up[0][v] = parent[v];
        for (int j = 1; j <= LOG; j++) {
            for (int v = 0; v < n; v++) {
                int mid = up[j-1][v];
                up[j][v] = (mid == -1 ? -1 : up[j-1][mid]);
            }
        }
    }

    int lift(int v, int k) const {
        for (int j = 0; j <= LOG; j++) {
            if (k & (1 << j)) v = (v == -1 ? -1 : up[j][v]);
        }
        return v;
    }

    int lca(int u, int v) const {
        if (depth[u] < depth[v]) swap(u, v);
        u = lift(u, depth[u] - depth[v]);
        if (u == v) return u;
        for (int j = LOG; j >= 0; j--) {
            if (up[j][u] != up[j][v]) {
                u = up[j][u];
                v = up[j][v];
            }
        }
        return parent[u];
    }
};


// binary lifting through dfs
struct LCA1 {
    int n;
    int LOG;
    vector<vector<int>> up;
    vector<vector<int>> g;
    int timer;
    vector<int> tin, tout;
    vector<int> order;
    vector<int> parent;

    LCA1() : n(0), LOG(0) {}
    LCA1(const vector<vector<int>>& G) { build(G); }

    void build(const vector<vector<int>>& G, int root = 0) {
        g = G;
        n = sz(g);
        LOG = 32 - __builtin_clz(n);
        timer = 0;
        tin.resize(n);
        tout.resize(n);
        up.assign(n, vi(LOG+1, -1));
        order.reserve(n);
        parent.resize(n);
        dfs(root, -1);
    }

    void dfs(int u, int p) {
        order.push_back(u);
        parent[u] = p;
        tin[u] = timer++;
        up[u][0] = p;

        for (int i = 1; i <= LOG; i++) {
            if (up[u][i-1] != -1) {
                up[u][i] = up[up[u][i-1]][i-1];
            } else {
                up[u][i] = -1;
            }
        }

        for (auto v : g[u]) {
            if (v != p) dfs(v, u);
        }

        tout[u] = timer++;
    }

    bool is_ancestor(int u, int v) {
        return tin[u] <= tin[v] && tout[u] >= tout[v];
    }

    int lca(int u, int v) {
        if (is_ancestor(u, v)) return u;
        if (is_ancestor(v, u)) return v;
        for (int i = LOG; i >= 0; i--) {
            if (up[u][i] != -1 && !is_ancestor(up[u][i], v)) u = up[u][i];
        }
        return up[u][0];
    }

    inline int getKthAncestor(int v, int k) {
        for (int j = 0; j <= LOG; j++) {
            if (k & (1 << j)) {
                v = up[v][j];
                if (v == -1) return -1; // no k-th parent exists
            }
        }
        return v;
    }

};



// binary lifting if the tree nodes are ordered nicely (root is 0, children 1, 2.. etc)
class LCA2 {
    vector<vector<int>> up;
    vector<int> depth;
    int LOG;
public:
    void binary_lifting(int n, vector<int>& parent) {
        LOG = 0;
        while ((1 << LOG) <= n) LOG++;

        up.resize(n, vector<int>(LOG+1));
        depth.resize(n);
        parent[0] = 0;
        for (int v = 0; v < n; v++) {
            up[v][0] = parent[v];
            if (v != 0) {
                depth[v] = depth[parent[v]] + 1;
            }
            for (int j = 1; j <= LOG; j++) {
                up[v][j] = up[up[v][j-1]][j-1];
            }
        }
    }

    int getKthAncestor(int v, int k) {
        if (depth[v] < k) return -1; // no k-th ancestor exists

        for (int j = 0; j <= LOG; j++) {
            if (k & (1 << j)) {
                v = up[v][j];
            }
        }
        return v;
    }
};
