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

