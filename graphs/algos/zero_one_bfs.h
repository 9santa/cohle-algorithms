#include "../base.h"

template<class Cost, bool directed>
vl zero_one_bfs(const Graph<Cost, directed>& G, int src) {
    static_assert(std::is_integral<Cost>::value, "0-1 BFS expects integral edge costs");
    const int N = G.N;
    vl dist(N, infty<ll>);
    deque<int> dq;

    dist[src] = 0;
    dq.push_front(src);

    while (!dq.empty()) {
        int v = dq.front(); dq.pop_front();
        ll dv = dist[v];

        for (auto& e : G[v]) {
            int to = e.to;
            int w = (int)e.cost; // expected 0 or 1
            assert(w == 0 || w == 1);
            if (dist[to] > dv + w) {
                dist[to] = dv + w;
                (w == 0 ? dq.push_front(to) : dq.push_back(to));
            }
        }
    }
    return dist;
}
