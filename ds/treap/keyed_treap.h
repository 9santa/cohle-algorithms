#pragma once
#include <bits/stdc++.h>
using namespace std;

using u64 = unsigned long long;
using u32 = unsigned int;
#define sz(x) int((x).size())

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


/** Keyed treap with monoid aggregates in sorted key order. Space: O(n). */
template<typename Key, class Monoid, class Comp = std::less<Key>>
struct TreapKeyed {
    using X = typename Monoid::value_type;

    struct Node {
        Key key;
        u32 pri;   // heap prio (higher = closer to root)
        int size;  // subtree size
        X x;       // value at node
        X prod;    // monoid product of subtree in inorder (key order)
        Node *l, *r;

        Node(const Key&k, const X& _x, u32 p) : key(k), pri(p), size(1), x(_x), prod(_x), l(nullptr), r(nullptr) {}
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

    /** Clears the treap while keeping pooled memory. Time: O(1). */
    void reset() { root = nullptr; pool.reset_keep_memory(); }

    /** Returns number of nodes. Time: O(1). */
    int size() const { return size(root); }

    /** Returns aggregate over all keys. Time: O(1). */
    X prod_all() const { return prod(root); }

    Node* build_cartesian(const vector<pair<Key, X>>& a) {
        if (a.empty()) return nullptr;
        vector<Node*> st;
        st.reserve(a.size());
        for (int i = 0; i < (int)a.size(); i++) {
            Node* cur = pool.create(a[i].first, a[i].second, splitmix64());
            Node* last = nullptr;
            while (!st.empty() && st.back()->pri > cur->pri) {
                last = st.back();
                st.pop_back();
            }
            cur->l = last;
            if (!st.empty()) st.back()->r = cur;
            st.push_back(cur);
        }
        Node* rt = st.front();
        function<void(Node*)> dfs = [&](Node* t) {
            if (!t) return;
            dfs(t->l);
            dfs(t->r);
            pull(t);
        };
        dfs(rt);
        return rt;
    }

    /** Returns aggregate over keys in [L, R). Expected time: O(log n). */
    X prod_range(const Key&L, const Key& R) {
        Node *a, *b, *c;
        tie(a, b) = split_lt(root, L); // a: < L, b: >= L
        tie(b, c) = split_lt(b, R);    // b: [L, R), c: >= R
        X ans = prod(b);
        root = merge3(a, b, c);
        return ans;
    }

    /** Returns aggregate over keys in [L, R]. Expected time: O(log n). */
    X prod_range_inclusive(Node*& root, const Key& L, const Key& R) {
        if (comp(R, L)) return Monoid::id();
        Node* a, *b, *c;
        tie(a, b) = split_lt(root, L);
        tie(b, c) = split_leq(b, R);
        X ans = prod(b);
        root = merge3(a, b, c);
        return ans;
    }

    /** Returns aggregate over keys <= k. Expected time: O(log n). */
    X prod_leq(const Key& k) const {
        Node* t = root;
        X res = Monoid::id();
        while (t) {
            if (leq(t->key, k)) { // t->key <= k
                res = Monoid::op(res, prod(t->l));
                res = Monoid::op(res, t->x);
                t = t->r;
            } else {
                t = t->l;
            }
        }
        return res;
    }

    // --- max_right / min_left ---
    template<class Pred>
    int max_right(int l, Pred pred) {
        assert(0 <= l && l <= size());
        assert(pred(Monoid::id()));
        auto [a, b] = split_by_order(root, l);
        X acc = Monoid::id();
        int take = take_prefix(b, acc, pred);
        root = merge(a, b);
        return l + take;
    }

    template<class Pred>
    int min_left(int r, Pred pred) {
        assert(0 <= r && r <= size());
        assert(pred(Monoid::id()));
        auto [a, b] = split_by_order(root, r);
        X acc = Monoid::id();
        int take = take_suffix(a, acc, pred);
        root = merge(a, b);
        return r - take;
    }

