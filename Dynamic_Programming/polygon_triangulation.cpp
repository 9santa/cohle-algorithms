#include "../header.h"

struct Point {
    double x, y;
};

double dist(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return sqrt(dx*dx + dy*dy);
}

// Finds minimal sum of perimeters of triangles. O(n^3)
// dp[i][j] - min cost of triangulation for vertices i..j
double polygon_triangulation() {
    int n; cin >> n;    // number of vertices in polygon
    vector<Point> p(n);
    F0R(i, n) cin >> p[i].x >> p[i].y;
    const double INF = 1e18;

    vector<vector<double>> dp(n, vector<double>(n, 0.0));

    // len = length of subpolygon
    for (int len = 2; len < n; len++) {
        for (int i = 0; i < n-len; i++) {
            int j = i + len;
            dp[i][j] = INF;

            for (int k = 1; k < j; k++) {
                double cost = dp[i][k] + dp[k][j] +
                    dist(p[i], p[k]) +
                    dist(p[k], p[j]) +
                    dist(p[i], p[j]);

                dp[i][j] = min(dp[i][j], cost);
            }
        }
    }

    return dp[0][n-1];
}
