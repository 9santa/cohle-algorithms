#include "../../header.h"
#include "../node_pool.h"

// Sequence treap (indices 0..n-1), Monoid
// point set/get, insert/erase, range prod, max_right, min_left
// Uses Node Pool, O(n) Memory
template<class Monoid>
struct Implicit_Treap {
    using X = typename Monoid::value_type;

    struct Node {
        X x, prod;
        int size;
        u32 pri;
        Node *l, *r;
        Node(const X& _x, u32 _pri) : x(_x), prod(_x), size(1), pri(_pri), l(nullptr), r(nullptr) {}
    };

    NodePool<Node> pool;
    Node* root = nullptr;

    // somewhat fast RNG
    u64 seed = (u64)chrono::steady_clock::now().time_since_epoch().count();
    u64 splitmix64() {
        seed += 0x9e3779b97f4a7c15ULL;
        u64 x = seed;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    Implicit_Treap() = default;
    Implicit_Treap(int n) { build(n); }
    template<typename F>
    Implicit_Treap(int n, F f) { build(n, f); }

    void reset() {
        root = nullptr;
        pool.reset_keep_memory();
    }

    // O(n)
    void build(int n) { build(n, [](int){ return Monoid::id(); }); }

    template<typename F>
    void build(int n, F f) {
        vector<X> a(n);
        for (int i = 0; i < n; i++) a[i] = f(i);
        build(a);
    }

    void build(const vector<X>& a) {
        reset();
        root = build_cartesian(a);
    }

    int size() const { return size(root); }

    // O(log n)
    X get(int pos) {
        assert(0 <= pos && pos < size());
        auto [a, b] = split(root, pos);
        auto [c, d] = split(b, 1);
        X res = c->x;
        root = merge(a, merge(c, d));
        return res;
    }

    // O(log n)
    void set(int pos, const X& val) {
        assert(0 <= pos && pos < size());
        auto [a, b] = split(root, pos);
        auto [c, d] = split(b, 1);
        c->x = val;
        pull(c);
        root = merge(a, merge(c, d));
    }

    // O(log n)
    void insert(int pos, const X& x) {
        assert(0 <= pos && pos <= size());
        auto [a, b] = split(root, pos);
        Node* nd = pool.create(x, splitmix64());
        root = merge(merge(a, nd), b);
    }

    // O(log n)
    void erase(int pos) {
        assert(0 <= pos && pos < size());
        auto [a, b] = split(root, pos);
        auto [c, d] = split(b, 1);
        c->l = c->r = nullptr;
        pool.destroy(c);
        root = merge(a, d);
    }

    // [l, r), O(log n)
    X prod(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        auto [a, b] = split(root, l);
        auto [c, d] = split(b, r - l);
        X res = prod_all(c);
        root = merge(a, merge(c, d));
        return res;
    }

    X prod_all() { return prod_all(root); }

    // O(log n)
    // check(id) = true and monotone under appending
    template<typename F>
    int max_right(const F check, int l) {
        assert(0 <= l && l <= size());
        assert(check(Monoid::id()));
        auto [a, b] = split(root, l);

        X sm = Monoid::id();
        int len = max_right_dfs(b, sm, check);
        root = merge(a, b);
        return l + len;
    }

    // O(log n)
    // check(id) = true and monotone under appending
    template<typename F>
    int min_left(const F check, int r) {
        assert(0 <= r && r <= size());
        assert(check(Monoid::id()));
        auto [a, b] = split(root, r);

        X sm = Monoid::id();
        int len = min_left_dfs(a, sm, check);
        root = merge(a, b);
        return r - len;
    }

    // O(n)
    vector<X> get_all() {
        vector<X> res;
        res.reserve(size());
        inorder(root, res);
        return res;
    }

private:
    static int size(Node* t) { return t ? t->size : 0; }
    static X prod_all(Node* t) { return t ? t->prod : Monoid::id(); }

    static void pull(Node* t) {
        if (!t) return;
        t->size = 1 + size(t->l) + size(t->r);
        t->prod = Monoid::op(Monoid::op(prod_all(t->l), t->x), prod_all(t->r));
    }

    pair<Node*, Node*> split(Node* t, int k) {
        if (!t) return { nullptr, nullptr };
        // push(t);
        if (size(t->l) >= k) {
            auto [a, b] = split(t->l, k);
            t->l = b;
            pull(t);
            return {a, t};
        } else {
            auto [a, b] = split(t->r, k - size(t->l) - 1);
            t->r = a;
            pull(t);
            return {t, b};
        }
    }

    Node* merge(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;
        if (a->pri < b->pri) {
            push(a);
            a->r = merge(a->r, b);
            pull(a);
            return a;
        } else {
            push(b);
            b->l = merge(a, b->l);
            pull(b);
            return b;
        }
    }

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

    template<typename F>
    int max_right_dfs(Node* t, X& sm, const F& check) {
        if (!t) return 0;
        // push(t);
        X candL = Monoid::op(sm, prod_all(t->l));
        if (check(candL)) {
            sm = candL;
            X candM = Monoid::op(sm, t->x);
            if (check(candM)) {
                sm = candM;
                return size(t->l) + 1 + max_right_dfs(t->r, sm, check);
            } else {
                return size(t->l);
            }
        } else {
            return max_right_dfs(t->l, sm, check);
        }
    }

    template<typename F>
    int min_left_dfs(Node* t, X& sm, const F& check) {
        if (!t) return 0;
        // push(t);
        X candR = Monoid::op(prod_all(t->r), sm);
        if (check(candR)) {
            sm = candR;
            X candM = Monoid::op(t->x, sm);
            if (check(candM)) {
                sm = candM;
                return size(t->r) + 1 + min_left_dfs(t->l, sm, check);
            } else {
                return size(t->r);
            }
        } else {
            return min_left_dfs(t->r, sm, check);
        }
    }

    void inorder(Node* t, vector<X>& out) {
        if (!t) return;
        // push(t);
        inorder(t->l, out);
        out.push_back(t->x);
        inorder(t->r, out);
    }
};
