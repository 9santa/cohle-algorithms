#pragma once
#include <bits/stdc++.h>
using namespace std;

using i128 = __int128;
using ll = long long;
#define all(x) (x).begin(), (x).end()
#define sz(x) int((x).size())
#ifndef COHLE_INFINITY_HELPER
#define COHLE_INFINITY_HELPER
template<class T> constexpr T infty = 0;
template<> constexpr int infty<int> = 1'010'000'000;
template<> constexpr long long infty<long long> = 2'020'000'000'000'000'000;
template<> constexpr double infty<double> = numeric_limits<double>::infinity();
template<> constexpr long double infty<long double> = numeric_limits<long double>::infinity();
#endif
template<class T, class U> bool setmin(T& a, const U& b) { if (b < a) { a = b; return true; } return false; }

/* Static/deque CHT = sorted slopes, arbitrary query order, faster
   Dynamic/multiset CHT = arbitrary slopes, arbitrary query order */

/** Line y = kx + b used in convex hull tricks. */
struct Line {
    ll k, b;    // y = k*x + b
    mutable ll p;

    Line(ll _k = 0, ll _b = 0) : k(_k), b(_b) {}

    /** Evaluates the line at x. Time: O(1). */
    i128 eval(ll x) const {
        return (i128)k * x + b;
    }
};

/** Static/deque convex hull trick for sorted slopes. Space: O(n). */
struct StaticCHT {
    deque<Line> hull;
    bool isMax, isInc;

    StaticCHT(bool _isMax, bool _isInc) : isMax(_isMax), isInc(_isInc) {}

    // проверка, является ли mid линия больше не нужна,
    // i.e. она больше не оптимальна не на каком отрезке
    /** Returns whether l2 is made obsolete by l1 and l3. Time: O(1). */
    bool bad(const Line& l1, const Line& l2, const Line& l3) {
        // крест на крест, чтобы избежать переполнений и делений
        i128 a = (i128)(l2.b - l1.b) * (l2.k - l3.k);
        i128 b = (i128)(l3.b - l2.b) * (l1.k - l2.k);
        if (isInc) return a >= b;   // for increasing slopes
        return a <= b;              // for decreasing slopes
    }

    /** Adds line y = kx + b. Amortized time: O(1). */
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
    /** Queries optimum value at x by binary search. Time: O(log n). */
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

    /** Returns whether l1 is worse than l2 at x. Time: O(1). */
    bool need_pop_front(const Line& l1, const Line& l2, ll x) {
        if (isMax) return l1.eval(x) <= l2.eval(x);
        else return l1.eval(x) >= l2.eval(x);
    }

    // O(1) amortized. For monotonic queries (increasing 'x') can also do two pointers.
    // Initialize ptr = 0 before 1st query
    /** Queries optimum for monotone x using a moving pointer. Amortized time: O(1). */
    ll query_two_pointers(ll x, size_t& ptr) {
        if (hull.empty()) return LLONG_MAX;

        // moving ptr forward, 'deleting' non-optimal from the front
        while (ptr + 1 < hull.size() && need_pop_front(hull[ptr], hull[ptr+1], x)) ++ptr;
        return hull[ptr].eval(x);
    }

    // O(n) linear search
    /** Queries optimum by linear scan. Time: O(n). */
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
/** Demonstrates CHT optimization for board packing DP. Time: O(n^2) shown, optimizable. */
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

/** Static CHT wrapper supporting min/max and slope order variants. Space: O(n). */
template<bool isMax, bool isInc>
struct StaticCHT1 {
    deque<Line> hull;
    static constexpr bool isMin = !isMax;

    bool better(ll a, ll b) const {
        if constexpr (isMax) return a >= b;
        else return a <= b;
    }

    static bool bad(const Line& l1, const Line& l2, const Line& l3) {
        i128 lhs = ((i128)l1.b - l2.b) * ((i128)l3.k - l2.k);
        i128 rhs = ((i128)l2.b - l3.b) * ((i128)l2.k - l1.k);

        if constexpr (isMax == isInc)
            return lhs >= rhs;
        else
            return lhs <= rhs;
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
            i128 y1 = hull[mid].eval(x);
            i128 y2 = hull[mid+1].eval(x);

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
