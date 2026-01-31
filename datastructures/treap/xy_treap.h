#include "../../header.h"
#include "../node_pool.h"

// Standard treap for multiset of points (X, Y): BST by X, max-heap by Y.
// Duplicate policy: left <= X0, right > X0, (duplicates go LEFT).
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

    void reset() {
        root = nullptr;
        pool.reset_keep_memory();
    }

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

    // O(log n)
    // insert (X with optional Y). Random priority if Y isn't provided
    void insert(const T& x, u32 y) {
        Node* nd = pool.create(x, y);
        auto [a, b] = split_leq(root, x); // dups go left
        root = merge(merge(a, nd), b);
    }
    void insert(const T& x) { insert(x, splitmix64()); }

    // O(log n)
    // erase one occurence of key (multiset erase-one)
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

    // O(log n)
    // lower_bound: first element with x >= key
    // Returns pointer to node (do not store if treap will change)
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

    // count occurences (multiset)
    int count(const T& key) {
        auto [a, bc] = split_lt(root, key);
        auto [b, c] = split_leq(bc, key);
        int res = size(b);
        root = merge(a, merge(b, c));
        return res;
    }
};
