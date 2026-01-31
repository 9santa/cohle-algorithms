#include "../../header.h"
#include "../node_pool.h"

// Keyed trep, max-heap by priority
// Duplicate policy: keys <= node go LEFT, kets > node go RIGHT
// Ops Time complexity: O(log n)
template<typename Key, class Monoid, class Comp = std::less<Key>>
struct Treap_Rotate {
    using X = typename Monoid::value_type;

    struct Node {
        Key key;
        u32 pri;   // heap prio (higher = closer to root)
        int size;  // subtree size
        X x;       // value at node
        X prod;    // monoid product of subtree in inorder (key order)
        Node *l, *r;

        Node(const Key&k, const X& v, u32 p) : key(k), pri(p), size(1), x(v), prod(v), l(nullptr), r(nullptr) {}
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

    void reset() { root = nullptr; pool.reset_keep_memory(); }

    int size() const { return size(root); }
    X prod_all() const { return prod(root); }

    // O(log n)
    // Range aggregate over keys in [l, r)
    X prod_range(const Key&L, const Key& R) {
        Node *a, *b, *c;
        tie(a, b) = split_lt(root, L); // a: < L, b: >= L
        tie(b, c) = split_lt(b, R);    // b: [L, R), c: >= R
        X ans = prod(b);
        root = merge(a, merge(b, c));
        return ans;
    }

    // multiset insert: rotation-based
    void insert(const Key& k, const X& v) { root = ins(root, k, v, splitmix64()); }
    void insert_with_pri(const Key& k, const X& v, u32 p) { root = ins(root, k, v, p); }

    // Erase one occurrence by key. Returns whether erased
    bool erase_one(const Key& k) {
        bool erased = false;
        root = era(root, k, erased);
        return erased;
    }

    // Count occurrences by key
    int count(const Key& k) {
        Node* a, *bc, *b, *c;
        tie(a, bc) = split_lt(root, k); // < k, >= k
        tie(b, c) = split_leq(bc, k); // <= k, > k (so b is == k because bc had >= k)
        int ans = size(b);
        root = merge(a, merge(b, c));
        return ans;
    }

    // lower_bound: first key >= k
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

private:
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

    // Rotation-based insert:
    // 1) BST insert by key
    // 2) rotate if child priority exceeds parent (for max-heap)
    Node* ins(Node* t, const Key& k, const X& v, u32 p) {
        if (!t) return pool.create(k, v, p);
        if (leq(k, t->key)) {
            t->l = ins(t->l, k, v, p);
            if (t->l->pri > t->pri) t = rotR(t);
        } else {
            t->r = ins(t->r, k, v, p);
            if (t->r->pri > t->pri) t = rotL(t);
        }
        pull(t);
        return t;
    }

    // Rotation-based erase-one:
    // Find a node with key=k; then rotate it down until it becomes a leaf
    // then remove it
    Node* era(Node* t, const Key& k, bool& erased) {
        if (!t) return nullptr;
        if (!erased && equal(t->key, k)) { // found the key=k
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
                t->r = era(t->r, k, erased);
            } else {
                t = rotL(t);
                t->l = era(t->l, k, erased);
            }
            pull(t);
            return t;
        }
        // standard BST-descent
        if (leq(k, t->key)) t->l = era(t->l, k, erased);
        else t->r = era(t->r, k, erased);
        pull(t);
        return t;
    }

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

    // split_leq = split_less_or_equal: (<= key) and (> key)
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
};
