#include "../base.h"

// Returns {ok, color}, where color[v] is 0/1 if ok empty vector otherwise
// Intended for undirected graphs
template<class Cost>
pair<bool, vector<int>> bipartite_coloring(const Graph<Cost, false>& G) {
    const int N = G.N;
    vi color(N, -1);

    for (int i = 0; i < N; i++) {
        if (color[i] != -1) continue;
        queue<int> q;
        q.push(i);
        color[i] = 0;

        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (auto& e : G[v]) {
                int to = e.to;
                if (color[to] == -1) {
                    color[to] = color[v] ^ 1;
                    q.push(to);
                } else if (color[to] == color[v]) {
                    return {false, {}};
                }
            }
        }
    }
    return {true, color};
}
