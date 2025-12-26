#include <bits/stdc++.h>
#include <vector>

using namespace std;

constexpr int WHITE = 0;
constexpr int GREY = 1;
constexpr int BLACK = 2;


std::vector<int> dfs(const std::vector<vector<int>>& graph, int start) {
    std::vector<int> visited(graph.size(), WHITE);
    std::vector<int> traversed_path;

    visited[start] = GREY;
    std::stack<int> st;
    st.push(start);

    while(!st.empty()) {
        int cur = st.top();
        st.pop();

        if(visited[cur] == GREY) {
            traversed_path.push_back(cur + 1);

            for(auto it : graph[cur]) {
                st.push(it);
                if(visited[it] != BLACK) {
                    visited[it] = GREY;
                }
            }
            visited[cur] = BLACK;   // Node has been explored
        }
    }
    return traversed_path;
}


// find path from start to end
vector<int> find_path_dfs(int start, int end, const vector<vector<int>>& graph) {
    int n = static_cast<int>(graph.size());
    vector<bool> visited(n, false);
    vector<int> parent(n, -1);
    stack<int> st;

    st.push(start);
    visited[start] = true;

    while(!st.empty()) {
        int cur = st.top();
        st.pop();

        if(cur == end) {
            // reconstruct path
            vector<int> path;
            for(int at = end; at != -1; at = parent[at]) {
                path.push_back(at);
            }
            reverse(path.begin(), path.end());
            return path;
        }

        for(auto v : graph[cur]) {
            if(!visited[v]) {
                visited[v] = true;
                parent[v] = cur;
                st.push(v);
            }
        }
    }

    return {};  // no path found
}

// recursive dfs: find simple path from u to target in the tree
bool find_path_tree(int u, int p, int target) {
    if (u == target) {
        path.pb(u);
        return true;
    }

    for (auto v : g[u]) {
        if (v == p) continue;
        if (dfs1(v, u, target)) {
            path.pb(u);
            return true;
        }
    }
    return false;
}

// recursive dfs: find simple paths between every 2 nodes in the tree
bool dfs(int current, int target, int parent, vector<int>& parentMap) {
        parentMap[current] = parent;

        if (current == target) {
            return true;
        }

        for (int neighbor : adj[current]) {
            if (neighbor != parent) {
                if (dfs(neighbor, target, current, parentMap)) {
                    return true;
                }
            }
        }

        return false;
    }
