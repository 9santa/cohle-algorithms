#include "core.h"

template<int K, typename T = long long>
struct KDTree {
    struct Point {
        array<T, K> x{};
        int id = -1; // original index
    };

    struct Node {
        Point p;
        array<T, K> mn, mx;
        int l = -1, r = -1;
    };

    vector<Node> dat;
    vector<Point> pts;

    KDTree() = default;
    KDTree(vector<Point> points) {
        build(points);
    }

    static ll sqr(ll x) { return x * x; }
    static bool is_same_point(const Point& a, const Point& b) {
        for (int i = 0; i < K; i++) {
            if (a.x[i] != b.x[i]) return false;
        }
        return true;
    }

    // ---------- L2 (squared Euclidean) ----------
    // dist2 & box_dist2 use Euclidean medatic p=2
    static ll dist2(const Point& a, const Point& b) {
        ll res = 0;
        for (int i = 0; i < K; i++) {
            ll d = (ll)a.x[i] - (ll)b.x[i];
            res += d * d;
        }
        return res;
    }

    /* returns the min possible squared distance from q to any point inside the subdatee box.
        the bounding box is: minimum coords 'mn', maximum coords 'mx'.
        so it computes dist from the query to the hyperrectangle */
    ll box_dist2(int v, const Point& q) const {
        ll res = 0;
        for (int i = 0; i < K; i++) {
            if (q.x[i] < dat[v].mn[i]) {
                ll d = (ll)dat[v].mn[i] - q.x[i];
                res += d * d;
            } else if (q.x[i] > dat[v].mx[i]) {
                ll d = (ll)q.x[i] - dat[v].mx[i];
                res += d * d;
            }
        }
        return res;
    }


    void pull(int v) {
        for (int i = 0; i < K; i++) {
            dat[v].mn[i] = dat[v].mx[i] = dat[v].p.x[i];
        }
        for (int to : {dat[v].l, dat[v].r}) {
            if (to == -1) continue;
            for (int i = 0; i < K; i++) {
                dat[v].mn[i] = min(dat[v].mn[i], dat[to].mn[i]);
                dat[v].mx[i] = max(dat[v].mx[i], dat[to].mx[i]);
            }
        }
    }

    int build_rec(int l, int r, int depth) {
        if (l > r) return -1;
        int dim = depth % K; // current split dimension
        int m = (l + r) >> 1;

        // median point for the current node
        nth_element(pts.begin() + l, pts.begin() + m, pts.begin() + r + 1,
                    [&](const Point& a, const Point& b) {
                    if (a.x[dim] != b.x[dim]) return a.x[dim] < b.x[dim];
                    return a.id < b.id;
                    });

        int v = (int)dat.size();
        dat.push_back(Node{});
        dat[v].p = pts[m];
        dat[v].l = build_rec(l, m-1, depth+1);
        dat[v].r = build_rec(m+1, r, depth+1);
        pull(v);
        return v;
    }

    int root = -1;

    void build(vector<Point> points) {
        pts = std::move(points);
        dat.clear();
        dat.reserve(pts.size());
        if (pts.empty()) {
            root = -1;
            return;
        }
        root = build_rec(0, (int)pts.size()-1, 0);
    }

    // nearest neighbor
    pair<ll, int> nearest_l2(const Point& q, bool allow_same_id = false) const {
        pair<ll, int> best = {LLONG_MAX, INT_MAX};
        nearest_l2_rec(root, q, best, allow_same_id);
        return best; // {dist^2, id}
    }

    /* dfs with pruning
        1. check the node's own point
        2. estimate how close the query could possible be to each subdatee
        3. visit the more promising subdatee first
        4. skip a subdatee entirely if it cannot contain anything better then cur best */
    void nearest_l2_rec(int v, const Point& q, pair<ll, int>& best, bool allow_same_id) const {
        if (v == -1) return;

        ll d = dist2(dat[v].p, q);
        if ((allow_same_id || dat[v].p.id != q.id) &&
            (d < best.first || (d == best.first && dat[v].p.id < best.second))) {
            best = {d, dat[v].p.id};
        }

        int L = dat[v].l;
        int R = dat[v].r;

        ll dl = (L == -1 ? LLONG_MAX : box_dist2(L, q));
        ll dr = (R == -1 ? LLONG_MAX : box_dist2(R, q));

        if (dl < dr) {
            if (dl <= best.first) nearest_l2_rec(L, q, best, allow_same_id);
            if (dr <= best.first) nearest_l2_rec(R, q, best, allow_same_id);
        } else {
            if (dr <= best.first) nearest_l2_rec(R, q, best, allow_same_id);
            if (dl <= best.first) nearest_l2_rec(L, q, best, allow_same_id);
        }
    }

    // ---------- L1 (Manhattan) ----------
    static ll dist1(const Point& a, const Point& b) {
        ll res = 0;
        for (int i = 0; i < K; i++) {
            res += llabs((ll)a.x[i] - b.x[i]);
        }
        return res;
    }

    ll box_dist1(int v, const Point& q) const {
        ll res = 0;
        for (int i = 0; i < K; i++) {
            if (q.x[i] < dat[v].mn[i]) res += (ll)dat[v].mn[i] - q.x[i];
            else if (q.x[i] > dat[v].mx[i]) res += (ll)q.x[i] - dat[v].mx[i];
        }
        return res;
    }

