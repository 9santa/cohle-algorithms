#pragma once
#include <bits/stdc++.h>
using namespace std;

using u32 = unsigned int;

/** Chunked arena allocator with a free list for Node objects. Space: O(allocated chunks). */
template<class Node, int CHUNK_BITS = 12> // 2^12 = 4096 slots per chunk
struct NodePool {
    // static -> one per template instantiation
    static constexpr int CHUNK_SIZE = 1 << CHUNK_BITS;

    // Each slot can hold one Node object
    struct Slot {
        union { // union -> only one member "active" at a time
            Slot* next; // point free-list when free
            alignas(Node) std::byte storage[sizeof(Node)]; // when occupied, raw bytes to host a Node
        };
    };

    vector<Slot*> chunks;      // pointers to each allocated chunk
    Slot* cur = nullptr;       // pointer to the currect chunk's base
    int cur_used = 0;          // how many slots in cur chunk are used
    Slot* free_head = nullptr; // head of the free-list, when you destroy nodes, their slots get pushed here for reuse

    NodePool() { add_chunk(); }

    ~NodePool() {
        for (Slot* p : chunks) {
            ::operator delete(p, std::align_val_t(alignof(Slot)));
        }
    }

    // Delete copying
    NodePool(const NodePool&) = delete;
    NodePool& operator=(const NodePool&) = delete;

    /** Resets allocation pointers without destroying live nodes. Time: O(1). */
    void reset_keep_memory() {
        free_head = nullptr;
        if (!chunks.empty()) {
            cur = chunks[0];
            cur_used = 0;
        }
    }

    /** Frees all chunks and allocates a fresh first chunk. Time: O(number of chunks). */
    void release() {
        for (Slot* p : chunks) {
            ::operator delete(p, std::align_val_t(alignof(Slot)));
        }
        chunks.clear();
        cur = nullptr;
        cur_used = 0;
        free_head = nullptr;
        add_chunk();
    }

    /** Ensures capacity for at least n nodes. Time: O(extra chunks). */
    void reserve_nodes(int n) {
        int need_chunks = (n + CHUNK_SIZE - 1) / CHUNK_SIZE;
        while ((int)chunks.size() < need_chunks) add_chunk();
    }

    /** Constructs a node in pooled storage. Amortized time: O(1). */
    template<class... Args>
    Node* create(Args&&... args) {
        Slot* s = acquire_slot();
        return ::new ((void*)s) Node(std::forward<Args>(args)...);
    }

    /** Clones x into pooled storage, or returns nullptr for nullptr. Amortized time: O(1). */
    Node* clone(const Node* x) {
        if (!x) return nullptr;
        Slot* s = acquire_slot();
        return ::new ((void*)s) Node(*x);
    }

    /** Destroys x and returns its slot to the free list. Time: O(1). */
    void destroy(Node* x) {
        if (!x) return;
        x->~Node(); // run the destructor (can't 'delete x' cos didn't allocate with 'new')
        Slot* s = reinterpret_cast<Slot*>(x);
        // Push slot to the free-list
        s->next = free_head;
        free_head = s;
    }

private:
    void add_chunk() {
        auto* mem = (Slot*)::operator new(sizeof(Slot) * CHUNK_SIZE, std::align_val_t(alignof(Slot)));
        chunks.push_back(mem);
        cur = mem;
        cur_used = 0;
    }

    Slot* acquire_slot() {
        if (free_head) {
            Slot* s = free_head;
            free_head = free_head->next;
            return s;
        }
        if (cur_used == CHUNK_SIZE) add_chunk();
        return &cur[cur_used++];
    }
};


/** Randomized binary search tree sequence over a monoid. Space: O(n). */
template<class Monoid, bool PERSISTENT = false>
struct RBST_Seq_Monoid {
    using X = Monoid::value_type;
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

    /** Splits into [0, k) and [k, n). Expected time: O(log n). */
    pair<np, np> split(np t, int k) {
        if (!t) return { nullptr, nullptr };
        t = clone(t);
        push(t);
        int sl = size(t->l);
        if (k <= sl) {
            auto [a, b] = split(t->l, k);
            t->l = b;
            pull(t);
            return {a, t};
        } else {
            auto [a, b] = split(t->r, k-sl-1);
            t->r = a;
            pull(t);
            return {t, b};
        }
    }

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

    /** Inserts x before pos and returns the new root. Expected time: O(log n). */
    np insert(np t, int pos, const X& x) {
        auto [a, b] = split(t, pos);
        return merge(merge(a, new_node(x)), b);
    }

    /** Erases a[pos] and returns the new root. Expected time: O(log n). */
    np erase(np t, int pos) {
        auto [a, b] = split(t, pos);
        auto [c, d] = split(b, 1);
        // skip destroy when persistent
        if (!PERSISTENT) pool.destroy(c);
        return merge(a, d);
    }

    /** Returns a[pos]. Expected time: O(log n). */
    X get(np t, int pos) {
        auto [a, b] = split(t, pos);
        auto [c, d] = split(b, 1);
        X res = c ? c->x : Monoid::id();
        t = merge(a, merge(c, d));
        return res;
    }

    /** Sets a[pos] = x and returns the new root. Expected time: O(log n). */
    np set(np t, int pos, const X& x) {
        auto [a, b] = split(t, pos);
        auto [c, d] = split(b, 1);
        c = clone(c);
        c->x = x;
        pull(c);
        return merge(a, merge(c, d));
    }

    /** Returns the monoid product on [l, r). Expected time: O(log n). */
    X prod(np t, int l, int r) {
        auto [a, b] = split(t, l);
        auto [c, d] = split(b, r-l);
        X res = prod(c);
        t = merge(a, merge(c, d));
        return res;
    }

    /** Reverses [l, r) and returns the new root. Expected time: O(log n). */
    np reverse(np t, int l, int r) {
        auto [a, b] = split(t, l);
        auto [c, d] = split(b, r-l);
        c = clone(c);
        apply_rev(c);
        return merge(a, merge(c, d));
    }
};
