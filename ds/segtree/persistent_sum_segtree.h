#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Persistent segment tree over an arbitrary monoid. Space: O(n + q log n). */
template<class Monoid>
struct PST {
    using X = typename Monoid::value_type;
    struct Node {
        int l = 0, r = 0;
        X val = Monoid::id();
    };

    int n;
    vector<Node> st;    // node pool, st[0] = null node

    PST() : n(0) {}
    PST(int n_, int reserve_nodes = 0) { init(n_, reserve_nodes); }

    /** Initializes an empty persistent tree over n positions. Time: O(1). */
    void init(int n_, int reserve_nodes = 0) {
        n = n_;
        st.clear();
        st.reserve(max(1, reserve_nodes));
        st.push_back(Node());
    }

    int clone(const Node& x) {
        st.push_back(x);
        return (int)st.size() - 1;
    }

    /** Builds one version from values f(i) over [0, n). Time: O(n). */
    template<class F>
    int build(F f) { return build(0, n, f); }

    template<class F>
    int build(int tl, int tr, F& f) {
        Node cur;
        if (tr - tl == 1) {
            cur.val = f(tl);
            return clone(cur);
        }
        int tm = (tl+tr)>>1;
        cur.l = build(tl, tm, f);
        cur.r = build(tm, tr, f);
        cur.val = Monoid::op(st[cur.l].val, st[cur.r].val);
        return clone(cur);
    }

    /** Returns a new root after setting a[pos] to x. Time: O(log n). */
    int set(int root, int pos, const X& x) { return set(root, 0, n, pos, x); }

    int set(int v, int tl, int tr, int pos, const X& x) {
        Node cur = st[v];
        if (tr - tl == 1) {
            cur.val = x;
            return clone(cur);
        }
        int tm = (tl+tr)>>1;
        if (pos < tm) cur.l = set(cur.l, tl, tm, pos, x);
        else cur.r = set(cur.r, tm, tr, pos, x);
        cur.val = Monoid::op(st[cur.l].val, st[cur.r].val);
        return clone(cur);
    }

    /** Returns a new root after applying op(a[pos], delta). Time: O(log n). */
    int apply(int root, int pos, const X& delta) { return apply(root, 0, n, pos, delta); }

    int apply(int v, int tl, int tr, int pos, const X& delta) {
        Node cur = st[v];
        if (tr - tl == 1) {
            cur.val = Monoid::op(cur.val, delta);
            return clone(cur);
        }
        int tm = (tl+tr)>>1;
        if (pos < tm) cur.l = apply(cur.l, tl, tm, pos, delta);
        else cur.r = apply(cur.r, tm, tr, pos, delta);
        cur.val = Monoid::op(st[cur.l].val, st[cur.r].val);
        return clone(cur);
    }

    /** Returns the monoid product over [l, r) in this version. Time: O(log n). */
    X prod(int root, int l, int r) const { return prod(root, 0, n, l, r); }

    X prod(int v, int tl, int tr, int l, int r) const {
        if (!v || r <= tl || tr <= l) return Monoid::id();
        if (l <= tl && tr <= r) return st[v].val;
        int tm = (tl+tr)>>1;
        return Monoid::op(prod(st[v].l, tl, tm, l, r),
                          prod(st[v].r, tm, tr, l, r));
    }

    /** Returns the monoid product over the whole version. Time: O(1). */
    X all_prod(int root) const { return st[root].val; }

    /** Returns max r such that f(prod(root, l, r)) is true. Time: O(log n). */
    template<class F>
    int max_right(int root, int l, F f) const {
        assert(0 <= l && l <= n);
        assert(f(Monoid::id()));
        if (l == n) return n;
        X acc = Monoid::id();
        return max_right_dfs(root, 0, n, l, f, acc);
    }

