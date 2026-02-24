#include "../../datastructures/splay/acted_lazy_implicit_splay.h"
#include "../../alg/acted_monoid/sum_add.h"

struct HLD_Splay {
    int n;
    V<vi> g;
    vi parent, depth, heavy, head, siz;
    vl value;

    // per-path storage
    V<vi> pathNodes;    // nodes per path (head is index 0)
    vi pathId;          // which path node is on
    vi idxInPath;       // index within that path
    vi heads;           // list of head nodes

    // splay per path
    using AM = ActedMonoid_Sum_Add<ll>;
    using Splay = SplayTree_ActedMonoid<AM>;
    vector<Splay> trees;

    HLD_Splay(int n_) : n(n_), g(n), parent(n, -1), depth(n, 0), heavy(n, -1), head(n, 0),
                        siz(n, 0), value(n, 0), pathId(n, -1), idxInPath(n, -1) {}

    int dfs1(int u, int p) {
        parent[u] = p;
        heavy[u] = -1;
        siz[u] = 1;
        int mx = 0;
        for (auto v : g[u]) {
            if (v == p) continue;
            depth[v] = 1 + depth[u];
            int sub = dfs1(v, u);
            siz[u] += sub;
            if (sub > mx) mx = sub, heavy[u] = v;
        }
        return siz[u];
    }

    void dfs2(int u, int h, int pid) {
        head[u] = h;
        pathId[u] = pid;
        idxInPath[u] = sz(pathNodes[pid]);
        pathNodes[pid].push_back(u);

        if (heavy[u] != -1) dfs2(heavy[u], h, pid);
        for (auto v : g[u]) if (v != parent[u] && v != heavy[u]) {
            int newPid = sz(pathNodes);
            pathNodes.push_back({});
            heads.push_back(v);
            dfs2(v, v, newPid);
        }
    }

    void build(const V<vi>& G, int root = 0) {
        g = G;
        depth[root] = 0;
        parent[root] = -1;
        dfs1(root, -1);
        pathNodes.clear(), heads.clear(), pathNodes.push_back({}), heads.push_back(root);
        dfs2(root, root, 0);
        trees.clear();
        trees.reserve(pathNodes.size());

        for (int pid = 0; pid < sz(pathNodes); pid++) {
            vl arr;
            arr.reserve(pathNodes[pid].size());
            for (auto u : pathNodes[pid]) arr.push_back(value[u]);
            trees.emplace_back(arr);
        }
    }

    // amort O(log n)
    void path_update(int u, int v, ll delta) {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            int pid = pathId[u];
            // segment is head[u]..u => indices [0..idxInPath[u]]
            trees[pid].update_range(0, idxInPath[u] + 1, delta);
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) swap(u, v);
        int pid = pathId[u];
        int l = idxInPath[u], r = idxInPath[v];
        trees[pid].update_range(l, r + 1, delta);
    }

    // amort O(log n)
    ll path_prod(int u, int v) {
        ll res = 0;
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            int pid = pathId[u];
            res += trees[pid].prod(0, idxInPath[u] + 1);
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) swap(u, v);
        int pid = pathId[u];
        int l = idxInPath[u], r = idxInPath[v];
        res += trees[pid].prod(l, r + 1);
        return res;
    }
};
