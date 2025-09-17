#include <bits/stdc++.h>
#include <climits>


using namespace std;

// adjacency list. pair: {neighbor, weight}
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


int main(void)
{
    clock_t start = clock();
    ios::sync_with_stdio(0);
    cin.tie(0);



    double run_time = (double)(clock() - start) / CLOCKS_PER_SEC;
	cerr << "\nRun Time : " << std::fixed << std::setprecision(6) << run_time << " seconds\n";

    return 0;
}
