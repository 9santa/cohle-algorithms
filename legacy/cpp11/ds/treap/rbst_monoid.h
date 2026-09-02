#include "../header.h"
#include "../node_pool.h"

// Randomized Binary Search Tree
// sequence, random merge + Monoid + optional persistence + reverse
// merge(a, b) chooses root from a with prob |a| / (|a| + |b|) else from b

/** Randomized binary search tree sequence over a monoid. Space: O(n). */
template<class Monoid, bool PERSISTENT = false>
struct RBST_Seq_Monoid {
    typedef typename Monoid::value_type X;
    // static_assert(Monoid::commute == false);

    struct Node {
        Node *l, *r;
        X x, prod, rev_prod; // forward and reverse aggregates (for non-commutative monoids)
        int size;
        bool rev; // lazy reverse flag
        Node() : l(nullptr), r(nullptr), x(Monoid::id()), prod(Monoid::id()), rev_prod(Monoid::id()), size(0), rev(false) {}
        Node(const X& _x) : l(nullptr), r(nullptr), x(_x), prod(_x), rev_prod(_x), size(1), rev(false) {}
    };

    NodePool<Node> pool;
    using np = Node*; // node pointer alias

    // xor128 RNG (fast, but fails on BigCrush)
    u32 x=123456789, y=362436069, z=521288629, w=88675123;
    u32 rng() {
        u32 t = x ^ (x << 1);
        x = y, y = z, z = w;
        return w = (w * (w >> 19)) ^ (t ^ (t >> 8));
    }

    /** Clears pooled state. Time: O(1). */
    void reset() { pool.reset_keep_memory(); }

    // pass root, get new root if persistent,
    // or same pointer mutated if not persistent
    /** Allocates a node. Amortized time: O(1). */
    np new_node(const X& x) { return pool.create(x); }

    /** Returns subtree size. Time: O(1). */
    static int size(np t) { return t ? t->size : 0; }
    /** Returns subtree product. Time: O(1). */
    static X prod(np t) { return t ? t->prod : Monoid::id(); }
    /** Returns reversed-order subtree product. Time: O(1). */
    static X rprod(np t) { return t ? t->rev_prod : Monoid::id(); }

    /** Clones a node when persistence is enabled. Amortized time: O(1). */
    np clone(np t) {
        if (!t || !PERSISTENT) return t;
        return pool.clone(t);
    }

    /** Toggles subtree reversal. Time: O(1). */
    static void apply_rev(np t) {
        if (!t) return;
        t->rev ^= 1; // reverse lazy reverse flag
        swap(t->l, t->r);
        swap(t->prod, t->rev_prod);
    }

    // push reverse lazy to children (clone children if persistent)
    void push(np t) {
        if (!t || !t->rev) return;
        if (t->l) { t->l = clone(t->l), apply_rev(t->l); }
        if (t->r) { t->r = clone(t->r), apply_rev(t->r); }
        t->rev = 0;
    }

    // pull one layer from children
    static void pull(np t) {
        if (!t) return;
        t->size = 1 + size(t->l) + size(t->r);
        t->prod = Monoid::op(Monoid::op(prod(t->l), t->x), prod(t->r));
        // reverse: right reversed + x + left reversed
        t->rev_prod = Monoid::op(Monoid::op(rprod(t->r), t->x), rprod(t->l));
    }

    // split by index: [0..k) and [k..)
    /** Splits into [0, k) and [k, n). Expected time: O(log n). */
    pair<np, np> split(np t, int k) {
        if (!t) return { nullptr, nullptr };
        t = clone(t);
        push(t);
        int sl = size(t->l);
        if (k <= sl) {
            // Replaced structured bindings with explicit pair unpacking for C++11.
            pair<np, np> ab = split(t->l, k);
            np a = ab.first;
            np b = ab.second;
            t->l = b;
            pull(t);
            return {a, t};
        } else {
            pair<np, np> ab = split(t->r, k-sl-1);
            np a = ab.first;
            np b = ab.second;
            t->r = a;
            pull(t);
            return {t, b};
        }
    }

    // merge: random root by sizes
    /** Merges two consecutive sequences. Expected time: O(log n). */
    np merge(np a, np b) {
        if (!a) return b;
        if (!b) return a;
        u32 sa = (u32)size(a), sb = (u32)size(b);
        if (rng() % (sa + sb) < sa) {
            a = clone(a);
            push(a);
            a->r = merge(a->r, b);
            pull(a);
            return a;
        } else {
            b = clone(b);
            push(b);
            b->l = merge(a, b->l);
            pull(b);
            return b;
        }
    }

    // sequence ops (all expected O(log n))
    /** Inserts x before pos and returns the new root. Expected time: O(log n). */
    np insert(np t, int pos, const X& x) {
        pair<np, np> ab = split(t, pos);
        np a = ab.first;
        np b = ab.second;
        return merge(merge(a, new_node(x)), b);
    }

    /** Erases a[pos] and returns the new root. Expected time: O(log n). */
    np erase(np t, int pos) {
        pair<np, np> ab = split(t, pos);
        np a = ab.first;
        np b = ab.second;
        pair<np, np> cd = split(b, 1);
        np c = cd.first;
        np d = cd.second;
        // skip destroy when persistent
        if (!PERSISTENT) pool.destroy(c);
        return merge(a, d);
    }

    /** Returns a[pos]. Expected time: O(log n). */
    X get(np t, int pos) {
        pair<np, np> ab = split(t, pos);
        np a = ab.first;
        np b = ab.second;
        pair<np, np> cd = split(b, 1);
        np c = cd.first;
        np d = cd.second;
        X res = c ? c->x : Monoid::id();
        t = merge(a, merge(c, d));
        return res;
    }

    /** Sets a[pos] = x and returns the new root. Expected time: O(log n). */
    np set(np t, int pos, const X& x) {
        pair<np, np> ab = split(t, pos);
        np a = ab.first;
        np b = ab.second;
        pair<np, np> cd = split(b, 1);
        np c = cd.first;
        np d = cd.second;
        c = clone(c);
        c->x = x;
        pull(c);
        return merge(a, merge(c, d));
    }

    // range prod on [l, r)
    /** Returns the monoid product on [l, r). Expected time: O(log n). */
    X prod(np t, int l, int r) {
        pair<np, np> ab = split(t, l);
        np a = ab.first;
        np b = ab.second;
        pair<np, np> cd = split(b, r-l);
        np c = cd.first;
        np d = cd.second;
        X res = prod(c);
        t = merge(a, merge(c, d));
        return res;
    }

    // reverse [l, r)
    /** Reverses [l, r) and returns the new root. Expected time: O(log n). */
    np reverse(np t, int l, int r) {
        pair<np, np> ab = split(t, l);
        np a = ab.first;
        np b = ab.second;
        pair<np, np> cd = split(b, r-l);
        np c = cd.first;
        np d = cd.second;
        c = clone(c);
        apply_rev(c);
        return merge(a, merge(c, d));
    }
};
