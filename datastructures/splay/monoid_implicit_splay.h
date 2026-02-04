#include "../../header.h"

template<class Monoid>
struct SplayTree_Monoid {
    using MX = Monoid;
    using X = typename MX::value_type;

    struct Node {
        int ch[2]{0, 0};
        X val, prod;
        int siz = 1, p = 0;
    };

    int n = 0; // number of input elements
    int root = 0; // root index
    vector<Node> t; // node pool, 0 = null

    SplayTree_Monoid(int n_) { init(n_ + 5); build(n_); }

    template<class F> SplayTree_Monoid(int n_, F f) { init(n_ + 5); build(n_, f); }
    SplayTree_Monoid(const vector<X>& v) { init(sz(v) + 5); build(v); }

    void init(int reserve_n = 0) {
        t.clear();
        t.reserve(max(1, reserve_n) + 3);
        t.pop_back(Node{}); // dummy node at 0
        root = 0;
        n = 0;
    }

    // --- build ---
    void build(int m) { build(m, [](int) -> X { return MX::id(); }); }
    void build(const vector<X>& v) {
        build(sz(v), [&](int i) -> X { return v[i]; });
    }

    template<class F>
    void build(int m, F f) {
        n = m;
        vector<X> a(n+2);
        a[0] = MX::id();   // left sentinel
        for (int i = 0; i < n; i++) a[i+1] = f(i);
        a[n+1] = MX::id(); // right sentinel
        // rebuild pool
        t.clear();
        t.push_back(Node{});
        root = build_rec(a, 0, sz(a), 0);
    }

    // --- basic ops ---
    int size() const { return n; }

    // set a[pos] = x
    void set(int pos, X x) {
        assert(0 <= pos && pos < n);
        int v = kth(pos + 1); // +1 cos of left sentinel
        splay(v);
        t[v].val = x;
        pull(v);
    }

    X get(int pos) {
        assert(0 <= pos && pos < n);
        int v = kth(pos + 1);
        splay(v);
        return t[v].val;
    }

    // prod on [l, r)
    X prod(int l, int r) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return MX::id();
        auto [L, R] = expose(l, r);
        int mid = t[R].ch[0];
        return mid ? t[mid].prod : MX::id();
    }

    X prod_all() { return prod(0, n); }

    // insert x before position pos
    void insert(int pos, X x) {
        assert(0 <= pos && pos <= n);
        // boundaries are pos and pos+1 cos of sentinel
        int L = kth(pos);
        splay(L);
        int R = kth(pos+1);
        splay(R, L); // make R right child of root L

        int nd = new_node(x, R);
        t[R].ch[0] = nd;
        pull(R);
        pull(L);
        n++;
    }

    // erase element at position pos (0..n-1)
    void erase(int pos) {
        assert(0 <= pos && pos < n);
        // isolate [pos, pos+1)
        int L = kth(pos);
        splay(L);
        int R = kth(pos + 2);
        splay(R, L);

        t[R].ch[0] = 0; // drop middle (it has exactly 1 element)
        pull(R);
        pull(L);
        n--;
    }

    // max_right: largest r >= l such that check(prod(l, r)) is true
    template<class F>
    int max_right(const F& check, int l) {
        assert(0 <= l && l <= n);
        assert(check(MX::id()));
        if (l == n) return n;
        // isolate suffix [l, n)
        int L = kth(l);
        splay(L);
        int R = kth(n+1);
        splay(R, L);
        int mid = t[R].ch[0];

        X sm = MX::id();
        int len = walk_prefix(mid, sm, check);
        pull(R);
        pull(L);
        return l + len;
    }

    // min_left: smallest l <= r such that check(prod(l, r)) is true
    template<class F>
    int min_left(const F& check, int r) {
        assert(0 <= r && r <= n);
        assert(check(MX::id()));
        if (r == 0) return 0;
        // isolate prefix [0, r)
        int L = kth(0);
        splay(L);
        int R = kth(r+1);
        splay(R, L);
        int mid = t[R].ch[0];

        X sm = MX::id();
        int len = walk_suffix(mid, sm, check);
        pull(R);
        pull(L);
        return r - len;
    }

    // collect everything in correct order with in-order traversal
    vector<X> get_all() {
        vector<X> out;
        out.reserve(n);
        inorder_collect(root, out);
        return out;
    }

