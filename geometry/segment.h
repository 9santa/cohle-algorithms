#pragma once
#include "point.h"

namespace geom {

/** Segment between two points. Space: O(1). */
template<typename T>
struct Segment {
    Point<T> a, b;
    constexpr Segment(const Point<T>& _a, const Point<T>& _b) : a(_a), b(_b) {}

    // Vector representing the segment
    Point<T> diff() const { return b - a; }

    [[nodiscard]] double length() const { return (b-a).abs(); }

    // Distance from point p to segment
    long double dist_to_seg(const Point<T>& p) const {
        Point<T> ab = b - a;
        Point<T> ap = p - a;
        T t = std::max(T(0), std::min(T(1), ap.dot(ab)/ab.norm()));
        Point<T> closest = a + ab * t;
        return (p - closest).euclid();
    }

    // Check if point lies on segment
    bool contains(const Point<T>& p) const {
        return ccw(a, b, p) == 0 &&
            (p.x >= std::min(a.x, b.x) && p.x <= std::max(a.x, b.x)) &&
            (p.y >= std::min(a.y, b.y) && p.y <= std::max(a.y, b.y));
    }
};


/** Returns whether vectors a and b are parallel. Time: O(1). */
template<typename T>
bool parallel(const Point<T>& a, const Point<T>& b) {
    if constexpr (std::is_floating_point_v<T>) return std::abs(a.cross(b)) <= geoutils::EPS<T>;
    else return a.cross(b) == 0;
}

// Distance
/** Returns Euclidean distance between a and b. Time: O(1). */
template<typename T>
double distance(const Point<T>& a, const Point<T>& b) { return (a-b).euclid(); }

template<typename T>
T squared_distance(const Point<T>& a, const Point<T>& b) { return (a-b).norm(); }

// Linear interpolation (line between 2 points)
/** Linearly interpolates between a and b by t. Time: O(1). */
template<typename T, typename U>
Point<double> lerp(const Point<T>& a, const Point<U>& b, double t) {
    return Point<double>(a) * (1-t) + Point<double>(b) * t;
}

// Chords on a circle with points labeled 1..2n, endpoints are distinct.
/** Returns whether chords (a,b) and (c,d) intersect on a circle. Time: O(1). */
bool chord_intersect(int a, int b, int c, int d) {
    return (a < c && c < b) ^ (a < d && d < b);
}




} // namespace geom
