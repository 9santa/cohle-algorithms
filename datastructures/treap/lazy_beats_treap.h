#include "../../header.h"

struct Lazy {
    bool rev;  // toggles order (swap children)
    ll val;    // val is the increment amount
    bool inc;  // bool for whether this tag increments (inc = false means set)
    int kval;
    bool kinc;
    ll mn, mx; // pending lower/upper bounds: after applying the tag, values should be <= mn (chmin), >= mx (chmax)
    ll add;    // pending addition to segment

    // composes two lazy tags: apply 'oth' after this
    void operator+=(const Lazy& oth) {
        rev ^= oth.rev;

        if (!oth.inc) val = 0, inc = false;
        val += oth.val;

        if (!oth.kinc) kval = 0, kinc = false;
        kval += oth.kval;

        // 'beats' clamp with pending add:
        // we store mn/mx with bounds in the same coord system as values AFTER add
        // i.e. chmin/chmax respect pending add, as if it is applied
        if (oth.mn - add <= mx) mn = mx = oth.mn - add;
        else if (oth.mx - add >= mn) mn = mx = oth.mx - add;
        else {
            mn = min(mn, oth.mn - add);
            mx = max(mx, oth.mx - add);
        }
        add += oth.add;
    }
};

// Lazy identity
const Lazy lid = {false, 0, true, 0, true, (ll)4e18, (ll)-4e18, 0};

Lazy chmin_tag(ll x) { Lazy t = lid; t.mn = x; return t; }
Lazy chmax_tag(ll x) { Lazy t = lid; t.mx = x; return t; }
Lazy add_tag(ll x) { Lazy t = lid; t.add = x; return t; }
Lazy rev_tag() { Lazy t = lid; t.rev = true; return t; }

struct Value {
    int ksum;
    ll sum;
    ll mx, mxcnt, mx2;
    ll mn, mncnt, mn2;

    static Value make(ll x, ll len = 1) {
        return {0, x*len, x, len, (ll)-4e18, x, len, (ll)4e18};
    }

    // Tag does nothing on this segment => stop recursion
    bool can_break(const Lazy& lazy) const {
        return lazy.mn >= mx && lazy.mx <= mn && lazy.add == 0;
    }

    // Safe to apply tag without descending
    bool can_tag(const Lazy& lazy) const {
        return mx2 < lazy.mn && mn2 > lazy.mx;
    }

    // Apply lazy to this segment, O(1)
    void apply(Lazy lazy, int sz) {
        if (!lazy.kinc) ksum = 0;
        ksum += lazy.val * sz;

        // apply chmin/chmax
        if (mn == mx) {
            mn = mx = min(lazy.mn, mn);
            mn = mx = max(lazy.mx, mn);
            sum = mn * mncnt;
        } else if (mn == mx2) {
            if (lazy.mx > mn) mn = mx2 = lazy.mx;
            if (lazy.mn < mx) mx = mn2 = lazy.mn;
            sum = mn * mncnt + mx * mxcnt;
        } else {
            if (lazy.mn < mx) sum -= (mx - lazy.mn) * mxcnt, mx = lazy.mn;
            if (lazy.mx > mn) sum += (lazy.mx - mn) * mncnt, mn = lazy.mx;
        }

        // apply add
        sum += lazy.add * sz;
        mx += lazy.add, mx2 += lazy.add;
        mn += lazy.add, mn2 += lazy.add;
    }

    Value operator+(const Value& oth) const {
        Value res{};
        res.ksum = ksum + oth.ksum;
        res.sum = sum + oth.sum;

        // max merge
        if (mx == oth.mx) {
            res.mx = mx;
            res.mxcnt = mxcnt + oth.mxcnt;
            res.mx2 = max(mx2, oth.mx2);
        } else if (mx > oth.mx) {
            res.mx = mx;
            res.mxcnt = mxcnt;
            res.mx2 = max(mx2, oth.mx);
        } else {
            res.mx = oth.mx;
            res.mxcnt = oth.mxcnt;
            res.mx2 = max(mx, oth.mx2);
        }

        // min merge
        if (mn == oth.mn) {
            res.mn = mn;
            res.mncnt = mncnt + oth.mncnt;
            res.mn2 = min(mn2, oth.mn2);
        } else if (mn < oth.mn) {
            res.mn = mn;
            res.mncnt = mncnt;
            res.mn2 = min(mn2, oth.mn);
        } else {
            res.mn = oth.mn;
            res.mncnt = oth.mncnt;
            res.mn2 = min(mn, oth.mn2);
        }

        return res;
    }
};

