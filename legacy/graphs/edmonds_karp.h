
// infinity
constexpr int INF = 1e9;

int N;                          // number of vertices
vector<vector<int>> capacity;   // capacities matrix
vector<vector<int>> flow;       // current flow matrix
vector<int> parent;             // to store the path
int source, sink;               // source and sink vertices numbers

int bfs() {
    parent.assign(N, -1);
    parent[source] = -2;    // mark source as visited
    queue<pair<int, int>> q;
    q.push({source, INF});

    while (!q.empty()) {
        int v = q.front().first;
        int current_flow = q.front().second;
        q.pop();

        for (int u = 0; u < N; u++) {
            int residual_capacity = capacity[v][u] - flow[v][u];
            if (parent[u] == -1 && residual_capacity > 0) {
                parent[u] = v;
                int new_flow = min(current_flow, residual_capacity);
                if (u == sink) {
                    return new_flow;
                }
                q.push({u, new_flow});
            }
        }
    }

    return 0;
}

int max_flow() {
    int total_flow = 0;
    flow.assign(N, vector<int>(N, 0));

    while (true) {
        int pushed = bfs();
        if (pushed == 0) break;

        total_flow += pushed;
        int current_vertex = sink;
        while (current_vertex != source) {
            int prev_vertex = parent[current_vertex];
            flow[prev_vertex][current_vertex] += pushed;
            flow[current_vertex][prev_vertex] -= pushed;
            current_vertex = prev_vertex;
        }
    }

    return total_flow;
}


signed main(void) {
    N = 10;
    source = 0;
    sink = 9;

    capacity = {
    //   0   1   2   3   4   5   6   7   8   9
      {  0, 11,  0, 10,  0,  0,  0,  0,  0,  0 }, // 0
      {  0,  0,  5,  0, 24,  4,  0,  0,  0,  0 }, // 1
      {  0,  0,  0,  0, 14,  0, 13,  0,  0, 15 }, // 2
      {  0,  0, 15,  0,  0,  0,  0,  4,  0,  0 }, // 3
      {  0,  0,  0,  0,  0, 11,  0,  0, 23,  0 }, // 4
      {  0,  0, 23,  0,  0,  0,  0,  0,  0,  0 }, // 5
      {  0,  0,  0,  0,  0,  0,  0,  0,  0, 11 }, // 6
      {  0,  0,  1,  0,  0,  0,  0,  0,  0,  1 }, // 7
      {  0,  0,  0,  0,  0, 12,  0, 13,  0,  2 }, // 8
      {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }  // 9
    };

    cout << "Max flow: " << max_flow() << "\n";



    return 0;
}
