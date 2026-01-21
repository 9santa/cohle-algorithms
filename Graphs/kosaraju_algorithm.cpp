
/**
 * Description: Kosaraju's Algorithm, DFS twice to generate
 * strongly connected components in topological order. a,b
 * in same component if both a to b and b to a paths exist.
 * Time: O(N+M)
 */
struct SCCk {
    int N; V<vi> adj, radj;
    vi topo, comp, comps; V<bool> vis;
    void init(int _N) {
        N = _N;
        adj.resize(N), radj.resize(N), comp.assign(N, -1), vis.resize(N);
    }
    void dfs(int x) {
        vis[x] = 1;
        for (auto y : adj[x]) if (!vis[y]) dfs(y);
        topo.pb(x);
    }
    void dfs2(int x, int v) {
        comp[x] = v;
        for (auto y : radj[x]) if (comp[y] == -1) dfs2(y, v);
    }
    void gen() {
        F0R(i, N) if (!vis[i]) dfs(i);
        reverse(all(topo));
        for (auto x : topo) if (comp[x] == -1) dfs2(x, x), comps.pb(x);
    }
};

