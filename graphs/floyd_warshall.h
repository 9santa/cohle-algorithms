
// Floyd-Warshall algorithm for shortest paths between any 2 vertices
const int INF = numeric_limits<int>::max();

/** Floyd-Warshall all-pairs shortest paths with path reconstruction. Space: O(n^2). */
class FloydWarshall {
private:
    vector<vector<int>> dist;    // Distance matrix
    vector<vector<int>> next;    // Next node in shortest path
    int n;                       // Number of vertices

public:
    /** Initializes n vertices using 1-based vertex ids. Time: O(n^2). */
    FloydWarshall(int vertices) : n(vertices) {
        // Initialize distance matrix with INF
        dist.resize(n+1, vector<int>(n+1, INF));
        next.resize(n+1, vector<int>(n+1, -1));

        // Distance from a vertex to itself is 0
        for (int i = 1; i <= n; i++) {
            dist[i][i] = 0;
            next[i][i] = i;
        }
    }

    // Add a directed edge from u to v with weight w
    /** Adds directed edge u -> v. Time: O(1). */
    void addEdge(int u, int v, int w) {
        dist[u][v] = w;
        next[u][v] = v;
    }

    // Add an undirected edge between u and v with weight w
    /** Adds undirected edge u-v. Time: O(1). */
    void addUndirectedEdge(int u, int v, int w) {
        addEdge(u, v, w);
        addEdge(v, u, w);
    }

    // Run Floyd-Warshall algorithm
    /** Computes all-pairs shortest paths. Time: O(n^3). */
    void compute() {
        for (int k = 1; k <= n; k++) {
            for (int i = 1; i <= n; i++) {
                for (int j = 1; j <= n; j++) {
                    // Check for overflow and valid paths
                    if (dist[i][k] != INF && dist[k][j] != INF && 
                        dist[i][k] + dist[k][j] < dist[i][j]) {

                        int oldDist = dist[i][j];
                        dist[i][j] = dist[i][k] + dist[k][j];
                        next[i][j] = next[i][k];

                    }
                }
            }

        }

        bool negativeCycleFound = false;
        for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (dist[i][k] != INF && dist[k][j] != INF &&
                        dist[i][k] + dist[k][j] < dist[i][j]) {
                        // Negative cycle detected
                        dist[i][j] = -INF;
                        next[i][j] = -1;
                        negativeCycleFound = true;
                    }
                }
            }
        }

        if (negativeCycleFound) {
            cout << "Negative cycles were detected in the graph!\n";
        } else {
            cout << "No negative cycles detected.\n";
        }
    }

    // Get shortest path distance from u to v
    /** Returns shortest-path distance from u to v. Time: O(1). */
    int getDistance(int u, int v) {
        return dist[u][v];
    }

    // Reconstruct shortest path from u to v
    /** Reconstructs a shortest path from u to v. Time: O(path length). */
    vector<int> getPath(int u, int v) {
        vector<int> path;

        // No path exists or negative cycle detected
        if (dist[u][v] == INF || dist[u][v] == -INF) {
            return path;
        }

        // Build path using next matrix
        int current = u;
        path.push_back(current);

        while (current != v) {
            if (next[current][v] == -1) {
                // No path exists
                return vector<int>();
            }
            current = next[current][v];
            path.push_back(current);
        }

        return path;
    }
};
