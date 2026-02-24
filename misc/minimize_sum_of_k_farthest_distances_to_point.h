#include "../header.h"

// 2D optimization
// Minimize sum of k farthest distances to point (X, Y)

struct Point {
    long double x, y;
};

void solve(void) {
    int n, k; cin >> n >> k;

    vector<Point> a(n);
    long double minX = 1e30L, maxX = -1e30L;
    long double minY = 1e30L, maxY = -1e30L;

    for (int i = 0; i < n; i++) {
        cin >> a[i].x >> a[i].y;
        minX = min(minX, a[i].x);
        maxX = max(maxX, a[i].x);
        minY = min(minY, a[i].y);
        maxY = max(maxY, a[i].y);
    }

    // Sum of k largest distances to point (x, y)
    auto F = [&](ld x, ld y) -> ld {
        static vector<ld> d;
        d.resize(n);
        for (int i = 0; i < n; i++) {
            ld dx = x - a[i].x;
            ld dy = y - a[i].y;
            d[i] = sqrtl(dx * dx + dy * dy);
        }
        int pos = n-k;  // k largest
        nth_element(d.begin(), d.begin() + pos, d.end());
        ld sum = 0;
        for (int i = pos; i < n; i++) sum += d[i];
        return sum;
    };

    // Inner ternary search on Y with fixed X
    auto bestYforX = [&](ld X) -> ld {
        ld ly = minY, ry = maxY;
        for (int it = 0; it < 60; it++) {   // 60 iterations usually is enough
            ld m1 = ly + (ry - ly) / 3.0L;
            ld m2 = ry - (ry - ly) / 3.0L;
            ld f1 = F(X, m1);
            ld f2 = F(X, m2);
            if (f1 < f2)
                ry = m2;
            else
                ly = m1;
        }
        return (ly + ry) / 2.0L;    // middle point
    };

    ld lx = minX, rx = maxX;
    ld bestX = minX, bestY = minY;
    ld bestVal = 1e60L;

    // Outer ternary search on X
    for (int it = 0; it < 60; it++) {
        ld m1 = lx + (rx - lx) / 3.0L;
        ld m2 = rx - (rx - lx) / 3.0L;
        ld y1 = bestYforX(m1);
        ld y2 = bestYforX(m2);
        ld f1 = F(m1, y1);
        ld f2 = F(m2, y2);

        if (f1 < f2) {
            rx = m2;
            if (f1 < bestVal) {
                bestVal = f1;
                bestX = m1;
                bestY = y1;
            }
        } else {
            lx = m1;
            if (f2 < bestVal) {
                bestVal = f2;
                bestX = m2;
                bestY = y2;
            }
        }
    }

    cout.setf(ios::fixed);
    cout << setprecision(10);
    cout << (ld)bestX << " " << (ld)bestY << "\n";
    cout << bestVal << "\n";
}
