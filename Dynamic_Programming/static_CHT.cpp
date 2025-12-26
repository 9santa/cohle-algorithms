#include "../header.h"

struct Line {
    ll k, b;    // y = k*x + b
    mutable ll p;

    Line(ll _k = 0, ll _b = 0) : k(_k), b(_b) {}

    ll eval(ll x) const {
        return k * x + b;
    }
};

struct StaticCHT {
    deque<Line> hull;

    // проверка, является ли mid линия больше не нужна,
    // i.e. она больше не оптимальна не на каком отрезке
    bool bad(const Line& l1, const Line& l2, const Line& l3) {
        // крест на крест, чтобы избежать переполнений и делений
        i128 a = (i128)(l3.b - l1.b) * (l1.k - l2.k);
        i128 b = (i128)(l2.b - l1.b) * (l1.k - l3.k);
        return a <= b;
    }

    void add_line(ll k, ll b) {
        Line newline(k, b);

        while (hull.size() >= 2 && bad(hull[hull.size()-2], hull.back(), newline)) {
            hull.pop_back();
        }
        hull.push_back(newline);
    }

    // binary search on hull to find min f(x)
    ll query(ll x) {
        if (hull.empty()) return LLONG_MAX;

        int lo = 0, hi = sz(hull)-1;
        while (lo < hi) {
            int mid = lo + (hi - lo)/2;
            if (hull[mid].eval(x) <= hull[mid+1].eval(x)) {
                hi = mid;
            } else {
                lo = mid + 1;
            }
        }
        return hull[lo].eval(x);
    }
};
