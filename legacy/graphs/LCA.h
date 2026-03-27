#include "../header.h"

// binary lifting through dfs
class LCA {
    int n, LOG;
    vector<vector<int>> g;

    int timer;
    vector<int> tin, tout;
    vector<vector<int>> up;

public:
    void dfs(int v, int p) {
        tin[v] = timer++;
        up[v][0] = p;
        for (int i = 1; i <= LOG; i++) {
            if (up[v][i-1] != -1)
                up[v][i] = up [up[v][i-1]] [i-1];
            else
                up[v][i] = -1;
        }

        for (int u : g[v]) {
            if (u != p) dfs(u, v);
        }

        tout[v] = timer++;
    }

    inline bool is_ancestor(int u, int v) {
        return tin[u] <= tin[v] && tout[u] >= tout[v];
    }

    inline int lca(int u, int v) {
        if (is_ancestor(u, v)) return u;
        if (is_ancestor(v, u)) return v;
        for (int i = LOG; i >= 0; i--) {
            if (!is_ancestor(up[u][i], v)) {
                u = up[u][i];
            }
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

    void init(int _n, int root) {
        n = _n;
        tin.resize(n);
        tout.resize(n);
        timer = 0;
        LOG = ceil(log2(n));
        up.resize(n, vector<int>(LOG+1));
        dfs(root, -1);
    }
};


// binary lifting if the tree nodes are ordered nicely (root is 0, children 1, 2.. etc)
class LCA1 {
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