    /** Inserts key k with value v. Expected time: O(log n). */
    void insert(const Key& k, const X& v) { root = insert(root, k, v, (u32)splitmix64()); }

    /** Inserts key k with value v and explicit priority p. Expected time: O(log n). */
    void insert_with_pri(const Key& k, const X& v, u32 p) { root = insert(root, k, v, p); }

    void add(Node*& root, const Key& key, const X& delta) {
        Node *a, *bc, *b, *c;
        tie(a, bc) = split_lt(root, key);
        tie(b, c) = split_leq(bc, key);
        if (!b) {
            b = pool.create(key, delta, splitmix64());
        } else {
            b->x = Monoid::op(b->x, delta);
            pull(b);
        }
        root = merge(a, merge(b, c));
    }

    /** Applies Monoid::op at key, inserting it if absent. Expected time: O(log n). */
    void add(const Key& key, const X& delta) { add(root, key, delta); }

    /** Erases one occurrence of key k. Expected time: O(log n). */
    bool erase_one(const Key& k) {
        bool erased = false;
        root = erase_one(root, k, erased);
        return erased;
    }

    /** Counts occurrences of key k. Expected time: O(log n). */
    int count(const Key& k) {
        Node* a, *bc, *b, *c;
        tie(a, bc) = split_lt(root, k); // < k, >= k
        tie(b, c) = split_leq(bc, k); // <= k, > k (so b is == k because bc had >= k)
        int ans = size(b);
        root = merge(a, merge(b, c));
        return ans;
    }

    int count_leq(Node* t, const Key& key) {
        if (!t) return 0;
        if (comp(key, t->key)) {
            return count_leq(t->l, key);
        } else {
            return size(t->l) + 1 + count_leq(t->r, key);
        }
    }

    /** Returns first node with key >= k, or nullptr. Expected time: O(log n). */
    Node* lower_bound(const Key& k) const {
        Node* t = root;
        Node* ans = nullptr;
        while (t) {
            if (!comp(t->key, k)) {
                ans = t;
                t = t->l;
            } else t = t->r;
        }
        return ans;
    }

    /** Returns number of keys < k. Expected time: O(log n). */
    int order_of_key(const Key& k) const {
        Node* t = root;
        int res = 0;
        while (t) {
            if (comp(t->key, k)) { // t->key < k
                res += size(t->l) + 1;
                t = t->r;
            } else {
                t = t->l;
            }
        }
        return res;
    }

private:
    // --- merge/split core ---
    static int size(Node* t) { return t ? t->size : 0; }
    static X prod(Node* t) { return t ? t->prod : Monoid::id(); }

    static void pull(Node* t) {
        if (!t) return;
        t->size = 1 + size(t->l) + size(t->r);
        t->prod = Monoid::op(Monoid::op(prod(t->l), t->x), prod(t->r));
    }

    // Define <= via !(b < a)
    bool leq(const Key& a, const Key& b) const { return !comp(b, a); }
    // Define ==
    bool equal(const Key& a, const Key& b) const { return !comp(a, b) && !comp(b, a); }

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

    // --- insertion / deletion ---

    // Rotation-based insert:
    // 1) BST insert by key
    // 2) rotate if child priority exceeds parent (for max-heap)
    Node* insert(Node* t, const Key& key, const X& v, u32 p) {
        if (!t) return pool.create(key, v, p);
        if (key == t->key) {
            // if keys can repeat, modify policy. here we overwrite
            t->x = Monoid::op(t->x, v);
            pull(t);
            return t;
        }
        if (key <= t->key) {
            t->l = insert(t->l, key, v, p);
            if (t->l->pri > t->pri) t = rotR(t);
        } else {
            t->r = insert(t->r, key, v, p);
            if (t->r->pri > t->pri) t = rotL(t);
        }
        pull(t);
        return t;
    }