// Value identity
const Value vid = {0, 0, (ll)-4e18, 0, (ll)-4e18, (ll)4e18, 0, (ll)4e18};

// -------------------------------------------------
// Implicit Treap Beats: range ops by index [l, r)
// -------------------------------------------------
template<class Value, class Lazy>
struct Implicit_Treap_Beats {
    using X = Value;
    using A = Lazy;

    struct Node {
        X val; // single element value
        X agg; // subtree aggregate
        A lz;  // pending lazy to push to children (x/agg are getting updated when lazy is applied to the node)

        int siz;
        u32 pri;
        Node *l, *r;

        Node(const X& v, u32 p) : val(v), agg(v), lz(lid), siz(1), pri(p), l(nullptr), r(nullptr) {}
    };

    using np = Node*;
    np root = nullptr;

    u64 seed = (u64)chrono::steady_clock::now().time_since_epoch().count();
    u64 rnd() {
        seed += 0x9e3779b97f4a7c15ULL;
        u64 x = seed;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    static int siz(np t) { return t ? t->siz : 0; }
    int siz() const { return siz(root); }
    static X agg(np t) { return t ? t->agg : vid; }

    static bool is_lid(const A& z) {
        return !z.rev && z.add == 0 && z.mn == lid.mn && z.mx == lid.mx &&
               z.val == 0 && z.kval == 0 && z.inc && z.kinc;
    }

    void build(const V<ll>& a) {
        clear();
        if (a.empty()) return;
        V<np> st;
        st.reserve(sz(a));
        for (ll v : a) {
            np cur = new Node(X::make(v), rnd());
            np last = nullptr;
            while (!st.empty() && st.back()->pri < cur->pri) {
                last = st.back();
                st.pop_back();
                pull(last);
            }
            cur->l = last;
            if (!st.empty()) st.back()->r = cur;
            st.push_back(cur);
        }
        root = st.front(); // root is bottom of stack

        // pull all nodes in reverse topolgical order via iterative postorder
        V<np> order;
        order.reserve(sz(a));
        {
            stack<np> s;
            s.push(root);
            while (!s.empty()) {
                np t = s.top(); s.pop();
                order.pb(t);
                if (t->l) s.push(t->l);
                if (t->r) s.push(t->r);
            }
        }
        R0F(i, sz(order)) pull(order[i]);
    }

    // apply tag to a node in O(1)
    // - update x and agg immediately (so queries can read values without forcing a full push)
    // - accumulate tag into lz for future propagation
    static void apply_at(np t, const A& z) {
        if (!t || is_lid(z)) return;
        t->val.apply(z, 1);
        t->agg.apply(z, siz(t));
        t->lz += z;
    }

    // Push lazy tag down 1 level:
    // 1) apply tag to this node
    // 2) if rev: swap children
    // 3) propagate tag to children (composition)
    static void push(np t) {
        if (!t || is_lid(t->lz)) return;
        // if reverse: swap children
        if (t->lz.rev) swap(t->l, t->r);
        // propagate tag to children (updates their val/agg immediately)
        if (t->l) apply_at(t->l, t->lz);
        if (t->r) apply_at(t->r, t->lz);
        t->lz = lid;
    }

    // Recompute siz/agg from children + val
    static void pull(np t) {
        if (!t) return;
        t->siz = 1 + siz(t->l) + siz(t->r);
        t->agg = agg(t->l) + t->val + agg(t->r);
    }

    // merge assumes all indices in 'a' come before all indices in 'b'
    static np merge(np a, np b) {
        if (!a || !b) return a ? a : b;
        push(a), push(b);
        if (a->pri > b->pri) {
            a->r = merge(a->r, b);
            pull(a);
            return a;
        } else {
            b->l = merge(a, b->l);
            pull(b);
            return b;
        }
    }

    // split by index: [0,,k) and [k..)
    static pair<np, np> split(np t, int k) {
        if (!t) return { nullptr, nullptr };
        push(t);
        if (k <= siz(t->l)) {
            auto [a, b] = split(t->l, k);
            t->l = b;
            pull(t);
            return {a, t};
        } else {
            auto [a, b] = split(t->r, k - siz(t->l) - 1);
            t->r = a;
            pull(t);
            return {t, b};
        }
    }

    // split out [l, r): returns (A, B, C) where B is [l, r)
    static tuple<np, np, np> split3(np t, int l, int r) {
        auto [ab, c] = split(t, r);     // ab: < r; c: >= r
        auto [a, b] = split(ab, l);  // b: >= l, < r; a < l
        return {a, b, c};
    }

    static np merge3(np a, np b, np c) { return merge(merge(a, b), c); }

    // O(1) reset that leaks resources (like NodePool reset_keep_memory)
    void reset_fast() { root = nullptr; }

    // real delete-all (safe for multitests)
    void clear() { clear_rec(root); root = nullptr; }
    static void clear_rec(np t) {
        if (!t) return;
        push(t);
        clear_rec(t->l);
        clear_rec(t->r);
        delete t;
    }

    // find i-th node (0-index). pushes lazies along path for correctness
    np findi(int i) {
        assert(0 <= i && i < siz());
        np t = root;
        while (true) {
            push(t);
            int L = siz(t->l);
            if (i == L) return t;
            if (i < L) t = t->l;
            else i -= L+1, t = t->r;
        }
    }

    // point get (as Value)
    X get_val(int i) { return findi(i)->val; }

    void set_ll(int i, ll v) {
        modifyi(i, [&](np n) {
            n->val = X::make(v);
        });
    }

    // modify i-th node with a callback op(np)
    template<class Op>
    void modifyi(int i, Op op) {
        assert(0 <= i && i < siz());
        auto [a, bc] = split(root, i);
        auto [b, c] = split(bc, 1);
        // b is the single node
        push(b);
        op(b);
        pull(b);
        root = merge3(a, b, c);
    }

    // Beats apply on a whole subtree
    // if can_break => nothing changes
    // if can_tag => apply lazily at this node in O(1) and stop
    // Else => recurse to children, then fix aggregates on the way back up
    static void apply_beats(np t, A tag) {
        if (!t) return;
        push(t);
        if (t->agg.can_break(tag)) return;
        if (t->agg.can_tag(tag)) {
            apply_at(t, tag);
            return;
        }
        apply_beats(t->l, tag);
        apply_beats(t->r, tag);
        t->val.apply(tag, 1);
        pull(t);
    }

    // In-place beats apply on a subrange [lo, hi) within this subtree
    // lo/hi are relative indices inside t
    static void applyi(np t, int lo, int hi, const A& z) {
        if (!t || lo >= siz(t) || hi <= 0) return;
        if (t->agg.can_break(z)) return;

        if (lo <= 0 && siz(t) <= hi && t->agg.can_tag()) {
            apply_at(t, z);
            return;
        }

        push(t);
        int L = siz(t->l);
        if (lo <= L && L < hi) t->val.apply(z, 1);

        applyi(t->l, lo, min(hi, L), z);
        applyi(t->r, lo - (L+1), hi - (L+1), z);
        pull(t);
    }

    static X queryi(np t, int lo, int hi) {
        if (!t || lo >= siz(t) || hi <= 0) return vid;
        if (lo <= 0 && siz(t) <= hi) return t->agg;
        push(t);
        int L = siz(t->l);
        X res = vid;
        res = res + queryi(t->l, lo, min(hi, L));
        if (lo <= L && L < hi) res = res + t->val;
        res = res + queryi(t->r, lo - (L+1), hi - (L+1));
        return res;
    }

    void insert(int i, ll x) {
        // Node made from scalar x
        np nd = new Node(X::make(x), rnd());
        auto [a, b] = split(root, i);
        root = merge(merge(a, nd), b);
    }

    // erase index i, returns erased Value
    X erase(int i) {
        assert(0 <= i && i < siz());
        auto [a, bc] = split(root, i);
        auto [b, c] = split(bc, 1);
        push(b);
        X ret = b->val;
        delete b;
        root = merge(a, c);
        return ret;
    }

    // rotate sequence so index i becomes start: [0..i) moves to end (i, i+1, i+2, ..., 0, 1, ..., i-1)
    void rotate(int i) {
        assert(0 <= i && i <= siz());
        auto [a, b] = split(root, i);
        root = merge(b, a);
    }

    // reverse range [l, r)
    void reverse(int l, int r) {
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        A tag = lid;
        tag.rev = true;
        if (b) b->lz += tag;
        root = merge(a, merge(b, c));
    }

    // apply beats tag on range [l, r)
    void apply(int l, int r, A tag) {
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        apply_beats(b, tag);
        root = merge(a, merge(b, c));
    }

    // query aggregate on range [l, r) (split-based)
    X query(int l, int r) {
        assert(0 <= l && l <= r && r <= siz());
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        X res = agg(b);
        root = merge(a, merge(b, c));
        return res;
    }

    // query aggregate on range [l, r) (in-place, no splits)
    X query_fast(int l, int r) {
        assert(0 <= l && l <= r && r <= siz());
        return queryi(root, l, r);
    }
};
