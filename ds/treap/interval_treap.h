#pragma once
#include <bits/stdc++.h>
using namespace std;

using u64 = unsigned long long;
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


/** Treap-backed interval tree keyed by interval starts. Space: O(n). */
template<class Coord, class Comp = std::less<Coord>>
struct Interval_Treap {
    struct Node {
        Coord L, R; // interval [L, R)
        Coord mxR;  // max R in subtree
        u32 pri;   // heap prio (higher = closer to root)
        int size;  // subtree size
        Node *l, *r;

        Node(const Coord& _L, const Coord& _R, u32 _pri) : L(_L), R(_R), mxR(_R), pri(_pri), l(nullptr), r(nullptr) {}
    };

    NodePool<Node> pool;
    Node* root = nullptr;
    Comp comp;

    // somewhat fast RNG
    u64 seed = (u64)chrono::steady_clock::now().time_since_epoch().count();
    u64 splitmix64() {
        seed += 0x9e3779b97f4a7c15ULL;
        u64 x = seed;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    /** Clears the tree while keeping pooled memory. Time: O(1). */
    void reset() { root = nullptr; pool.reset_keep_memory(); }

    /** Inserts interval [L, R), assuming unique L. Expected time: O(log n). */
    void insert(Coord L, Coord R) {
        assert(!comp(R, L) && "require L <= R");
        root = ins(root, pool.create(L, R, splitmix64()));
    }

    /** Erases the interval with start L, if present. Expected time: O(log n). */
    bool erase_by_L(const Coord& L) {
        bool erased = false;
        root = era(root, L, erased);
        return erased;
    }

    /** Returns an interval containing x, or nullptr. Expected time: O(log n). */
    Node* find_containing(const Coord& x) const {
        Node* t = root;
        while (t) {
            // if left subtree has some interval with R > x, the answer might be there
            if (t->l && comp(x, t->l->mxR)) { t = t->l; continue; }
            // check current interval: L <= X < R
            if (!comp(x, t->L) && comp(x, t->R)) return t;
            t = t->r;
        }
        return nullptr;
    }

    /** Returns whether any interval contains x. Expected time: O(log n). */
    bool contains_point(const Coord& x) const { return find_containing(x) != nullptr; }

    /** Returns whether any interval overlaps [L, R). Expected time: O(log n). */
    bool any_overlap(const Coord& L, const Coord& R) const {
        return any_overlap_rec(root, L, R);
    }

private:
    static Coord mx(Node* t) { return t ? t->mxR : Coord(); }

    void pull(Node* t) {
        if (!t) return;
        t->mxR = t->R;
        if (t->l && comp(t->mxR, t->l->mxR)) t->mxR = t->l->mxR;
        if (t->r && comp(t->mxR, t->r->mxR)) t->mxR = t->r->mxR;
    }

    // Rotate right: bring left child up
    // Rotations preserve in-order traversal sequence
    Node* rotR(Node* t) {
        Node* p = t->l;
        t->l = p->r;
        p->r = t;
        pull(t);
        pull(p);
        return p;
    }

    // Rotate left: bring right child up
    Node* rotL(Node* t) {
        Node* p = t->r;
        t->r = p->l;
        p->l = t;
        pull(t);
        pull(p);
        return p;
    }

    // Rotation-based insert:
    // 1) BST by L
    // 2) rotate if child priority exceeds parent (for max-heap)
    Node* ins(Node* t, Node* nd) {
        if (!t) { pull(nd); return nd; }
        if (comp(nd->L, t->L)) {
            t->l = ins(t->l, nd);
            if (t->l->pri > t->pri) t = rotR(t);
        } else {
            t->r = ins(t->r, nd);
            if (t->r->pri > t->pri) t = rotL(t);
        }
        pull(t);
        return t;
    }

    // Rotation-based erase-one:
    // Find a node with key=k; then rotate it down until it becomes a leaf
    // then remove it
    Node* era(Node* t, const Coord& L, bool& erased) {
        if (!t) return nullptr;
        if (!comp(t->L, L) && !comp(L, t->L)) { // found the key=k
            erased = true;
            // remove t by rotating down
            if (!t->l && !t->r) {
                pool.destroy(t);
                return nullptr;
            }
            if (!t->l) {
                Node* r = t->r;
                pool.destroy(t);
                return r;
            }
            if (!t->r) {
                Node* l = t->l;
                pool.destroy(t);
                return l;
            }
            // both children exist: rotate the higher-priority child up
            if (t->l->pri > t->r->pri) {
                t = rotR(t);
                t->r = era(t->r, L, erased);
            } else {
                t = rotL(t);
                t->l = era(t->l, L, erased);
            }
            pull(t);
            return t;
        }
        // standard BST-descent
        if (comp(L, t->L)) t->l = era(t->l, L, erased);
        else t->r = era(t->r, L, erased);
        pull(t);
        return t;
    }

    bool overlap_one(Node* t, const Coord& L, const Coord& R) const {
        return comp(t->L, R) && comp(L, t->R);
    }

    bool any_overlap_rec(Node* t, const Coord& L, const Coord& R) const {
        if (!t) return false;
        // if left subtree exists and its mxR > L, it might contain an interval overlapping [L, R)
        if (t->l && comp(L, t->l->mxR)) {
            if (any_overlap_rec(t->l, L, R)) return true;
        }
        if (overlap_one(t, L, R)) return true;
        // prune right subtree: if t->L >= R, everything on right starts even later -> no overlap
        if (!comp(t->L, R)) return false;
        return any_overlap_rec(t->r, L, R);
    }
};
