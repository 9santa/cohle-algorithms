#include <bits/stdc++.h>

using namespace std;

vector<bool> visited; // keeps track of which vertices are already visited

// runs depth first search starting at vertex v.
// each visited vertex is appended to the output vector when dfs leaves it.
void dfs(int v, vector<vector<int>> const& adj, vector<int> &output) {
    visited[v] = true;
    for (auto u : adj[v])
        if (!visited[u])
            dfs(u, adj, output);
    output.push_back(v);
}

// input: adj -- adjacency list of G
// output: components -- the strongy connected components in G
// output: adj_cond -- adjacency list of G^SCC (by root vertices)
void strongly_connected_components(vector<vector<int>> const& adj,
                                  vector<vector<int>> &components,
                                  vector<vector<int>> &adj_cond) {
    int n = adj.size();
    components.clear(), adj_cond.clear();

    vector<int> order; // will be a sorted list of G's vertices by exit time

    visited.assign(n, false);

    // first series of depth first searches
    for (int i = 0; i < n; i++)
        if (!visited[i])
            dfs(i, adj, order);

    // create adjacency list of G^T
    vector<vector<int>> adj_rev(n);
    for (int v = 0; v < n; v++)
        for (int u : adj[v])
            adj_rev[u].push_back(v);

    visited.assign(n, false);
    reverse(order.begin(), order.end());

    vector<int> roots(n, 0); // gives the root vertex of a vertex's SCC
    int comp_id = 0;

    // second series of depth first searches
    for (auto v : order)
        if (!visited[v]) {
            std::vector<int> component;
            dfs(v, adj_rev, component);
            components.push_back(component);
            int root = *min_element(begin(component), end(component));
            for (auto u : component)
                roots[u] = comp_id;
            comp_id++;
        }

    // add edges to condensation graph
    adj_cond.assign(n, {});
    for (int v = 0; v < n; v++)
        for (auto u : adj[v])
            if (roots[v] != roots[u])
                adj_cond[roots[v]].push_back(roots[u]);
}


int main(void) {
    ios::sync_with_stdio(0);
    cin.tie(0);


    vector<vector<int>> graph0 = {
        {},
        {2, 6},
        {3, 4},
        {7, 1},
        {5},
        {4},
        {3, 7},
        {5}
    };

    vector<vector<int>> graph1 = {
        {},           // 0 unused (1-indexed)
        {2},          // 1 -> 2
        {3, 4},       // 2 -> 3, 2 -> 4
        {1},          // 3 -> 1
        {5},          // 4 -> 5
        {6},          // 5 -> 6
        {4, 7},       // 6 -> 4, 6 -> 7
        {8},          // 7 -> 8
        {9},          // 8 -> 9
        {7}           // 9 -> 7
    };

    vector<vector<int>> graph2 = {
        {},           // 0 unused
        {2},          // 1 → 2
        {3},          // 2 → 3  
        {1},          // 3 → 1 (cycle: 1-2-3)
        {1, 5},       // 4 → 1, 4 → 5
        {6},          // 5 → 6
        {4},          // 6 → 4 (cycle: 4-5-6)
        {6, 8},       // 7 → 6, 7 → 8
        {9},          // 8 → 9
        {7}           // 9 → 7 (cycle: 7-8-9)
    };

    vector<vector<int>> graph3 = {
        {},           // 0 unused (1-indexed)
        {2, 3},       // 1 -> 2, 1 -> 3
        {4},          // 2 -> 4
        {2},          // 3 -> 2
        {5, 6},       // 4 -> 5, 4 -> 6
        {7},          // 5 -> 7
        {5},          // 6 -> 5 (creates cycle 5-6)
        {8},          // 7 -> 8
        {9},          // 8 -> 9
        {7}           // 9 -> 7 (creates cycle 7-8-9)
    };


    vector<vector<int>> components;
    vector<vector<int>> adj_cond;
    strongly_connected_components(graph0, components, adj_cond);

    for(int i = 0; i < components.size(); i++) {
        for(auto v : components[i]) {
            cout << v << " ";
        }
        cout << "\n";
    }


    return 0;
}
