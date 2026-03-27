
class Dijkstra {
public:
    // pair: {dest, weight}
    vector<int> dijkstra(int start, const vector<vector<pair<int, int>>>& graph) {
        int n = (int)graph.size();
        vector<int> distance(n+1, INT_MAX);
        vector<bool> processed(n+1, false);

        auto compare = [](const pair<int, int>& left, const pair<int, int>& right) {
                                    return left.second > right.second;
                                    };
        priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(compare)> pq(compare);

        distance[start] = 0;
        pq.push({start, 0});

        while(!pq.empty()) {
            int u = pq.top().first;
            int dist_u = pq.top().second;
            pq.pop();

            if(processed[u]) continue;
            processed[u] = true;

            for(const auto& neighbor : graph[u]) {
                int v = neighbor.first;
                int weight = neighbor.second;

                if(distance[u] != INT_MAX && distance[u] + weight < distance[v]) {
                    distance[v] = distance[u] + weight;
                    pq.push({v, distance[v]});
                }
            }
        }

        return distance;
    }
};