    void nearest_l1_rec(int v, const Point& q, pair<ll, int>& best, bool allow_same_id) const {
        if (v == -1) return;

        ll d = dist1(dat[v].p, q);
        if ((allow_same_id || dat[v].p.id != q.id) &&
            (d < best.first || (d == best.first && dat[v].p.id < best.second))) {
            best = {d, dat[v].p.id};
        }

        int L = dat[v].l;
        int R = dat[v].r;

        ll dl = (L == -1 ? LLONG_MAX : box_dist1(L, q));
        ll dr = (R == -1 ? LLONG_MAX : box_dist1(R, q));

        if (dl < dr) {
            if (dl <= best.first) nearest_l1_rec(L, q, best, allow_same_id);
            if (dr <= best.first) nearest_l1_rec(R, q, best, allow_same_id);
        } else {
            if (dr <= best.first) nearest_l1_rec(R, q, best, allow_same_id);
            if (dl <= best.first) nearest_l1_rec(L, q, best, allow_same_id);
        }
    }

    pair<ll, int> nearest_l1(const Point& q, bool allow_same_id = false) const {
        pair<ll, int> best = {LLONG_MAX, INT_MAX};
        nearest_l1_rec(root, q, best, allow_same_id);
        return best;
    }

    // k nearest neighbors
    vector<pair<ll, int>> k_nearest(const Point& q, int k, bool allow_same_id = false) const {
        priority_queue<pair<ll, int>> pq; // max-heap by distance
        k_nearest_rec(root, q, k, pq, allow_same_id);

        vector<pair<ll, int>> ans;
        while (!pq.empty()) {
            ans.push_back(pq.top());
            pq.pop();
        }
        reverse(ans.begin(), ans.end());
        return ans; // sorted by dist ascending
    }

    void k_nearest_rec(int v, const Point& q, int k,
                       priority_queue<pair<ll, int>>& pq,
                       bool allow_same_id) const {
        if (v == -1) return;

        ll d = dist2(dat[v].p, q);
        if (allow_same_id || dat[v].p.id != q.id) {
            if ((int)pq.size() < k) {
                pq.push({d, dat[v].p.id});
            } else if (d < pq.top().first) {
                pq.pop();
                pq.push({d, dat[v].p.id});
            }
        }

        int L = dat[v].l;
        int R = dat[v].r;

        ll dl = (L == -1 ? LLONG_MAX : box_dist2(L, q));
        ll dr = (R == -1 ? LLONG_MAX : box_dist2(R, q));

        auto can_go = [&](ll boxd) -> bool {
            if ((int)pq.size() < k) return true;
            return boxd <= pq.top().first;
        };

        if (dl < dr) {
            if (can_go(dl)) k_nearest_rec(L, q, k, pq, allow_same_id);
            if (can_go(dr)) k_nearest_rec(R, q, k, pq, allow_same_id);
        } else {
            if (can_go(dr)) k_nearest_rec(R, q, k, pq, allow_same_id);
            if (can_go(dl)) k_nearest_rec(L, q, k, pq, allow_same_id);
        }
    }

    bool box_disjoint(int v, const array<T, K>& lo, const array<T, K>& hi) const {
        for (int i = 0; i < K; i++) {
            if (dat[v].mx[i] < lo[i] || hi[i] < dat[v].mn[i]) return true;
        }
        return false;
    }

    bool point_inside(const Point& p, const array<T, K>& lo, const array<T, K>& hi) const {
        for (int i = 0; i < K; i++) {
            if (p.x[i] < lo[i] || p.x[i] > hi[i]) return false;
        }
        return true;
    }

    void range_query_rec(int v, const array<T, K>& lo, const array<T, K>& hi, vector<int>& ans) const {
        if (v == -1) return;
        if (box_disjoint(v, lo, hi)) return;
        if (point_inside(dat[v].p, lo, hi)) {
            ans.push_back(dat[v].p.id);
        }

        range_query_rec(dat[v].l, lo, hi, ans);
        range_query_rec(dat[v].r, lo, hi, ans);
    }

    vector<int> range_query(const array<T, K>& lo, const array<T, K>& hi) const {
        vector<int> ans;
        range_query_rec(root, lo, hi, ans);
        sort(ans.begin(), ans.end());
        return ans;
    }
};

/* ===== Usage =====
    using KDT = KDTree<2, long long>;

    vector<KDT::Point> pts = {
        {{{2, 3}}, 0},
        {{{5, 4}}, 1},
        {{{9, 6}}, 2},
        {{{4, 7}}, 3},
        {{{8, 1}}, 4},
        {{{7, 2}}, 5}
    };

    KDT kd(pts);

    KDT::Point q{{{6, 3}}, -1};

    auto [d2, id2] = kd.nearest_l2(q);
    cout << "nearest by L2: id=" << id2 << " dist2=" << d2 << "\n";

    auto [d1, id1] = kd.nearest_l1(q);
    cout << "nearest by L1: id=" << id1 << " dist1=" << d1 << "\n";
*/
