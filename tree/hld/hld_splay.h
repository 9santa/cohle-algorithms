#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
template<class T> using V = vector<T>;
using vi = vector<int>;
using vl = vector<long long>;
#define sz(x) int((x).size())

/** Implicit splay tree with acted monoid range updates and products. Space: O(n). */
template<class ActedMonoid>
struct SplayTree_ActedMonoid {
    using AM = ActedMonoid;
    using MX = typename AM::Monoid_X;
    using MA = typename AM::Monoid_A;
    using X = typename MX::value_type;
    using A = typename MA::value_type;

    struct Node {
        int ch[2]{0, 0};
        X val, prod;
        int siz = 1, p = 0;
        A lz{MA::id()};
    };

    int n = 0; // number of input elements
    int root = 0; // root index
    vector<Node> t; // node pool, 0 = null

    /** Builds n identity elements. Time: O(n). */
    SplayTree_ActedMonoid(int n_) { init(n_ + 5); build(n_); }

    /** Builds from generator f(i). Time: O(n). */
    template<class F>
    SplayTree_ActedMonoid(int n_, F f) { init(n_ + 5); build(n_, f); }
    /** Builds from values. Time: O(n). */
    SplayTree_ActedMonoid(const vector<X>& v) { init(sz(v) + 5); build(v); }

    /** Clears storage and reserves optional capacity. Time: O(n) to clear. */
    void init(int reserve_n = 0) {
        t.clear();
        t.reserve(max(1, reserve_n) + 3);
        t.push_back(Node{}); // dummy node at 0
        root = 0;
        n = 0;
    }

    /** Rebuilds with m identity elements. Time: O(m). */
    void build(int m) { build(m, [](int) -> X { return MX::id(); }); }
    /** Rebuilds from values. Time: O(n). */
    void build(const vector<X>& v) {
        build(sz(v), [&](int i) -> X { return v[i]; });
    }

    /** Rebuilds from generator f(i). Time: O(m). */
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

    /** Returns the number of stored user elements. Time: O(1). */
    int size() const { return n; }

    /** Sets a[pos] = x. Amortized time: O(log n). */
    void set(int pos, X x) {
        assert(0 <= pos && pos < n);
        int v = kth(pos + 1); // +1 cos of left sentinel
        splay(v);
        push(v);
        t[v].val = x;
        pull(v);
    }

    /** Applies action a to [l, r). Amortized time: O(log n). */
    void update_range(int l, int r, A a) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return;
        auto [L, R] = expose(l, r);
        int mid = t[R].ch[0];
        apply_at(mid, a);
        pull(R);
        pull(L);
    }

    /** Returns a[pos]. Amortized time: O(log n). */
    X get(int pos) {
        assert(0 <= pos && pos < n);
        int v = kth(pos + 1);
        splay(v);
        push(v);
        return t[v].val;
    }

    /** Returns the monoid product on [l, r). Amortized time: O(log n). */
    X prod(int l, int r) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return MX::id();
        auto [L, R] = expose(l, r);
        int mid = t[R].ch[0];
        return mid ? t[mid].prod : MX::id();
    }

    /** Returns the product of the full sequence. Amortized time: O(log n). */
    X prod_all() { return prod(0, n); }

    /** Inserts x before position pos. Amortized time: O(log n). */
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

    /** Erases a[pos]. Amortized time: O(log n). */
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

    /** Returns the largest r >= l with check(prod(l, r)) true. Amortized time: O(log n). */
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

    /** Returns the smallest l <= r with check(prod(l, r)) true. Amortized time: O(log n). */
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

    /** Returns all elements in order. Time: O(n). */
    vector<X> get_all() {
        vector<X> out;
        out.reserve(n);
        inorder_collect(root, out);
        return out;
    }