    template<class F>
    int max_right_dfs(int v, int tl, int tr, int l, F& f, X& acc) const {
        if (tr <= l) return tr; // completely before l, skip
        if (!v) {
            // null node contributes id, so we can treat this whole segment as accepted
            return tr;
        }
        if (l <= tl) {
            X nxt = Monoid::op(acc, st[v].val);
            if (f(nxt)) {
                acc = nxt;
                return tr; // take whole segment
            }
            if (tr - tl == 1) {
                return tl; // cannot take this single element if a leaf
            }
        }
        int tm = (tl+tr)>>1;
        int res = max_right_dfs(st[v].l, tl, tm, l, f, acc);   // try to take left child
        if (res < tm) return res;   // if couldn't take the whole left child segment, answer inside left
        return max_right_dfs(st[v].r, tm, tr, l, f, acc);  // try to extend via right child, if took the whole left child
    }

    /** Returns min l such that f(prod(root, l, r)) is true. Time: O(log n). */
    template<class F>
    int min_left(int root, int r, F f) const {
        assert(0 <= r && r <= n);
        assert(f(Monoid::id()));
        if (r == 0) return 0;
        X acc = Monoid::id();
        return min_left_dfs(root, 0, n, r, f, acc);
    }

    template<class F>
    int min_left_dfs(int v, int tl, int tr, int r, F& f, X& acc) const {
        if (r <= tl) return tl;
        if (!v) {
            return tl;
        }
        if (tr <= r) {
            X nxt = Monoid::op(st[v].val, acc);
            if (f(nxt)) {
                acc = nxt;
                return tl;
            }
            if (tr - tl == 1) {
                return tr;
            }
        }
        int tm = (tl+tr)>>1;
        int res = min_left_dfs(st[v].r, tm, tr, r, f, acc);
        if (res > tm) return res;
        return min_left_dfs(st[v].l, tl, tm, r, f, acc);
    }
};


/** Persistent frequency/sum segment tree over positions [1, n]. Space: O(q log n). */
struct PST1 {
    struct Node {
        int l = 0, r = 0; // children indices in pool
        int sum = 0;
    };

    int n;  // positions 1..n
    vector<Node> st;    // node pool, st[0] = null node

    PST1(int n_, int reserve_nodes = 0) : n(n_) {
        st.reserve(max(1, reserve_nodes));
        st.push_back(Node());    // null node at index 0
    }

    int clone(const Node& x) {
        st.push_back(x);
        return (int)st.size() - 1;
    }

    /** Returns a new root after adding delta at position pos. Time: O(log n). */
    int add(int v, int tl, int tr, int pos, int delta) {
        Node cur = st[v];
        if (tl == tr) {
            cur.sum += delta;
            return clone(cur);
        }
        int tm = (tl+tr) >> 1;
        if (pos <= tm) cur.l = add(cur.l, tl, tm, pos, delta);
        else cur.r = add(cur.r, tm+1, tr, pos, delta);
        cur.sum = st[cur.l].sum + st[cur.r].sum;
        return clone(cur);
    }

    /** Returns a new root after adding delta at position pos. Time: O(log n). */
    int add(int root, int pos, int delta) {
        return add(root, 1, n, pos, delta);
    }

    /** Returns the range sum over [l, r]. Time: O(log n). */
    int query(int v, int tl, int tr, int l, int r) const {
        if (!v || r < tl || l > tr) return 0;
        if (l <= tl && tr <= r) return st[v].sum;
        int tm = (tl+tr) >> 1;
        return query(st[v].l, tl, tm, l, r) + query(st[v].r, tm+1, tr, l, r);
    }

    /** Returns the range sum over [l, r]. Time: O(log n). */
    int query(int root, int l, int r) const {
        if (l > r) return 0;
        return query(root, 1, n, l, r);
    }

    /** Returns the total sum in this version. Time: O(1). */
    int total(int root) const { return st[root].sum; }

    /** Returns the smallest position p with prefix sum >= k. Time: O(log n). */
    int kth(int v, int tl, int tr, int k) const {
        if (tl == tr) return tl;
        int left = st[v].l;
        int left_sum = st[left].sum;
        int tm = (tl+tr) >> 1;
        if (k <= left_sum) return kth(left, tl, tm, k);
        return kth(st[v].r, tm+1, tr, k - left_sum);
    }

    /** Returns the smallest position p with prefix sum >= k. Time: O(log n). */
    int kth(int root, int k) const {
        // caller should check 1 <= k <= total(root)
        return kth(root, 1, n, k);
    }
};
