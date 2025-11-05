#include <bits/stdc++.h>
#include <vector>

using namespace std;


vector<vector<int>> kosaraju(const vector<vector<int>>& graph) {
    // build reversed edges graph
    int n = (int)graph.size() - 1;
    vector<vector<int>> inverted(n + 1);
    for(int i = 1; i <= n; i++) {
        for(auto u : graph[i]) {
            inverted[u].push_back(i);
        }
    }

    vector<bool> visited(n+1, false);
    vector<int> order;

    function<void(int)> dfs1 = [&](int u) {
        visited[u] = true;
        for(auto v : graph[u]) {
            if(!visited[v]) dfs1(v);
        }
        order.push_back(u);
    };

    for(int i = 1; i <= n; i++) {
        if(!visited[i]) {
            dfs1(i);
        }
    }

    visited.assign(n+1, false);
    vector<vector<int>> sccs;

    function<void(int, vector<int>&)> dfs2 = [&](int u, vector<int>& comp) {
        visited[u] = true;
        for(auto v : inverted[u]) {
            if(!visited[v]) {
                dfs2(v, comp);
            }
        }
        comp.push_back(u);
    };

    for(int i = (int)order.size() - 1; i >= 0; i--) {
        int u = order[i];
        if(!visited[u]) {
            vector<int> comp;
            dfs2(u, comp);
            sccs.push_back(comp);
        }
    }


    return sccs;
}


// tests
int main(void) {
    ios::sync_with_stdio(0);
    cin.tie(0);


    vector<vector<int>> graph0 = {
        {},
        {2, 6},
        {3, 4},
        {1, 7},
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


    vector<vector<int>> sccs = kosaraju(graph0);
    for(int i = 0; i < sccs.size(); i++) {
        for(auto v : sccs[i]) {
            cout << v << " ";
        }
        cout << "\n";
    }


    return 0;
}
