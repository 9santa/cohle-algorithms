#pragma once
#include <bits/stdc++.h>
using namespace std;

// For polygon area
struct Point {
    double x, y;
};

// Point-in-polygon: Ray-Casting Algorithm
// Odd cross count -> point is inside, even cross count -> outside
bool insidePolygon(const vector<Point>& p, double x, double y) {
    int n = (int)p.size();
    bool inside = false;

    for (int i = 0, j = n-1; i < n; j = i++) {
        // side (xj, yj) -> (xi, yi)
        double xi = p[i].x, yi = p[i].y;
        double xj = p[j].x, yj = p[j].y;

        // check if a side crosses horizontal ray
        bool crosses_Y = (yi > y) != (yj > y);

        if (crosses_Y) {
            // x_intersection is an intersect point of a side with a ray from our point
            double x_intersection = xi + (y - yi) * (xj - xi) / (yj - yi);

            if (x < x_intersection) {
                inside = !inside;
            }
        }
    }

    return inside;
}
