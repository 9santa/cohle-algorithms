
// finds shortest single source shortest path if weights are 0/1
// O(E). adjacency list. pair: {neighbor, weight}
vector<int> zero_one_bfs(const vector<vector<pair<int, int>>>& graph, int root) {
    int n = static_cast<int>(graph.size());
    vector<int> dist(n, INT_MAX);
    deque<int> dq;

    dq.push_front(root);
    dist[root] = 0;

    while(!dq.empty()) {
        int cur = dq.front();
        dq.pop_front();

        for(auto [v, weight] : graph[cur]) {
            if(dist[v] > dist[cur] + weight) {
                dist[v] = dist[cur] + weight;

                if(weight == 0) {
                    dq.push_front(v);
                } else {
                    dq.push_back(v);
                }
            }
        }
    }

    return dist;
}