    // Rotation-based erase-one:
    // Find a node with key=k; then rotate it down until it becomes a leaf
    // then remove it
    Node* erase_one(Node* t, const Key& k, bool& erased) {
        if (!t) return nullptr;
        if (equal(t->key, k)) { // found the key=k
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
                t->r = erase_one(t->r, k, erased);
            } else {
                t = rotL(t);
                t->l = erase_one(t->l, k, erased);
            }
            pull(t);
            return t;
        }
        // standard BST-descent
        if (leq(k, t->key)) t->l = erase_one(t->l, k, erased);
        else t->r = erase_one(t->r, k, erased);
        pull(t);
        return t;
    }

    // --- split / merge ---

    // split by key; split_lt = split_less_than: (< key) and (>= key)
    pair<Node*, Node*> split_lt(Node* t, const Key& key) {
        if (!t) return { nullptr, nullptr };
        if (comp(t->key, key)) { // node->key < key -> split the right subtree
            auto [a, b] = split_lt(t->r, key); // a: < key, b: >= key
            t->r = a; // cut the tree: now whole t < key
            pull(t);
            return {t, b};
        } else {
            auto [a, b] = split_lt(t->l, key);
            t->l = b;
            pull(t);
            return {a, t};
        }
    }

    // split by key; split_leq = split_less_or_equal: (<= key) and (> key)
    pair<Node*, Node*> split_leq(Node* t, const Key& key) {
        if (!t) return { nullptr, nullptr };
        if (leq(t->key, key)) {
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

    // split by order: [0, k) and [k, n)
    pair<Node*, Node*> split_by_order(Node* t, int k) {
        if (!t) return {nullptr, nullptr};
        if (size(t->l) >= k) {
            auto [a, b] = split_by_order(t->l, k);
            t->l = b;
            pull(t);
            return {a, t};
        } else {
            auto [a, b] = split_by_order(t->r, k - size(t->l) - 1);
            t->r = a;
            pull(t);
            return {t, b};
        }
    }

    // split into [0, l), [l, r), [r, n)
    array<Node*, 3> split3_by_order(Node* t, int l, int r) {
        auto [a, bc] = split_by_order(t, l);
        auto [b, c] = split_by_order(bc, r-l);
        return {a, b, c};
    }

    // k-way split by order cuts (sorted cuts)
    vector<Node*> split_k_by_order(Node* t, const vector<int>& cuts) {
        vector<Node*> res;
        res.reserve(sz(cuts) + 1);
        Node* cur = t;
        int last = 0;
        for (int p : cuts) {
            auto [a, b] = split_by_order(cur, p - last);
            res.push_back(a);
            cur = b;
            last = p;
        }
        res.push_back(cur);
        return res;
    }

    // merge assumes all keys in a <= all keys in b
    Node* merge(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;
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

    Node* merge3(Node* a, Node* b, Node* c) { return merge(merge(a, b), c); }

    Node* merge_all(const vector<Node*>& ts) {
        Node* cur = nullptr;
        for (auto t : ts) cur = merge(cur, t);
        return cur;
    }

    template<class Pred>
    int take_prefix(Node* t, X& acc, Pred pred) {
        if (!t) return 0;
        // try take entire left subtree
        if (t->l) {
            X cand = Monoid::op(acc, prod(t->l));
            if (!pred(cand)) return take_prefix(t->l, acc, pred);
            acc = cand;
        }
        // try take current node
        {
            X cand = Monoid::op(acc, t->x);
            if (!pred(cand)) return size(t->l);
            acc = cand;
        }
        // then take from right
        int res = size(t->l) + 1;
        if (t->r) res += take_prefix(t->r, acc, pred);
        return res;
    }

    template<class Pred>
    int take_suffix(Node* t, X& acc, Pred pred) {
        if (!t) return 0;
        // try take entire right subtree
        if (t->r) {
            X cand = Monoid::op(prod(t->r), acc);
            if (!pred(cand)) return take_suffix(t->r, acc, pred);
            acc = cand;
        }
        // tru take current node
        {
            X cand = Monoid::op(t->x, acc);
            if (!pred(cand)) return size(t->r);
            acc = cand;
        }
        // then take from left
        int res = size(t->r) + 1;
        if (t->l) res += take_suffix(t->l, acc, pred);
        return res;
    }
};
