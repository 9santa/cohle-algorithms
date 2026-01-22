
// O((n+q) log n)
struct MajorityDS {

struct Node {
    int cand; // possible majority candidate
    int cnt;  // net dominance after cacllations
};

static Node cmb(Node a, Node b) {
    if (a.cnt == 0)  return b;
    if (b.cnt == 0)  return a;
    if (a.cand == b.cand) return {a.cand, a.cnt + b.cnt};
    if (a.cnt > b.cnt) return {a.cand, a.cnt - b.cnt};
    if (b.cnt > a.cnt) return { b.cand, b.cnt - a.cnt};
    return {0, 0};
}

static vector<int> a;

struct SegTree {
    int n;
    vector<Node> t;

    void init(int _n) {
        n = _n;
        t.resize(4*n);
    }

    void build(const vi& a, int v, int tl, int tr) {
        if (tl == tr) t[v] = Node{a[tl], 1};
        else {
            int tm = tl+(tr-tl)/2;
            build(a, v*2, tl, tm);
            build(a, v*2+1, tm+1, tr);
            t[v] = cmb(t[v*2], t[v*2+1]);
        }
    }

    void update(int i, int val) {
        a[i] = val;
        update(1, 0, n-1, i, val);
    }

    void update(int v, int tl, int tr, int posi, int val) {
        if (tl == tr) t[v] = Node{val, 1};
        else {
            int tm = tl+(tr-tl)/2;
            if (posi <= tm) {
                update(v*2, tl, tm, posi, val);
            } else {
                update(v*2+1, tm+1, tr, posi, val);
            }
            t[v] = cmb(t[v*2], t[v*2+1]);
        }
    }

    Node query(int v, int tl, int tr, int l, int r) {
        if (l > r) return {0, 0};
        if (l == tl && r == tr) return t[v];
        int tm = tl+(tr-tl)/2;
        return cmb(
            query(v*2, tl, tm, l, min(r, tm)),
            query(v*2+1, tm+1, tr, max(l, tm+1), r)
        );
    }
};

template<typename T>
struct FenwickTree {
    int n;
    vector<T> fw;

    FenwickTree(int _n = 0) : n(_n) { fw.assign(n+1, 0); }

    static int lsb(int i) { return i & -i; }

    void build(const vector<T>& a) {
        n = sz(a) + 1;
        for (int i = 1; i < n; i++) {
            fw[i] += a[i-1];
            if (i + lsb(i) <= sz(a)) {
                fw[i + lsb(i)] += fw[i];
            }
        }
    }

    void update(int i, T val) {
        for (++i; i < n; i += lsb(i))
            fw[i] += val;
    }

    T sum(int i) {
        T res = 0;
        for (++i; i > 0; i -= lsb(i)) res += fw[i];
        return res;
    }
};

struct FenwickTree01 {
    vector<u64> a;       // bit blocks
    #undef ft
    FenwickTree<int> ft; // counts of ones per block

    FenwickTree01(int n = 0) {
        int m = (n + 63) / 64;
        a.resize(m);
        ft = FenwickTree<int>(m);
    }

    void set(int i, bool f) {
        int j = i % 64; // bit inside block
        i /= 64;        // block index
        int x = (a[i] >> j) & 1;    // current bit
        if (x && !f) {
            a[i] ^= 1ULL << j;
            ft.update(i, -1);
        } else if (!x && f) {
            a[i] |= 1ULL << j;
            ft.update(i, 1);
        }
    }

    // prefix sum query [0, i)
    int sum(int i) {
        int block = i / 64;
        int bit = i % 64;

        int s = 0;
        if (block > 0)
            s = ft.sum(block - 1); // full blocks [0, block)
        if (bit && block < sz(a)) s += __builtin_popcountll(a[block] & ((1ULL << bit) - 1));
        return s;
    }

