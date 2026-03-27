
// DFS Simple Recursive Implementation
void DFS(int root, const vector<vector<int>> &graph, vector<bool> &visited) {
    if (visited[root])
        return;
    visited[root] = true;

    for (auto &v : graph[root]) {
        if (!visited[v]) {
            DFS(v, graph, visited);
        }
    }
}
