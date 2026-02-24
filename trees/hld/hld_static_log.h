#include "../../graphs/base.h"
#include "../rooted_tree.h"
#include "../lca.h"

template<class Cost = int>
struct StaticLogHLD {
    using G = Graph<Cost, false>;

    const G& g;
    int n, root;

    RootedTree rt;
    LCA lca;

    // heavy-light
    vi siz, heavy, head, pathId, idxInPath;
    V<vi> paths;
    vi pathHead; // node id of head for each path

    // light subtrees size: bsize[v] = sz[v] - sz[heavy[v]]
    vi bsize;

    // Weighted-centroid BST over vertices (per heavy path)
    vi auxPar, auxL, auxR; // parent/left/right
    vl auxSum;  // subtree sum in BST
    vl val;     // current vertex values

    vi eid_to_child;

    StaticLogHLD(const G& graph, int r = 0) : g(graph), n(graph.N), root(r) {
        assert(g.built && "Graph must be built()");
        siz.assign(n, 0);
        heavy.assign(n, -1);
        head.assign(n, 0);
        pathId.assign(n, -1);
        idxInPath.assign(n, -1);
        bsize.assign(n, 1);

        auxPar.assign(n, -1);
        auxL.assign(n, -1);
        auxR.assign(n, -1);
        auxSum.assign(n, 0);
        val.assign(n, 0);
    }

    // ----- build helpers -----

    void build_heavy() {
        // subtree sizes bottom-up using rt.order
        for (int i = sz(rt.order)-1; i >= 0; i--) {
            int v = rt.order[i];
            siz[v] = 1;
            int mx = 0;
            heavy[v] = -1;

            for (auto& e : g[v]) {
                int to = e.to;
                if (rt.parent[to] != v) continue; // only children
                siz[v] += siz[to];
                if (siz[to] > mx) {
                    mx = siz[to];
                    heavy[v] = to;
                }
            }
        }

        for (int v = 0; v < n; v++) {
            if (heavy[v] == -1) bsize[v] = siz[v];
            else bsize[v] = siz[v] - siz[heavy[v]];
            if (bsize[v] <= 0) bsize[v] = 1;
        }
    }

    void build_paths() {
        paths.clear();
        pathHead.clear();

        vi stk;
        stk.push_back(root);

        while (!stk.empty()) {
            int h = stk.back();
            stk.pop_back();

            int pid = sz(paths);
            paths.push_back({});
            pathHead.push_back(h);

            for (int v = h; v != -1; v = heavy[v]) {
                head[v] = h;
                pathId[v] = pid;
                idxInPath[v] = sz(paths[pid]);
                paths[pid].push_back(v);

                // light children start new paths
                for (auto& e : g[v]) {
                    int to = e.to;
                    if (rt.parent[to] != v || to == heavy[v]) continue;

                    stk.push_back(to);
                }
            }
        }
    }

    // build BST for one path (vertices in pv[l..r], order is path order)
    int build_aux(const vi& pv, const vl& pref, int l, int r, int parent_node) {
        if (l > r) return -1;

        ll total = pref[r+1] - pref[l];
        ll half = (total + 1) / 2;

        // find first k in [l..r] such that weight(l..k) >= half
        int lo = l, hi = r, k = r;
        while (lo <= hi) {
            int mid = (lo + hi) >> 1;
            ll wleft = pref[mid+1] - pref[l];
            if (wleft >= half) {
                k = mid;
                hi = mid - 1;
            } else {
                lo = mid + 1;
            }
        }

        int v = pv[k];
        auxPar[v] = parent_node;

        int L = build_aux(pv, pref, l, k-1, v);
        int R = build_aux(pv, pref, k+1, r, v);
        auxL[v] = L;
        auxR[v] = R;

        auxSum[v] = val[v]
            + (L == -1 ? 0LL : auxSum[L])
            + (R == -1 ? 0LL : auxSum[R]);

        return v;
    }

    void build_all_aux() {
        // safety reset
        for (int v = 0; v < n; v++) {
            auxPar[v] = auxL[v] = auxR[v] = -1;
            auxSum[v] = val[v];
        }

        for (auto& pv : paths) {
            int m = sz(pv);
            vl pref(m+1, 0);
            for (int i = 0; i < m; i++) {
                pref[i+1] = pref[i] + (ll)bsize[pv[i]];
            }

            build_aux(pv, pref, 0, m-1, -1);
        }
    }

    // build everything.
    void build(const vl& a) {
        assert(sz(a) == n);
        val = a;
        rt = tree_dfs_root(g, root);
        lca.build(rt);

        build_heavy();
        build_paths();
        build_all_aux();

        // edge-id -> child vertex map (optional)
        eid_to_child.assign(g.M, -1);
        for (int v = 0; v < n; v++) {
            int eid = rt.parent_eid[v];
            if (eid != -1) eid_to_child[eid] = v;
        }
    }

    // ----- queries on centroid BST -----

    inline ll sum_left(int v) const { return (auxL[v] == -1 ? 0LL : auxSum[auxL[v]]); }

    // prefix sum on a heavy path from its head to vertex v (inclusive)
    ll prefix_sum_in_path(int v) const {
        ll res = sum_left(v) + val[v];
        int cur = v;
        while (auxPar[cur] != -1) {
            int p = auxPar[cur];
            if (auxR[p] == cur) {
                res += sum_left(p) + val[p];
            }
            cur = p;
        }
        return res;
    }

    // sum from root to vertex v (inclusive) using heavy-path jumps + centroid prefix sums
    ll root_sum(int v) const {
        ll res = 0;
        while (v != -1) {
            res += prefix_sum_in_path(v);
            int h = head[v];
            v = rt.parent[h];
        }
        return res;
    }

    // ----- public API -----

    void point_set_vertex(int v, ll x) {
        ll delta = x - val[v];
        val[v] = x;
        for (int cur = v; cur != -1; cur = auxPar[cur]) auxSum[cur] += delta;
    }

    void point_add_vertex(int v, ll delta) {
        val[v] += delta;
        for (int cur = v; cur != -1; cur = auxPar[cur]) auxSum[cur] += delta;
    }

    ll path_sum_vertex(int u, int v) const {
        int w = lca.lca(u, v);
        return root_sum(u) + root_sum(v) - 2LL * root_sum(w) + val[w];
    }

    // ----- edge mode: store each edge (parent[v]-v) at vertex v -----
    // val[root] must be 0 for pure edge sums

    void point_set_edge(int eid, ll x) {
        int v = eid_to_child[eid];
        assert(v != -1);
        point_set_vertex(v, x);
    }

    void point_add_edge(int eid, ll delta) {
        int v = eid_to_child[eid];
        assert(v != -1);
        point_add_vertex(v, delta);
    }

    ll path_sum_edge(int u, int v) const {
        int w = lca.lca(u, v);
        // with edge weights stored at child vertices, no +val[w] is needed
        return root_sum(u) + root_sum(v) - 2LL * root_sum(w);
    }
};

/* Example
static void example() {
    int n; cin >> n;
    Graph<int, false> g(n);
    for (int i = 0; i < n-1; i++) {
        int u, v; cin >> u >> v;
        g.add_edge(u, v);
    }
    g.build();

    vl a(n);
    for (int i = 0; i < n; i++) cin >> a[i];

    StaticLogHLD<int> ds(g, 0);
    ds.build(a);

    cout << ds.path_sum_vertex(u, v) << "\n";
    ds.point_add_vertex(x, +5);
}
*/
