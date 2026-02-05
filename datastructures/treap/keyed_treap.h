#include "../../header.h"
#include "../node_pool.h"

/*
    --- Keyed treap, max-heap by priority ---
    Inorder sorted by key (Comp)
    Heap by pri (larget pri closer to root)
    Duplicate policy: keys <= node go LEFT, keys > node go RIGHT
    Ops Time complexity: O(log n)
*/
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

    // --- basic ---
    void reset() { root = nullptr; pool.reset_keep_memory(); }
    int size() const { return size(root); }
    X prod_all() const { return prod(root); }

    Node* build_cartesian(const vector<X>& a) {
        if (a.empty()) return nullptr;
        vector<Node*> st;
        st.reserve(a.size());
        for (int i = 0; i < (int)a.size(); i++) {
            Node* cur = pool.create(a[i], splitmix64());
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

    // O(log n)
    // Range aggregate over keys in [l, r)
    X prod_range(const Key&L, const Key& R) {
        Node *a, *b, *c;
        tie(a, b) = split_lt(root, L); // a: < L, b: >= L
        tie(b, c) = split_lt(b, R);    // b: [L, R), c: >= R
        X ans = prod(b);
        root = merge3(a, b, c);
        return ans;
    }

    // Range aggregate over keys in [l, r]
    X prod_range_inclusive(Node*& root, const Key& L, const Key& R) {
        if (comp(R, L)) return Monoid::id();
        Node* a, *b, *c;
        tie(a, b) = split_lt(root, L);
        tie(b, c) = split_leq(b, R);
        X ans = prod(b);
        root = merge3(a, b, c);
        return ans;
    }

    // Prefix prod: nodes with key <= k
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

    // multiset insert: rotation-based
    void insert(const Key& k, const X& v) { root = ins(root, k, v, (u32)splitmix64()); }
    void insert_with_pri(const Key& k, const X& v, u32 p) { root = ins(root, k, v, p); }

    void add(Node*& root, const Key& key, const X& delta) {
        Node *a, *bc, *b, *c;
        tie(a, bc) = split_lt(root, key);
        tie(b, c) = split_leq(bc, key);
        if (!b) {
            b = pool.create(key, delta, splitmix64());
        } else {
            b->cnt++;
            b->x = Monoid::op(b->x, delta);
            pull(b);
        }
        root = merge(a, merge(b, c));
    }

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

    int count_leq(Node* t, const Key& key) {
        if (!t) return 0;
        if (key < t->key) {
            return count_leq(t->l, key);
        } else {
            return size(t->l) + t->cnt + count_leq(t->r, key);
        }
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

    // #keys < k
    int order_of_key(const Key& k) const {
        Node* t = root;
        int res = 0;
        while (t) {
            if (comp(t->key, k)) { // t->key < k
                res += size(t->l) + t->cnt;
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
            return {a, b};
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