private:
    // --- node utils ---
    int siz(int x) const { return x ? t[x].siz : 0; }

    // recompute parent from two children
    void pull(int x) {
        if (!x) return;
        t[x].siz = 1 + siz(t[x].ch[0]) + siz(t[x].ch[1]);
        X leftp = t[x].ch[0] ? t[t[x].ch[0]].prod : MX::id();
        X rightp = t[x].ch[1] ? t[t[x].ch[1]].prod : MX::id();
        t[x].prod = MX::op(leftp, MX::op(t[x].val, rightp));
    }

    // update node's val/prod and store lazy tag
    void apply_at(int x, const A& a) {
        if (!x) return;
        t[x].val = AM::act(t[x].val, a, 1);
        t[x].prod = AM::act(t[x].prod, a, (ll)t[x].siz);
        t[x].lz = MA::op(t[x].lz, a);
    }

    // apply lazy at children, clear lazy from this node
    void push(int x) {
        if (!x) return;
        if (t[x].lz == MA::id()) return;
        apply_at(t[x].ch[0], t[x].lz);
        apply_at(t[x].ch[1], t[x].lz);
        t[x].lz = MA::id();
    }

    int new_node(const X& v, int p) {
        t.push_back(Node{});
        int x = sz(t) - 1;
        t[x].val = v;
        t[x].prod = v;
        t[x].p = p;
        t[x].siz = 1;
        t[x].lz = MA::id();
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
        vector<int> st;
        // collect path x -> ... -> goal
        for (int y = x; y != goal; y = t[y].p) st.push_back(y);
        st.push_back(goal);
        for (int i = sz(st)-1; i >= 0; i--) push(st[i]);
        while (t[x].p != goal) {
            int p = t[x].p;
            int g = t[p].p;
            if (g == goal) {
                rotate(x); // ZIG
            } else if (is_right(x) == is_right(p)) {
                // ZIG-ZIG
                rotate(p);
                rotate(x);
            } else { // is_right(p) && is_right(x) == 0
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
            push(x);
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
        push(x);
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
        push(x);
        int res = 0;
        int L = t[x].ch[0], R = t[x].ch[1];

        if (R) {
            if (!check(MX::op(sm, t[R].prod))) return res + walk_suffix(R, sm, check);
            sm = MX::op(sm, t[R].prod);
            res += t[R].siz;
        }
        if (!check(MX::op(sm, t[x].val))) return res;
        sm = MX::op(sm, t[x].val);
        res += 1;

        if (L) {
            if (!check(MX::op(sm, t[L].prod))) return walk_suffix(L, sm, check);
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


/** Additive monoid with inverse and scalar multiplication. Space: O(1). */
template<typename E>
struct Monoid_Add {
    using value_type = E;
    using X = value_type;
    /** Returns x + y. Time: O(1). */
    static constexpr X op(const X& x, const X& y) { return x + y; }
    /** Returns -x. Time: O(1). */
    static constexpr X inverse(const X& x) { return -x; }
    /** Returns x multiplied by b. Time: O(1). */
    static constexpr X multiply(const X& x, ll b) { return x * X(b); }
    /** Returns the additive identity. Time: O(1). */
    static constexpr X id() { return 0; }
    static constexpr bool commute = true;
};


/** Acted monoid for range add and range sum. Space: O(1). */
template<typename E>
struct ActedMonoid_Sum_Add {
    using Monoid_X = Monoid_Add<E>;
    using Monoid_A = Monoid_Add<E>;
    using X = typename Monoid_X::value_type;
    using A = typename Monoid_A::value_type;
    /** Applies an add tag to a sum aggregate of length 'size'. Time: O(1). */
    static constexpr X act(const X& x, const A &a, const ll &size) {
        return x + a * E(size);
    }
};


/** HLD storing each heavy path in a lazy implicit splay tree. Space: O(n). */
struct HLD_Splay {
    int n;
    V<vi> g;
    vi parent, depth, heavy, head, siz;
    vl value;

    // per-path storage
    V<vi> pathNodes;    // nodes per path (head is index 0)
    vi pathId;          // which path node is on
    vi idxInPath;       // index within that path
    vi heads;           // list of head nodes

    // splay per path
    using AM = ActedMonoid_Sum_Add<ll>;
    using Splay = SplayTree_ActedMonoid<AM>;
    vector<Splay> trees;

    HLD_Splay(int n_) : n(n_), g(n), parent(n, -1), depth(n, 0), heavy(n, -1), head(n, 0),
                        siz(n, 0), value(n, 0), pathId(n, -1), idxInPath(n, -1) {}

    /** Computes subtree sizes and heavy children. Time: O(subtree size). */
    int dfs1(int u, int p) {
        parent[u] = p;
        heavy[u] = -1;
        siz[u] = 1;
        int mx = 0;
        for (auto v : g[u]) {
            if (v == p) continue;
            depth[v] = 1 + depth[u];
            int sub = dfs1(v, u);
            siz[u] += sub;
            if (sub > mx) mx = sub, heavy[u] = v;
        }
        return siz[u];
    }

    /** Assigns path ids and indices inside each heavy path. Time: O(subtree size). */
    void dfs2(int u, int h, int pid) {
        head[u] = h;
        pathId[u] = pid;
        idxInPath[u] = sz(pathNodes[pid]);
        pathNodes[pid].push_back(u);

        if (heavy[u] != -1) dfs2(heavy[u], h, pid);
        for (auto v : g[u]) if (v != parent[u] && v != heavy[u]) {
            int newPid = sz(pathNodes);
            pathNodes.push_back({});
            heads.push_back(v);
            dfs2(v, v, newPid);
        }
    }

    /** Builds per-path splay trees. Time: O(n). */
    void build(const V<vi>& G, int root = 0) {
        g = G;
        depth[root] = 0;
        parent[root] = -1;
        dfs1(root, -1);
        pathNodes.clear(), heads.clear(), pathNodes.push_back({}), heads.push_back(root);
        dfs2(root, root, 0);
        trees.clear();
        trees.reserve(pathNodes.size());

        for (int pid = 0; pid < sz(pathNodes); pid++) {
            vl arr;
            arr.reserve(pathNodes[pid].size());
            for (auto u : pathNodes[pid]) arr.push_back(value[u]);
            trees.emplace_back(arr);
        }
    }

    // amort O(log n)
    /** Adds delta to all vertices on path u-v. Amortized time: O(log^2 n). */
    void path_update(int u, int v, ll delta) {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            int pid = pathId[u];
            // segment is head[u]..u => indices [0..idxInPath[u]]
            trees[pid].update_range(0, idxInPath[u] + 1, delta);
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) swap(u, v);
        int pid = pathId[u];
        int l = idxInPath[u], r = idxInPath[v];
        trees[pid].update_range(l, r + 1, delta);
    }

    // amort O(log n)
    /** Returns sum over vertices on path u-v. Amortized time: O(log^2 n). */
    ll path_prod(int u, int v) {
        ll res = 0;
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            int pid = pathId[u];
            res += trees[pid].prod(0, idxInPath[u] + 1);
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) swap(u, v);
        int pid = pathId[u];
        int l = idxInPath[u], r = idxInPath[v];
        res += trees[pid].prod(l, r + 1);
        return res;
    }
};
