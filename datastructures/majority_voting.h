#include "../header.h"

// O((n+q) log n) after preprocessing all future update values/positions.
// This is an offline dynamic-majority structure:
// - build() takes the initial array and the list of future point-updates
// - update()/query_majority() run online after that preparation step
struct MajorityDS {
    struct Node {
        int cand; // possible majority candidate
        int cnt;  // net dominance after cancellations
    };

    static Node cmb(Node a, Node b) {
        if (a.cnt == 0) return b;
        if (b.cnt == 0) return a;
        if (a.cand == b.cand) return {a.cand, a.cnt + b.cnt};
        if (a.cnt > b.cnt) return {a.cand, a.cnt - b.cnt};
        if (b.cnt > a.cnt) return {b.cand, b.cnt - a.cnt};
        return {0, 0};
    }

    struct SegTree {
        int n = 0;
        vector<Node> t;

        void init(int _n) {
            n = _n;
            t.assign(max(1, 4 * n), {0, 0});
        }

        void build(const vi& a, int v, int tl, int tr) {
            if (tl == tr) t[v] = Node{a[tl], 1};
            else {
                int tm = tl + (tr - tl) / 2;
                build(a, v * 2, tl, tm);
                build(a, v * 2 + 1, tm + 1, tr);
                t[v] = cmb(t[v * 2], t[v * 2 + 1]);
            }
        }

        void update(int v, int tl, int tr, int posi, int val) {
            if (tl == tr) t[v] = Node{val, 1};
            else {
                int tm = tl + (tr - tl) / 2;
                if (posi <= tm) update(v * 2, tl, tm, posi, val);
                else update(v * 2 + 1, tm + 1, tr, posi, val);
                t[v] = cmb(t[v * 2], t[v * 2 + 1]);
            }
        }

        Node query(int v, int tl, int tr, int l, int r) const {
            if (l > r) return {0, 0};
            if (l == tl && r == tr) return t[v];
            int tm = tl + (tr - tl) / 2;
            return cmb(
                query(v * 2, tl, tm, l, min(r, tm)),
                query(v * 2 + 1, tm + 1, tr, max(l, tm + 1), r)
            );
        }
    };

    template<typename T>
    struct FenwickTree {
        int n = 0;
        vector<T> fw;

        FenwickTree(int _n = 0) : n(_n), fw(n + 1, 0) {}

        static int lsb(int i) { return i & -i; }

        void update(int i, T val) {
            for (++i; i <= n; i += lsb(i)) fw[i] += val;
        }

        T sum_prefix(int i) const {
            T res = 0;
            for (++i; i > 0; i -= lsb(i)) res += fw[i];
            return res;
        }
    };

    struct FenwickTree01 {
        vector<u64> a;       // bit blocks
        FenwickTree<int> fw; // counts of ones per block

        FenwickTree01(int n = 0) { init(n); }

        void init(int n) {
            int m = (n + 63) / 64;
            a.assign(m, 0);
            fw = FenwickTree<int>(m);
        }

        void set(int i, bool f) {
            int j = i % 64; // bit inside block
            i /= 64;        // block index
            int x = (a[i] >> j) & 1; // current bit
            if (x && !f) {
                a[i] ^= 1ULL << j;
                fw.update(i, -1);
            } else if (!x && f) {
                a[i] |= 1ULL << j;
                fw.update(i, 1);
            }
        }

        // prefix sum query [0, i)
        int sum(int i) const {
            int block = i / 64;
            int bit = i % 64;

            int s = 0;
            if (block > 0) s = fw.sum_prefix(block - 1); // full blocks [0, block)
            if (bit && block < sz(a)) s += __builtin_popcountll(a[block] & ((1ULL << bit) - 1));
            return s;
        }

        int rangeSum(int l, int r) const {
            return sum(r) - sum(l);
        }
    };

    vi a;                     // compressed current array
    vi vals;                  // decompress values
    unordered_map<int, int> id_of;
    vector<vector<int>> xs;   // xs[v] = all indices where compressed value v can ever appear
    vector<FenwickTree01> occ; // occurrence bitsets by compressed value
    SegTree st;

    MajorityDS() = default;

    MajorityDS(const vi& initial, const vector<pair<int, int>>& future_updates = {}) {
        build(initial, future_updates);
    }

    void build(const vi& initial, const vector<pair<int, int>>& future_updates = {}) {
        vals = initial;
        for (auto [pos, val] : future_updates) {
            (void)pos;
            vals.push_back(val);
        }

        sort(all(vals));
        vals.erase(unique(all(vals)), vals.end());
        id_of.clear();
        for (int i = 0; i < sz(vals); i++) id_of[vals[i]] = i;

        a.resize(sz(initial));
        for (int i = 0; i < sz(initial); i++) a[i] = compress(initial[i]);

        xs.assign(sz(vals), {});
        for (int i = 0; i < sz(a); i++) xs[a[i]].push_back(i);
        for (auto [pos, val] : future_updates) xs[compress(val)].push_back(pos);
        for (auto& vec : xs) {
            sort(all(vec));
            vec.erase(unique(all(vec)), vec.end());
        }

        occ.assign(sz(vals), {});
        for (int v = 0; v < sz(vals); v++) occ[v].init(xs[v].size());
        for (int i = 0; i < sz(a); i++) {
            int v = a[i];
            int idx = lower_bound(all(xs[v]), i) - xs[v].begin();
            occ[v].set(idx, 1);
        }

        st.init(sz(a));
        if (!a.empty()) st.build(a, 1, 0, sz(a) - 1);
    }

    int size() const { return sz(a); }

    int compress(int x) const {
        auto it = id_of.find(x);
        assert(it != id_of.end());
        return it->second;
    }

