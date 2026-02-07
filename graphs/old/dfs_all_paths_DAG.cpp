
vector<vector<int>> dfs_all_paths_iterative(vector<vector<int>>& graph, int start, int end) {
    int n = static_cast<int>(graph.size());
    vector<vector<int>> all_paths;

    stack<pair<int, vector<int>>> st;
    st.push({start, {start}});

    while(!st.empty()) {
        auto [node, path] = st.top();
        st.pop();

        if(node == end) {
            all_paths.push_back(path);
            continue;
        }

        for(auto v : graph[node]) {
            vector<int> new_path = path;
            new_path.push_back(v);
            st.push({v, new_path});
        }
    }

    return all_paths;
}


