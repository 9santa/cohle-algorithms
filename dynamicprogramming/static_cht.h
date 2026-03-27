#include "core.h"


/* Static/deque CHT = sorted slopes, arbitrary query order, faster
   Dynamic/multiset CHT = arbitrary slopes, arbitrary query order */

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
    bool isMax, isInc;

    StaticCHT(bool _isMax, bool _isInc) : isMax(_isMax), isInc(_isInc) {}

    // проверка, является ли mid линия больше не нужна,
    // i.e. она больше не оптимальна не на каком отрезке
    bool bad(const Line& l1, const Line& l2, const Line& l3) {
        // крест на крест, чтобы избежать переполнений и делений
        i128 a = (i128)(l2.b - l1.b) * (l2.k - l3.k);
        i128 b = (i128)(l3.b - l2.b) * (l1.k - l2.k);
        if (isInc) return a >= b;   // for increasing slopes
        return a <= b;              // for decreasing slopes
    }

    void add_line(ll k, ll b) {
        Line newline(k, b);

        if (!hull.empty() && hull.back().k == k) {
            // if Max, keep higher b for tied k. if Min, keep lower b
            if (isMax) { if (b > hull.back().b) hull.pop_back(); else return; }
            else { if (b < hull.back().b) hull.pop_back(); else return; }
        }

        while (hull.size() >= 2 && bad(hull[hull.size()-2], hull.back(), newline)) {
            hull.pop_back();
        }
        hull.push_back(newline);
    }

    // O(log n) binary search on hull to find min/max f(x)
    ll query(ll x) {
        if (hull.empty()) return LLONG_MAX;

        int lo = 0, hi = sz(hull)-1;
        while (lo < hi) {
            int mid = lo + (hi - lo)/2;
            // if Min, we want the first line that stops decreasing
            // if Max, we want the first line that stops increasing
            if (isMax) {
                if (hull[mid].eval(x) >= hull[mid+1].eval(x)) hi = mid;
                else lo = mid + 1;
            } else {
                if (hull[mid].eval(x) <= hull[mid+1].eval(x)) hi = mid;
                else lo = mid + 1;
            }
        }
        return hull[lo].eval(x);
    }

    bool need_pop_front(const Line& l1, const Line& l2, ll x) {
        if (isMax) return l1.eval(x) <= l2.eval(x);
        else return l1.eval(x) >= l2.eval(x);
    }

    // O(1) amortized. For monotonic queries (increasing 'x') can also do two pointers.
    // Initialize ptr = 0 before 1st query
    ll query_two_pointers(ll x, size_t& ptr) {
        if (hull.empty()) return LLONG_MAX;

        // moving ptr forward, 'deleting' non-optimal from the front
        while (ptr + 1 < hull.size() && need_pop_front(hull[ptr], hull[ptr+1], x)) ++ptr;
        return hull[ptr].eval(x);
    }

    // O(n) linear search
    ll query_linear(ll x) {
        if (hull.empty()) return LLONG_MAX;
        ll min_val = infty<ll>;
        for (const auto& l : hull) {
            setmin(min_val, l.eval(x));
        }
        return min_val;
    }
};

// Дано N досок с шириной и длиной ai, bi. Надо упаковать их всех за минимальную стоимость. Стоимость коробки считается как height * width.
void sample_problem(vector<pair<int, int>> v) {
    // Naive DP solution O(n^2)
    int n = (int)v.size();
    sort(all(v));

    vector<pair<int, int>> boards;
    ll maxB = 0;
    for (int i = n-1; i >= 0; i--) {
        if (v[i].second > maxB) {
            boards.push_back(v[i]);
            maxB = v[i].second;
        }
    }

    reverse(all(boards));
    int m = (int)boards.size();
    vector<ll> dp(m+1, infty<ll>);
    dp[0] = 0;

    for (int i = 1; i <= m; i++) {
        for (int j = 0; j < i; j++) {
            setmin(dp[i], dp[j] + boards[i-1].first * boards[j].second);
        }
    }

    ll naive_ans = dp[m];


    // CHT solution O(n log n)
    dp.assign(m+1, infty<ll>);
    dp[0] = 0;
    StaticCHT hull(0, 1);
    hull.add_line(-boards[0].second, dp[0]);

    for (int i = 1; i <= m; i++) {
        ll query_x = -boards[i-1].first;

        dp[i] = hull.query(query_x);

        if (i < m) hull.add_line(-boards[i].second, dp[i]);
    }

    ll cht_ans = dp[m];
    assert(naive_ans == cht_ans);
}

template<bool isMax, bool isInc>
struct StaticCHT1 {
    deque<Line> hull;
    static constexpr bool isMin = !isMax;

    bool better(ll a, ll b) const {
        if constexpr (isMax) return a >= b;
        else return a <= b;
    }

    static bool bad(const Line& l1, const Line& l2, const Line& l3) {
        // remove l2 if intersection(l1, l2) >= intersection(l2, l3)
        return (i128)(l1.b - l2.b) * (l3.k - l2.k)
                >= (i128)(l2.b - l3.b) * (l2.k - l1.k);
    }

    void add_line(ll k, ll b) {
        Line newline(k, b);

        if (!hull.empty()) {
            if constexpr (isInc) {
                assert(hull.back().k <= k);
            } else {
                assert(hull.back().k >= k);
            }
        }

        if (!hull.empty() && hull.back().k == newline.k) {
            if (better(newline.b, hull.back().b)) hull.pop_back();
            else return;
        }

        while (hull.size() >= 2 && bad(hull[hull.size() - 2], hull.back(), newline)) {
            hull.pop_back();
        }
        hull.push_back(newline);
    }

    // O(log n), arbitrary query order
    ll query(ll x) const {
        assert(!hull.empty());
        int lo = 0, hi = sz(hull) - 1;
        while (lo < hi) {
            int mid = (lo + hi) >> 1;
            ll y1 = hull[mid].eval(x);
            ll y2 = hull[mid+1].eval(x);

            if (better(y1, y2)) hi = mid;
            else lo = mid + 1;
        }
        return hull[lo].eval(x);
    }
};

using MinIncCHT = StaticCHT1<false, true>;
using MinDecCHT = StaticCHT1<false, false>;
using MaxIncCHT = StaticCHT1<true, true>;
using MaxDecCHT = StaticCHT1<true, false>;