private:
    // --- node utils ---
    int siz(int x) const { return x ? t[x].siz : 0; }

    void pull(int x) {
        if (!x) return;
        t[x].siz = 1 + siz(t[x].ch[0]) + siz(t[x].ch[1]);
        X leftp = t[x].ch[0] ? t[t[x].ch[0]].prod : MX::id();
        X rightp = t[x].ch[1] ? t[t[x].ch[1]].prod : MX::id();
        t[x].prod = MX::op(leftp, MX::op(t[x].val, rightp));
    }

    int new_node(const X& v, int p) {
        t.push_back(Node{});
        int x = sz(t) - 1;
        t[x].val = v;
        t[x].prod = v;
        t[x].p = p;
        t[x].siz = 1;
        return x;
    }

    int is_right(int x) const {
        int p = t[x].p;
        return p && t[p].ch[1] == x;
    }

    void set_child(int par, int chi, int dir) {
        t[par].ch[dir] = chi;
        if (chi) t[chi].p = par;
    }

    void rotate(int x) {
        int p = t[x].p;
        int g = t[p].p;
        int dx = is_right(x);
        int dp = is_right(p);
        int b = t[x].ch[dx ^ 1];

        set_child(p, b, dx);
        set_child(x, p, dx ^ 1);

        t[x].p = g;
        if (g) t[g].ch[dp] = x;
        else root = x;

        pull(p);
        pull(x);
    }

    // zig / zig-zig / zig-zag
    void splay(int x, int goal = 0) {
        while (t[x].p != goal) {
            int p = t[x].p;
            int g = t[p].p;
            if (g == goal) {
                rotate(x); // ZIG
            } else if (is_right(x) == is_right(p)) {
                // ZIG-ZIG
                rotate(p);
                rotate(x);
            } else if (is_right(p) && is_right(x) == 0) {
                // ZIG-ZAG
                rotate(x);
                rotate(x);
            }
        }
        if (goal == 0) root = x;
    }

    // k-th in full sequence (including 2 sentinels), k in [0, n+1]
    int kth(int k) {
        assert(0 <= k && k <= n+1);
        int x = root;
        while (true) {
            int lsz = siz(t[x].ch[0]);
            if (k < lsz) x = t[x].ch[0];
            else if (k == lsz) return x;
            else {
                k -= lsz + 1;
                x = t[x].ch[1];
            }
        }
    }

    // expose [l, r) user range:
    // splay kth(l) to root, then splay kth(r+1) to root.right
    pair<int, int> expose(int l, int r){
        int L = kth(l);
        splay(L);
        int R = kth(r+1);
        splay(R, L);
        return {L, R};
    }

    // recursive build
    int build_rec(const vector<X>& a, int l, int r, int p) {
        if (l >= r) return 0;
        int m = (l+r)>>1;
        int x = new_node(a[m], p);
        int lc = build_rec(a, l, m, x);
        int rc = build_rec(a, m+1, r, x);
        t[x].ch[0] = lc;
        t[x].ch[1] = rc;
        pull(x);
        return x;
    }

    template<class F>
    int walk_prefix(int x, X& sm, const F& check) {
        if (!x) return 0;
        int res = 0;
        int L = t[x].ch[0], R = t[x].ch[1];

        if (L) {
            if (!check(MX::op(sm, t[L].prod))) return walk_prefix(L, sm, check);
            sm = MX::op(sm, t[L].prod);
            res += t[L].siz;
        }
        if (!check(MX::op(sm, t[x].val))) return res;
        sm = MX::op(sm, t[x].val);
        res += 1;

        if (R) {
            if (!check(MX::op(sm, t[R].prod))) return res + walk_prefix(R, sm, check);
            sm = MX::op(sm, t[R].prod);
            res += t[R].siz;
        }
        return res;
    }

    template<class F>
    int walk_suffix(int x, X& sm, const F& check) {
        if (!x) return 0;
        int res = 0;
        int L = t[x].ch[0], R = t[x].ch[1];

        if (R) {
            if (!check(MX::op(sm, t[R].prod))) return res + walk_prefix(R, sm, check);
            sm = MX::op(sm, t[R].prod);
            res += t[R].siz;
        }
        if (!check(MX::op(sm, t[x].val))) return res;
        sm = MX::op(sm, t[x].val);
        res += 1;

        if (L) {
            if (!check(MX::op(sm, t[L].prod))) return walk_prefix(L, sm, check);
            sm = MX::op(sm, t[L].prod);
            res += t[L].siz;
        }
        return res;
    }

    void inorder_collect(int x, vector<X>& out) {
        if (!x) return;
        inorder_collect(t[x].ch[0], out);
        out.push_back(t[x].val);
        inorder_collect(t[x].ch[1], out);
        // if called on root, drop sentinels
        if (x == root) {
            if (sz(out) >= 2) {
                out.erase(out.begin());
                out.pop_back();
            }
        }
    }

};
