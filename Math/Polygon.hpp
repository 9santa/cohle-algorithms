#pragma once
#include "Point.hpp"

namespace geom {

template<typename T>
using Polygon = std::vector<Point<T>>;

template<typename T>
double area(const Polygon<T>& poly) {
    double res = 0;
    for (size_t i = 0; i < poly.size(); i++) 
        res += cross(poly[i], poly[(i+1) % poly.size()]);
    return std::abs(res) / 2.0;
}

// Circle functions
template<typename T>
bool concircular(Point<T> a, Point<T> b, Point<T> c, Point<T> d) {
    a -= d, b -= d, c -= d;
    T det = a.norm() * b.cross(c) + b.norm() * c.cross(a) + c.norm() * a.cross(b);
    return det == 0;
}

template<typename T>
bool inside_circle(Point<T> a, Point<T> b, Point<T> c, Point<T> d) {
    a -= d, b -= d, c -= d;
    T det = a.norm() * b.cross(c) + b.norm() * c.cross(a) + c.norm() * a.cross( b);
    return det * ((b-a).cross(c-a) > 0 ? 1 : -1) > 0;
}

template<typename T>
bool outside_circle(Point<T> a, Point<T> b, Point<T> c, Point<T> d) {
    return !inside_circle(a, b, c, d) && !concircular(a, b, c, d);
}





























} // namespace geom