    int rangeSum(int l, int r) {
        return sum(r) - sum(l);
    }
};


signed main(void) {
    int n, q; re(n, q);
    a.resize(n);
    for (int i = 0; i < n; i++) cin >> a[i];

    vector<array<int, 3>> queries(q);
    vector<int> vals = a;
    for (int i = 0; i < q; i++) {
        cin >> queries[i][0] >> queries[i][1] >> queries[i][2];
        if (queries[i][0] == 0) vals.push_back(queries[i][2]);
    }

    // coordinate compression
    sort(all(vals));
    vals.erase(unique(all(vals)), vals.end());
    auto id = [&vals](int x) {
        return lower_bound(all(vals), x) - vals.begin();
    };

    for (int i = 0; i < n; i++) a[i] = id(a[i]);

    for (auto &[t, p, x] : queries)
        if (t == 0) x = id(x);

    /* static positions list
       xs[v] = all indices where value v can ever appear */
    int m = sz(vals);
    vector<vector<int>> xs(m);
    // initial array
    for (int i = 0; i < n; i++)
        xs[a[i]].push_back(i);

    // future updates
    for (auto [t, p, x] : queries) {
        if (t == 0) xs[x].push_back(p);
    }

    // clean it up
    for (int v = 0; v < m; v++) {
        sort(xs[v].begin(), xs[v].end());
        xs[v].erase(unique(xs[v].begin(), xs[v].end()), xs[v].end());
    }

    // Fenwick01 per value
    vector<FenwickTree01> occ(m);
    for (int v = 0; v < m; v++)
        occ[v] = FenwickTree01(xs[v].size());

    for (int i = 0; i < n; i++) {
        int v = a[i];
        int idx = lower_bound(all(xs[v]), i) - xs[v].begin();
        occ[v].set(idx, 1);
    }

    // --- segment tree ---
    SegTree st;
    st.init(n);
    st.build(a, 1, 0, n-1);

    // --- process queries ---
    for (auto [t, x, y] : queries) {
        if (t == 0) { // update
            int old = a[x];
            if (old != y) {
                int idx_old = lower_bound(all(xs[old]), x) - xs[old].begin();
                occ[old].set(idx_old, 0);

                int idx_new = lower_bound(all(xs[y]), x) - xs[y].begin();
                occ[y].set(idx_new, 1);

                a[x] = y;
                st.update(x, y);
            }
        } else { // query [x, y)
            int l = x, r = y-1;
            Node res = st.query(1, 0, n-1, l, r);

            if (res.cnt == 0) {
                cout << -1 << "\n";
                continue;
            }

            int v = res.cand;
            int L = lower_bound(all(xs[v]), x) - xs[v].begin();
            int R = lower_bound(all(xs[v]), y) - xs[v].begin();
            int freq = occ[v].rangeSum(L, R);

            if (freq * 2 > (y - x))
                cout << vals[v] << "\n";
            else
                cout << -1 << "\n";
        }
    }

    return 0;
}
}; // struct MajorityDS


// SLOW VERSION! FOR LEARNING PURPOSES! vector.erase() is the killer here, need something better
// For example: Fenwick over Fenwick01
namespace slow {
struct Node {
    int cand; // possible majority candidate
    int cnt;  // net dominance after cacllations
};

Node cmb(Node a, Node b) {
    if (a.cnt == 0)  return b;
    if (b.cnt == 0)  return a;
    if (a.cand == b.cand) return {a.cand, a.cnt + b.cnt};
    if (a.cnt > b.cnt) return {a.cand, a.cnt - b.cnt};
    if (b.cnt > a.cnt) return { b.cand, b.cnt - a.cnt};
    return {0, 0};
}

vector<int> a = {1, 5, 5, 2, 4, 3, 2};
unordered_map<int, vector<int>> pos;

struct SegTree {
    int n;
    vector<Node> t;

    void init(int _n) {
        n = _n;
        t.resize(4*n);
    }

    void build(const vi& a, int v, int tl, int tr) {
        if (tl == tr) t[v] = Node{a[tl], 1};
        else {
            int tm = tl+(tr-tl)/2;
            build(a, v*2, tl, tm);
            build(a, v*2+1, tm+1, tr);
            t[v] = cmb(t[v*2], t[v*2+1]);
        }
    }

    void update(int i, int val) {
        pos[a[i]].erase(remove(pos[a[i]].begin(), pos[a[i]].end(), i), pos[a[i]].end());
        pos[val].push_back(i);
        a[i] = val;
        update(1, 0, n-1, i, val);
    }

    void update(int v, int tl, int tr, int posi, int val) {
        if (tl == tr) t[v] = Node{val, 1};
        else {
            int tm = tl+(tr-tl)/2;
            if (posi <= tm) {
                update(v*2, tl, tm, posi, val);
            } else {
                update(v*2+1, tm+1, tr, posi, val);
            }
            t[v] = cmb(t[v*2], t[v*2+1]);
        }
    }

    Node query(int v, int tl, int tr, int l, int r) {
        if (l > r) return {0, 0};
        if (l == tl && r == tr) return t[v];
        int tm = tl+(tr-tl)/2;
        return cmb(
            query(v*2, tl, tm, l, min(r, tm)),
            query(v*2+1, tm+1, tr, max(l, tm+1), r)
        );
    }

};


int count(int v, int l, int r) {
    auto &vec = pos[v];
    return lower_bound(vec.begin(), vec.end(), r)
        - lower_bound(vec.begin(), vec.end(), l);
}


signed main(void) {
    for (int i = 0; i < sz(a); i++) {
        pos[a[i]].push_back(i);
    }
    int n = sz(a);
    SegTree t;
    t.init(n);
    t.build(a, 1, 0, n-1);

    int l = 1, r = 4;
    r--;
    auto res = t.query(1, 0, n-1, l, r);
    if (res.cnt == 0) print(-1);
    else {
        int freq = count(res.cand, l, r);
        if (freq * 2 > r-l) print(res.cand);
        else print(-1);
    }

    return 0;
}
} // namespace slow

