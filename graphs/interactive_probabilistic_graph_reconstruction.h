#include "../header.h"

struct Edge {
    int u, v;
};

mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

int randBit() {
    return (int)(rng() & 1);
}

// Block edge e
void block_edge(int id) {
    cout << "1 " << id << "\n";
    cout.flush();
}

// Unblock edge e
void unblock_edge(int id) {
    cout << "2 " << id << "\n";
    cout.flush();
}

// Type 3 Query: oracle chooses random vertex s,
// we send vertex v, get YES/NO
bool query_vertex(int v) {
    cout << "3 " << v << "\n";
    cout.flush();

    string resp;
    if (!(cin >> resp)) exit(0);
    return (resp[0] == 'Y' || resp[0] == 'y');
}

// Test (probabilistic: ~100%) if edge e is in Spanning Tree
bool is_bridge_probabilistic(const Edge& e, int id, int T = 48) {
    int seenNo = false;
    for (int t = 0; t < T; t++) {
        int v = (randBit() ? e.u : e.v);
        bool ok = query_vertex(v);
        if (!ok) {
            seenNo = true;
            break;
        }
    }
    return seenNo; // bridge if atleast 1 "No"
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m; cin >> n >> m;

    vector<Edge> edges(m+1);
    for (int i = 1; i <= m; i++) {
        cin >> edges[i].u >> edges[i].v;
    }

    // Edges status
    vector<bool> inTree(m+1, false); // edge is in spannig tree
    vector<bool> isReal(m+1, false); // real or fake edge

    // Step 1: build spanning tree. For each edge: block it and check if it's a bridge
    const int T = 48;

    for (int id = 1; id <= m; id++) {
        const Edge& e = edges[id];
        block_edge(id);

        bool bridge = is_bridge_probabilistic(e, id, T);

        if (bridge) {
            // need this edge for connectivity -> part of the spanning tree
            inTree[id] = true;
            isReal[id] = true;
            unblock_edge(id);   // unblock edge back
        } else {
            inTree[id] = false;
            // isReal is unknown yet
        }
    }

    vector<vector<pair<int, int>>> adj(n+1);
    // adj[v] = list (to, edge_id) spanning tree
    for (int id = 1; id <= m; id++) {
        if (!inTree[id]) continue;
        int u = edges[id].u;
        int v = edges[id].v;
        adj[u].push_back({v, id});
        adj[v].push_back({u, id});
    }

    // Utility for spanning tree
    vector<int> parent(n+1, -1);
    vector<int> parentEdge(n+1, -1);
    vector<int> depth(n+1, 0);

    int root = 1;
    queue<int> q;
    parent[root] = 0;
    parentEdge[root] = -1;
    depth[root] = 0;
    q.push(root);

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (auto [to, eid] : adj[v]) {
            if (to == parent[v]) continue;
            parent[to] = v;
            parentEdge[to] = eid;
            depth[to] = depth[v] + 1;
            q.push(to);
        }
    }

    // Helper func: build path (edge id list) between u and v in spanning tree
    auto get_path_edges = [&](int u, int v) {
        vector<int> path;
        int x = u, y = v;
        while (x != y) {
            if (depth[x] > depth[y]) {
                path.push_back(parentEdge[x]);
                x = parent[x];
            } else {
                path.push_back(parentEdge[y]);
                y = parent[y];
            }
        }
        return path;
    };

    // Step 2: check the remaining edges real or fake

    for (int id = 1; id <= m; id++) {
        if (inTree[id]) continue;

        const Edge& e = edges[id];
        unblock_edge(id);

        // Path between u and v in spanning tree
        vector<int> path = get_path_edges(e.u, e.v);
        if (path.empty()) {
            // shoudn't happen if the spanning tree is correct
            isReal[id] = false;
            block_edge(id);
            continue;
        }

        int f_id = path[0];
        const Edge& f = edges[f_id];
        block_edge(f_id);

        // Test: is f bridge. If 'e' is real -> spanning + e create a cycle, and f is not a bridge
        // If 'e' is fake -> it doesn't participate in the real graph, and f remains a bridge
        bool f_is_bridge = is_bridge_probabilistic(f, f_id, T);

        unblock_edge(f_id);
        block_edge(id);

        if (!f_is_bridge) {
            isReal[id] = true;
        } else {
            isReal[id] = false;
        }
    }

    cout << "! ";
    for (int id = 1; id <= m; id++) {
        cout << (isReal[id] ? 1 : 0);
        if (id + 1 <= m) cout << " ";
    }
    cout << "\n";
    cout.flush();

    return 0;
}
