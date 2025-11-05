#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>

using namespace std;

class EulerCircuitFinder {
    int V;
    vector<vector<int>> adj;
    vector<int> edgePtr;
    vector<vector<bool>> usedEdge;  // used[u][v] for edge u-v

public:
    EulerCircuitFinder(int vertices) : V(vertices), adj(vertices), edgePtr(vertices, 0), usedEdge(vertices, vector<bool>(vertices, false)) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    vector<int> findEulerCircuit() {
        vector<int> circuit;
        stack<int> st;

        // Find start vertex (has edges)
        int start = 0;
        for (int i = 0; i < V; i++) {
            if (!adj[i].empty()) {
                start = i;
                break;
            }
        }

        st.push(start);

        while (!st.empty()) {
            int v = st.top();

            // find next unmarked edge from v
            while (edgePtr[v] < (int)adj[v].size() && usedEdge[v][adj[v][edgePtr[v]]]) {
                edgePtr[v]++;
            }

            if (edgePtr[v] < (int)adj[v].size()) {
                // found unmarked edge v-u
                int u = adj[v][edgePtr[v]];
                edgePtr[v]++;

                // mark[vu]
                usedEdge[v][u] = true;
                usedEdge[u][v] = true;

                // dfs(u) - push to stack
                st.push(u);
            } else {
                // no more edges from v
                st.pop();
                if (!st.empty()) {
                    circuit.push_back(v);
                }
            }
        }

        return circuit;
    }
};



signed main(void) {
    
    vector<pair<int, int>> edges = {
        {0,1}, {1,2}, {2,3}, {3,0}, {0,2}
    };

    EulerCircuitFinder g(4);
    for (auto [u, v] : edges) g.addEdge(u, v);

    vector<int> circuit = g.findEulerCircuit();
    for (auto v : circuit) cout << v << " ";
    cout << "\n";


    return 0;
}
