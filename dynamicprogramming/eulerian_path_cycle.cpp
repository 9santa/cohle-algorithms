#include <../header.h>
#include <type_traits>

/*
Eulerian path / cycle (uses every edge exactly once) using Hierholzer's algorithm

Undirected graph:
- Eulerian cycle: connected (ignoring isolated vertices) + all degrees even
- Eulerian path: connected (ignoring isolated vertices) + exactly 0 or 2 odd-degree vertices

Directed graph:
- Eulerian cycle: all vertices with nonzero degree in one SCC and in[v] == out[v] for all v
- Eulerian path: exactly one start with out=in+1, one end with in=out+1, others in==out, plus connectivity condition

Time: O(E)
*/

struct Euler {
    bool ok = false;
    bool is_cycle = false;
    vector<int> vertices; // vertex sequence of the trail/circuit (length = E+1 of ok)
};

static bool connected_undirected(int n, const vector<vector<int>>& g, const vector<int>& deg) {
    int start = -1;
    for (int i = 0; i < n; i++) if (deg[i] > 0) { start = i; break; }
    if (start == -1) return true; // no edges -> trivially conneceted

    vector<char> vis(n, 0);
    deque<int> dq;
    dq.push_back(start);
    vis[start] = 1;

    while (!dq.empty()) {
        int v = dq.front(); dq.pop_front();
        for (auto to : g[v]) if (!vis[to]) {
            vis[to] = 1;
            dq.push_back(to);
        }
    }

    for (int i = 0; i < n; i++) {
        if (deg[i] > 0 && !vis[i]) return false;
    }
    return true;
}

static bool directed_reachable_from_start(int n, const vector<vector<int>>& g, const vector<int>& indeg, const vector<int>& outdeg, int start) {
    // check all nonzero-degree vertices are reachable from start in directed graph
    vector<char> vis(n, 0);
    deque<int> dq;
    dq.push_back(start);
    vis[start] = 1;

    while (!dq.empty()) {
        int v = dq.front(); dq.pop_front();
        for (auto to : g[v]) if (!vis[to]) {
            vis[to] = 1;
            dq.push_back(to);
        }
    }

    for (int i = 0; i < n; i++) {
        if ((indeg[i] + outdeg[i]) > 0 && !vis[i]) return false;
    }
    return true;
}

// UNDIRECTED Euler trail/circuit
Euler euler_undirected(int n, const vector<pair<int, int>>& edges) {
    Euler res;
    int m = (int)edges.size();
    if (m == 0) { // empty trail
        res.ok = true;
        res.is_cycle = true;
        res.vertices = {0};
        return res;
    }

    vector<int> deg(n, 0);
    vector<vector<int>> g(n); // for connectivity check
    vector<vector<pair<int, int>>> adj(n); // (to, edgeId)

    for (int id = 0; id < m; id++) {
        auto [u, v] = edges[id];
        deg[u]++, deg[v]++;
        g[u].push_back(v);
        g[v].push_back(u);

        adj[u].push_back({v, id});
        adj[v].push_back({u, id});
    }

    if (!connected_undirected(n, g, deg)) {
        res.ok = false;
        return res;
    }

    vector<int> odd;
    for (int i = 0; i < n; i++) if (deg[i] & 1) odd.push_back(i);

    if (!(odd.size() == 0 || odd.size() == 2)) {
        res.ok = false;
        return res;
    }

    int start = -1;
    if (odd.size() == 2) start = odd[0];
    else {
        for (int i = 0; i < n; i++) if (deg[i] > 0) { start = i; break; }
    }

    vector<char> used(m, 0); // used edges
    vector<int> it(n, 0);
    stack<int> st;
    vector<int> path;

    st.push(start);
    while (!st.empty()) {
        auto v = st.top();

        while (it[v] < (int)adj[v].size() && used[adj[v][it[v]].second]) it[v]++;

        if (it[v] == (int)adj[v].size()) {
            // dead end, add to answer
            path.push_back(v);
            st.pop();
        } else {
            auto [to, id] = adj[v][it[v]];
            used[id] = 1;
            st.push(to);
        }
    }

    // path should have exactly E+1 vertices
    if ((int)path.size() != m+1) {
        res.ok = false;
        return res;
    }
    reverse(all(path));

    res.ok = true;
    res.is_cycle = (odd.size() == 0);
    res.vertices = std::move(path);
    return res;
}

// DIRECTED Euler trail/circuit
Euler euler_directed(int n, const vector<pair<int, int>>& edges) {
    Euler res;
    int m = (int)edges.size();
    if (m == 0) {
        res.ok = true;
        res.is_cycle = true;
        res.vertices = {0};
        return res;
    }

    vector<int> indeg(n, 0), outdeg(n, 0);
    vector<vector<int>> und(n);               // for connectivity check
    vector<vector<pair<int, int>>> adj(n);  // (to, edgeId)

    for (int id = 0; id < m; id++) {
        auto [u, v] = edges[id];
        outdeg[u]++, indeg[v]++;
        und[u].push_back(v), und[v].push_back(u);
        adj[u].push_back({v, id});
    }

    // underlying connectivity ignoring isolated vertices
    vector<int> deg(n, 0);
    for (int i = 0; i < n; i++) deg[i] = indeg[i] + outdeg[i];
    if (!connected_undirected(n, und, deg)) {
        res.ok = false;
        return res;
    }

    int start = -1, end = -1;
    for (int i = 0; i < n; i++) {
        int diff = outdeg[i] - indeg[i];
        if (diff == 1) {
            if (start != -1) { res.ok = false; return res; }
            start = i;
        } else if (diff == -1) {
            if (end != -1) { res.ok = false; return res; }
            end = i;
        } else if (diff != 0) {
            res.ok = false;
            return res;
        }
    }

    bool is_cycle = false;
    if (start == -1 && end == -1) {
        // Eulerian cycle case: pick any vertex with outgoing edge (dont matter)
        for (int i = 0; i < n; i++) if (outdeg[i] > 0) { start = i; break; }
        is_cycle = true;
    } else if (start != -1 && end != -1) {
        is_cycle = false;
    } else {
        res.ok = false;
        return res;
    }

    // directed reachability from chosen start (practical check)
    vector<vector<int>> g(n);
    for (auto [u, v] : edges) g[u].push_back(v);
    if (!directed_reachable_from_start(n, g, indeg, outdeg, start)) {
        res.ok = false;
        return res;
    }

    vector<char> used(m, 0);
    vector<int> it(n, 0);
    stack<int> st;
    vector<int> path;

    st.push(start);
    while (!st.empty()) {
        auto v = st.top();

        while (it[v] < (int)adj[v].size() && used[adj[v][it[v]].second]) it[v]++;

        if (it[v] == (int)adj[v].size()) {
            path.push_back(v);
            st.pop();
        } else {
            auto [to, id] = adj[v][it[v]];
            used[id] = 1;
            st.push(to);
        }
    }

    if ((int)path.size() != m+1) {
        res.ok = false;
        return res;
    }
    reverse(all(path));

    res.ok = true;
    res.is_cycle = is_cycle;
    res.vertices = std::move(path);
    return res;
}
