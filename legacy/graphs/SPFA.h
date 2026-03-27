#include <bits/stdc++.h>
#include <vector>

using namespace std;

// Shortest Path Faster Algorithm
class SPFA {
private:
    vector<vector<pair<int, int>>> adj_list;    // pair: {neighbor, weight}

public:
    static vector<int> spfa(const vector<vector<pair<int, int>>>& graph, int start) {
        int n = (int)(graph.size());
        vector<int> dist(n, INT_MAX);
        vector<bool> inQueue(n, false);
        vector<int> count(n, 0);
        queue<int> q;

        dist[start] = 0;
        q.push(start);
        inQueue[start] = true;
        count[start]++;

        while(!q.empty()) {
            int u = q.front();
            q.pop();
            inQueue[u] = false;

            for(const auto& [v, weight] : graph[u]) {

                if (dist[u] != INT_MAX && dist[v] > dist[u] + weight) {
                    dist[v] = dist[u] + weight;

                    if(!inQueue[v]) {
                        q.push(v);
                        inQueue[v] = true;
                        count[v]++;

                        if(count[v] > n) {
                            cout << "Negative cycle was detected!\n";
                            return vector<int>();
                        }
                    }
                }
            }
        }
    return dist;
    }
};


// testing example
int main(void) {
    vector<vector<pair<int, int>>> graph(5+1);
    graph[1].push_back({2, 6});
    graph[1].push_back({3, 7});
    graph[2].push_back({3, 8});
    graph[2].push_back({4, 5});
    graph[2].push_back({5, -4});
    graph[3].push_back({4, -3});
    graph[3].push_back({5, 9});
    graph[4].push_back({2, -2});
    graph[5].push_back({4, 7});

    vector<int> distances = SPFA::spfa(graph, 1);

    for(int i = 1; i <= 5; i++) {
        cout << distances[i] << " ";
    }


    return 0;
}
