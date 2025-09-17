#include <bits/stdc++.h>


using namespace std;

// assuming graph is adjacency list
vector<int> bfs_shortest_path(const vector<vector<int>>& graph, int start, int end) {
    int n = static_cast<int>(graph.size());
    vector<bool> visited(n, false);
    vector<int> parent(n, -1);
    queue<int> q;

    q.push(start);
    visited[start] = true;

    while(!q.empty()) {
        int cur = q.front();
        q.pop();

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
                q.push(v);
            }
        }
    }

    return {}; // no path found
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
