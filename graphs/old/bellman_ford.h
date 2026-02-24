#include <algorithm>
#include <bits/stdc++.h>
#include <climits>

using namespace std;

vector<tuple<int, int, int>> graph; // tuple: (a, b, weight)

class BellmanFord {
private:
    vector<int> distance;
    vector<int> predecessor;

public:
    pair<bool, bool> bellmanford(int start, int n, const vector<tuple<int, int, int>>& graph) {
        distance.assign(n+1, INT_MAX);
        predecessor.assign(n+1, -1);

        distance[start] = 0;
        bool updated = false;

        // Relax all edges n-1 times
        for(int i = 1; i <= n-1; i++) {
            updated = false;
            for(const auto&e : graph) {
                int u, v, weight;
                tie(u, v, weight) = e;
                // distance[v] = min(distance[u] + weight, distance[v]);

                // Check for overflow and relax edge
                if(distance[u] != INT_MAX && distance[u] + weight < distance[v]) {
                    distance[v] = distance[u] + weight;
                    predecessor[v] = u;
                    updated = true; // means we improved the distance
                }
            }

            // Early termination if no updates (just for optimization, not necessary)
            if(!updated) break;

        }

        // Check for negative cycle, by relaxing one more time
        bool has_negative_cycle = false;
        for(const auto& e : graph) {
            int u, v, weight;
            tie(u, v, weight) = e;

            if(distance[u] != INT_MAX && distance[u] + weight < distance[v]) {
                has_negative_cycle = true;
                break;
            }
        }

        return {true, has_negative_cycle};
    }

    // Get distance from start node
    int get_distance(int node) const {
        return distance[node];
    }

    vector<int> reconstruct_path(int target) {
        vector<int> path;
        if(distance[target] == INT_MAX) return path;    // no path

        for(int cur = target; cur != -1; cur = predecessor[cur]) {
            path.push_back(cur);
        }

        reverse(path.begin(), path.end());
        return path;
    }
};
