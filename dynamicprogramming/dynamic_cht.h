#include "core.h"

/* Static/deque CHT = sorted slopes, arbitrary query order, faster
   Dynamic/multiset CHT = arbitrary slopes, arbitrary query order */

struct Line {
    ll k, b;      // y = kx + b
    mutable ll p; // last x where this line is optimal (right boundary)

    Line(ll _k = 0, ll _b = 0) : k(_k), b(_b), p(0) {}

    ll eval(ll x) const {
        return k * x + b;
    }

    bool operator<(const Line& rhs) const { return k < rhs.k; }

    bool operator<(ll x) const { return p < x; }
};

/*
    For arbitrary slopes. Can remove lines anywhere.
    Query by binary searching the intersection.
*/
struct DynamicCHT : multiset<Line, std::less<>> {
    static const ll INF = (ll)4e18;

    // floor division (to handle negatives)
    static ll div_floor(ll a, ll b) {
        return a / b - ((a ^ b) < 0 && a % b);
    }

    bool intersect(iterator x, iterator y) {
        if (y == end()) {
            x->p = INF;
            return false;
        }
        if (x->k == y->k) {
            x->p = (x->b > y->b ? INF : -INF);
        } else {
            x->p = div_floor(y->b - x->b, x->k - y->k);
        }
        return x->p >= y->p; // if true y is bad
    }

    void add_line(ll k, ll b) {
        auto z = insert(Line(k, b));
        auto y = z++;
        auto x = y;

        // remove useless lines ahead
        while (intersect(y, z)) z = erase(z);

        // check if left line x is now useless, if so recalculate 'p'
        if (x != begin() && intersect(--x, y)) {
            intersect(x, erase(y));
        }

        // remove useless before, because previous check can cause the 'domino effect'
        while ((y = x) != begin() && (--x)->p >= y->p) {
            intersect(x, erase(y));
        }
    }

    ll query(ll x) {
        assert(!empty());
        auto l = *lower_bound(x);
        return l.eval(x);
    }
};

template<bool isMax>
struct DynamicCHT1 {
    struct Line {
        ll k, b;      // y = kx + b
        mutable ll p; // last x where this line is optimal (right boundary)

        Line(ll _k = 0, ll _b = 0) : k(_k), b(_b), p(0) {}

        ll eval(ll x) const {
            return k * x + b;
        }

        bool operator<(const Line& rhs) const { return k < rhs.k; }

        bool operator<(ll x) const { return p < x; }
    };

    static constexpr ll INF = (ll)4e18;
    std::multiset<Line, std::less<>> hull;

    static ll floor_div(ll a, ll b) {
        return a / b - (((a ^ b) < 0) && (a % b));
    }

    bool intersect(typename std::multiset<Line, std::less<>>::iterator x,
                   typename std::multiset<Line, std::less<>>::iterator y) {
        if (y == hull.end()) {
            x-> p = INF;
            return false;
        }
        if (x->k == y->k) {
            x->p = (x->b > y->b ? INF : -INF);
        } else {
            x->p = floor_div(y->b - x->b, x->k, y->k);
        }
        return x->p >= y->p;
    }

    void add_line(ll k, ll b) {
        if constexpr (!isMax) {
            k = -k;
            b = -b;
        }

        auto z = hull.insert(Line(k, b));
        auto y = z++;
        auto x = y;

        while (intersect(y, z)) z = hull.erase(z);

        if (x != hull.begin() && intersect(--x, y)) {
            intersect(x, y = hull.erase(y));
        }

        while ((y = x) != hull.begin() && (--x)->p >= y->p) {
            intersect(x, hull.erase(y));
        }
    }

    ll query(ll x) const {
        assert(!hull.empty());
        auto l = *hull.lower_bound(x);
        ll ans = l.eval(x);
        if constexpr (!isMax) ans = -ans;
        return ans;
    }

    auto query_it(ll x) const {
        assert(!hull.empty());
        return hull.lower_bound(x);
    }
};
