#include "../header.h"
#include <type_traits>
#include <utility>

using ld = long double;

namespace geom {

template<typename T>
struct Point {
    T x{}, y{};

    constexpr Point() noexcept = default;
    constexpr Point(T _x, T _y) noexcept : x(_x), y(_y) {}
    template<typename U, typename V>
    constexpr Point(U _x, V _y) noexcept : x(static_cast<T>(_x)), y(static_cast<T>(_y)) {}

    // Conversions from pair/array
    template<typename U, typename V>
    constexpr Point(const std::pair<U, V>& p) : x(p.first), y(p.second) {}
    template<typename U>
    constexpr Point(const std::array<U, 2>& a) : x(a[0]), y(a[1]) {}

    // Operators
    constexpr Point operator+(const Point& rhs) const noexcept { return  {x + rhs.x, y + rhs.y}; }
    constexpr Point operator-(const Point& rhs) const noexcept { return {x - rhs.x, y - rhs.y}; }
    constexpr Point operator*(T scalar) const noexcept { return {x * scalar, y * scalar}; }
    constexpr Point operator/(T scalar) const noexcept { return {x / scalar, y / scalar}; }
    constexpr Point operator-() const noexcept { return {-x, -y}; }

    Point& operator+=(const Point& rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
    Point& operator-=(const Point& rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
    Point& operator*=(T scalar) noexcept { x *= scalar; y *= scalar; return *this; }
    Point& operator/=(T scalar) noexcept { x /= scalar; y /= scalar; return *this; }

    constexpr bool operator==(const Point& rhs) const noexcept { return x == rhs.x && y == rhs.y; }
    constexpr bool operator!=(const Point& rhs) const noexcept { return !(*this == rhs); }
    constexpr bool operator<(const Point& rhs) const noexcept { return x != rhs.x ? x < rhs.x : y < rhs.y; }

    // Vector operations
    // Dot product
    [[nodiscard]] constexpr T dot(const Point& rhs) const noexcept { return x*rhs.x + y*rhs.y; }
    // 2d cross product
    [[nodiscard]] constexpr T cross(const Point& rhs) const noexcept { return x*rhs.y - y*rhs.x; }
    // Squared len of the vector
    [[nodiscard]] constexpr T norm() const noexcept { return x*x + y*y; }
    // Euclidean length
    [[nodiscard]] constexpr long double euclid() const noexcept { return std::sqrt(norm()); }
    // Returns which half-plane the point lies in (upper vs lower)
    [[nodiscard]] constexpr bool half() const noexcept { return y > 0 || (y == 0 && x > 0); }

    // Rotates vector CCW by theta angle around origin
    [[nodiscard]] Point rotate(double theta) const {
        double c = std::cos(theta), s = std::sin(theta);
        return {x*c - y*s, x*s + y*c};
    }

    // Единичный сонаправленный вектор (ижики)
    [[nodiscard]] Point unit() const {
        auto n = euclid();
        return {x/n, y/n};
    }

    // Rotates 90 degrees CCW
    [[nodiscard]] Point perp() const noexcept { return {-y, x}; }
    [[nodiscard]] Point normal() const { return perp().unit(); }
    [[nodiscard]] Point reflect(const Point& o = {}) const noexcept { return {2*o.x - x, 2*o.y - y}; }

    [[nodiscard]] double angle() const noexcept {
        double a = std::atan2(y, x);
        return a >= 0 ? a : a + 2*acos(-1.0);
    }

    // Stream operators
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        return os << p.x << " " << p.y;
    }
    friend std::istream& operator>>(std::istream& is, Point& p) {
        return is >> p.x >> p.y;
    }
};

namespace geoutils {
    template<typename T>
    constexpr T EPS = std::is_integral_v<T> ? 0 : (T)1e-9;

    template<typename T>
    int sign(T x) {
        if (std::is_integral_v<T>) return (x > 0) - (x < 0);
        return (x > EPS<T>) - (x < -EPS<T>);
    }

    template<typename T>
    bool equals(T a, T b) { return sign(a - b) == 0; }
} // namespace geoutils

// Free functions
template<typename T>
constexpr T dot(const Point<T>& a, const Point<T>& b) noexcept { return a.dot(b); }
template<typename T>
constexpr T cross(const Point<T>& a, const Point<T>& b) noexcept { return a.cross(b); }
template<typename T>
constexpr T cross(const Point<T>& o, const Point<T>& a, const Point<T>& b) { return (a-o).cross(b-o); }

// Returns: 1 for CCW, -1 for CW, 0 for collinear
template<typename T>
int ccw(const Point<T>& a, const Point<T>& b, const Point<T>& c) {
    auto res = cross(b-a, c-a);
    return geoutils::sign(res);
}


// Angle between two points
template<typename T>
double angle_between(const Point<T>& a, const Point<T>& b) {
    return std::atan2(a.cross(b), a.dot(b));
}

// Angle sorting comparator
template<typename T>
struct compare_by_angle {
    Point<T> origin;
    explicit compare_by_angle(const Point<T>& o = {0, 0}) : origin(o) {}
    bool operator()(const Point<T>& a, const Point<T>& b) const {
        Point<T> pa = a - origin, pb = b - origin;
        if (pa.half() != pb.half()) return pa.half() > pb.half();
        return pa.cross(pb) > 0;
    }
};


} // namespace geom

