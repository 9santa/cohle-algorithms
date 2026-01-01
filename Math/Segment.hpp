#pragma once
#include "Point.hpp"

namespace geom {

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


template<typename T>
bool parallel(const Point<T>& a, const Point<T>& b) {
    if constexpr (std::is_floating_point_v<T>) return std::abs(a.cross(b)) <= geoutils::EPS<T>;
    else return a.cross(b) == 0;
}

// Distance
template<typename T>
double distance(const Point<T>& a, const Point<T>& b) { return (a-b).euclid(); }

template<typename T>
T squared_distance(const Point<T>& a, const Point<T>& b) { return (a-b).norm(); }

// Linear interpolation (line between 2 points)
template<typename T, typename U>
Point<double> lerp(const Point<T>& a, const Point<U>& b, double t) {
    return Point<double>(a) * (1-t) + Point<double>(b) * t;
}




















} // namespace geom