    int value_at(int i) const {
        assert(0 <= i && i < size());
        return vals[a[i]];
    }

    void update(int i, int x) {
        assert(0 <= i && i < size());
        int y = compress(x);
        int old = a[i];
        if (old == y) return;

        int idx_old = lower_bound(all(xs[old]), i) - xs[old].begin();
        occ[old].set(idx_old, 0);

        int idx_new = lower_bound(all(xs[y]), i) - xs[y].begin();
        assert(idx_new < sz(xs[y]) && xs[y][idx_new] == i);
        occ[y].set(idx_new, 1);

        a[i] = y;
        st.update(1, 0, size() - 1, i, y);
    }

    // query [l, r)
    optional<int> query_majority(int l, int r) const {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return nullopt;
        Node res = st.query(1, 0, size() - 1, l, r - 1);
        if (res.cnt == 0) return nullopt;

        int v = res.cand;
        int L = lower_bound(all(xs[v]), l) - xs[v].begin();
        int R = lower_bound(all(xs[v]), r) - xs[v].begin();
        int freq = occ[v].rangeSum(L, R);
        if (freq * 2 > (r - l)) return vals[v];
        return nullopt;
    }
};

// SLOW VERSION! FOR LEARNING PURPOSES! vector.erase() is the killer here, need something better
// For example: Fenwick over Fenwick01
namespace slow {
struct MajorityDS {
    struct Node {
        int cand; // possible majority candidate
        int cnt;  // net dominance after cancellations
    };

    static Node cmb(Node a, Node b) {
        if (a.cnt == 0) return b;
        if (b.cnt == 0) return a;
        if (a.cand == b.cand) return {a.cand, a.cnt + b.cnt};
        if (a.cnt > b.cnt) return {a.cand, a.cnt - b.cnt};
        if (b.cnt > a.cnt) return {b.cand, b.cnt - a.cnt};
        return {0, 0};
    }

    struct SegTree {
        int n = 0;
        vector<Node> t;

        void init(int _n) {
            n = _n;
            t.assign(max(1, 4 * n), {0, 0});
        }

        void build(const vi& a, int v, int tl, int tr) {
            if (tl == tr) t[v] = Node{a[tl], 1};
            else {
                int tm = tl + (tr - tl) / 2;
                build(a, v * 2, tl, tm);
                build(a, v * 2 + 1, tm + 1, tr);
                t[v] = cmb(t[v * 2], t[v * 2 + 1]);
            }
        }

        void update(int v, int tl, int tr, int posi, int val) {
            if (tl == tr) t[v] = Node{val, 1};
            else {
                int tm = tl + (tr - tl) / 2;
                if (posi <= tm) update(v * 2, tl, tm, posi, val);
                else update(v * 2 + 1, tm + 1, tr, posi, val);
                t[v] = cmb(t[v * 2], t[v * 2 + 1]);
            }
        }

        Node query(int v, int tl, int tr, int l, int r) const {
            if (l > r) return {0, 0};
            if (l == tl && r == tr) return t[v];
            int tm = tl + (tr - tl) / 2;
            return cmb(
                query(v * 2, tl, tm, l, min(r, tm)),
                query(v * 2 + 1, tm + 1, tr, max(l, tm + 1), r)
            );
        }
    };

    vi a;
    unordered_map<int, vector<int>> pos;
    SegTree st;

    MajorityDS() = default;
    explicit MajorityDS(const vi& initial) { build(initial); }

    void build(const vi& initial) {
        a = initial;
        pos.clear();
        for (int i = 0; i < sz(a); i++) pos[a[i]].push_back(i);
        for (auto& [v, vec] : pos) sort(all(vec));
        st.init(sz(a));
        if (!a.empty()) st.build(a, 1, 0, sz(a) - 1);
    }

    void update(int i, int val) {
        pos[a[i]].erase(remove(all(pos[a[i]]), i), pos[a[i]].end());
        pos[val].push_back(i);
        sort(all(pos[val]));
        a[i] = val;
        st.update(1, 0, sz(a) - 1, i, val);
    }

    int count(int v, int l, int r) const {
        auto it = pos.find(v);
        if (it == pos.end()) return 0;
        const auto& vec = it->second;
        return lower_bound(all(vec), r) - lower_bound(all(vec), l);
    }

    optional<int> query_majority(int l, int r) const {
        if (l >= r) return nullopt;
        auto res = st.query(1, 0, sz(a) - 1, l, r - 1);
        if (res.cnt == 0) return nullopt;
        int freq = count(res.cand, l, r);
        if (freq * 2 > (r - l)) return res.cand;
        return nullopt;
    }
};
} // namespace slow

#ifdef MAJORITY_VOTING_DEMO
signed main(void) {
    int n, q; re(n, q);
    vector<int> a(n);
    for (int i = 0; i < n; i++) cin >> a[i];

    vector<array<int, 3>> queries(q);
    vector<pair<int, int>> future_updates;
    for (int i = 0; i < q; i++) {
        cin >> queries[i][0] >> queries[i][1] >> queries[i][2];
        if (queries[i][0] == 0) future_updates.push_back({queries[i][1], queries[i][2]});
    }

    MajorityDS ds(a, future_updates);
    for (auto [t, x, y] : queries) {
        if (t == 0) ds.update(x, y);
        else {
            auto ans = ds.query_majority(x, y);
            if (ans.has_value()) cout << *ans << "\n";
            else cout << -1 << "\n";
        }
    }
    return 0;
}
#endif

#ifdef MAJORITY_VOTING_SLOW_DEMO
signed main(void) {
    slow::MajorityDS ds({1, 5, 5, 2, 4, 3, 2});
    auto res = ds.query_majority(1, 4);
    if (res.has_value()) print(*res);
    else print(-1);
    return 0;
}
#endif
