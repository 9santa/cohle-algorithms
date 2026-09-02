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


/** Treap with explicit key x and priority y. Space: O(n). */
template<typename T, class Comp = std::less<T>>
struct TreapXY {
    struct Node {
        T x;
        u32 y; // priority if not provided
        int size;
        Node *l, *r;
        Node(const T& _x, u32 _y) : x(_x), y(_y), size(1), l(nullptr), r(nullptr) {}
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
    void reset() {
        root = nullptr;
        pool.reset_keep_memory();
    }

    /** Returns number of keys. Time: O(1). */
    int size() const { return size(root); }

    // helpers
    int size(Node* t) const { return t ? t->size : 0; }

    void pull(Node* t) {
        if (!t) return;
        t->size = 1 + size(t->l) + size(t->r);
    }

    // a <= b with comparator
    bool leq(const T& a, const T& b) const {
        return !comp(b, a); // !(b < a)
    }

    // O(log n)
    // split by key (dups go left): [<= key] and [> key]
    pair<Node*, Node*> split_leq(Node* t, const T& key) {
        if (!t) return { nullptr, nullptr };
        if (leq(t->x, key)) {
            // t->l, t are <= key; t->r MIGHT contain elements both <= key and > key
            auto [a, b] = split_leq(t->r, key);
            t->r = a;
            pull(t);
            return {t, b};
        } else {
            auto [a, b] = split_leq(t->l, key);
            t->l = b;
            pull(t);
            return {a, t};
        }
    }

    // O(log n)
    // split by key (< key): [<key] and [>= key]
    pair<Node*, Node*> split_lt(Node* t, const T& key) {
        if (!t) return { nullptr, nullptr };
        if (comp(t->x, key)) { // t->x < key
            auto [a, b] = split_lt(t->r, key);
            t->r = a;
            pull(t);
            return {t, b};
        } else {
            auto [a, b] = split_lt(t->l, key);
            t->l = b;
            pull(t);
            return {a, t};
        }
    }

    // O(log n)
    // merge: assumes all keys in a <= all keys in b (under chosen policy)
    Node* merge(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;
        // max-heap by y: root is bigger y
        if (a->y >= b->y) {
            a->r = merge(a->r, b);
            pull(a);
            return a;
        } else {
            b->l = merge(a, b->l);
            pull(b);
            return b;
        }
    }

    /** Inserts x with priority y. Expected time: O(log n). */
    void insert(const T& x, u32 y) {
        Node* nd = pool.create(x, y);
        auto [a, b] = split_leq(root, x); // dups go left
        root = merge(merge(a, nd), b);
    }
    /** Inserts x with generated priority. Expected time: O(log n). */
    void insert(const T& x) { insert(x, splitmix64()); }

    /** Erases one occurrence of key, if present. Expected time: O(log n). */
    bool erase_one(const T& key) {
        auto [a, bc] = split_lt(root, key); // a < key, bc >= key
        auto [b, c] = split_leq(bc, key);   // b == key, c > key
        if (!b) {
            root = merge(a, merge(b, c));
            return false;
        }
        Node* del = b;
        b = merge(b->l, b->r);
        pool.destroy(del);
        root = merge(a, merge(b, c));
        return true;
    }

    /** Returns first node with x >= key, or nullptr. Expected time: O(log n). */
    Node* lower_bound(const T& key) const {
        Node* t = root;
        Node* ans = nullptr;
        while (t) {
            if (!comp(t->x, key)) { // t->x >= k
                ans = t;
                t = t->l;
            } else {
                t = t->r;
            }
        }
        return ans;
    }

    /** Counts occurrences of key. Expected time: O(log n). */
    int count(const T& key) {
        auto [a, bc] = split_lt(root, key);
        auto [b, c] = split_leq(bc, key);
        int res = size(b);
        root = merge(a, merge(b, c));
        return res;
    }
};
