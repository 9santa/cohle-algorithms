#pragma once

struct Link_Cut {
    struct Node {
        int ch[2] = {0, 0}; // splay children
        int p = 0; // parent
        int siz = 1; // subtree size
        bool rev = false; // lazy reverse tag

        ll val = 0; // vertex value
        ll sum = 0; // sum on aux splay subtree
        ll add = 0; // lazy add on aux splay subtree
    };

    int n;
    vector<Node> t;

    Link_Cut(int n_ = 0) { init(n_); }

    Link_Cut(const vector<ll>& a) {
        init(sz(a) - 1); // expect 1-indexed a[1..n]
        for (int i = 1; i <= n; i++) {
            t[i].val = t[i].sum = a[i];
        }
    }

    void init(int n_) {
        n = n_;
        t.assign(n+1, Node{});
        for (int i = 1; i <= n; i++) {
            t[i].siz = 1;
            t[i].sum = t[i].val;
        }
    }

    // ===== basic helpers =====

    bool is_root(int x) const {
        int p = t[x].p;
        return p == 0 || (t[p].ch[0] != x && t[p].ch[1] != x);
    }

    int siz(int x) const { return x ? t[x].siz : 0; }

    ll sum(int x) const { return x ? t[x].sum : 0LL; }

    // recalculate parent from its 2 children
    void pull(int x) {
        if (!x) return;
        t[x].siz = 1 + siz(t[x].ch[0]) + siz(t[x].ch[1]);
        t[x].sum = t[x].val + sum(t[x].ch[0]) + sum(t[x].ch[1]);
    }

    void apply_rev(int x) {
        if (!x) return;
        swap(t[x].ch[0], t[x].ch[1]);
        t[x].rev ^= 1; // invert rev flag
    }

    void apply_add(int x, ll delta) {
        if (!x) return;
        t[x].val += delta;
        t[x].sum += delta * t[x].siz;
        t[x].add += delta;
    }

    void push(int x) {
        if (!x) return;

        if (t[x].rev) {
            apply_rev(t[x].ch[0]);
            apply_rev(t[x].ch[1]);
            t[x].rev = false;
        }

        if (t[x].add != 0) {
            apply_add(t[x].ch[0], t[x].add);
            apply_add(t[x].ch[1], t[x].add);
            t[x].add = 0;
        }
    }

    // push full path from root to x
    void push_all(int x) {
        vector<int> stk;
        stk.reserve(32); // log(INT_MAX)
        int y = x;
        stk.push_back(y);
        while (!is_root(y)) {
            y = t[y].p;
            stk.push_back(y);
        }
        for (int i = sz(stk) - 1; i >= 0; i--) push(stk[i]);
    }

    void rotate(int x) {
        int p = t[x].p;
        int g = t[p].p;
        int dirx = (t[p].ch[1] == x); // 0 if x is left child of p, 1 if right child
        int b = t[x].ch[dirx ^ 1]; // dirx is the opposite child, subtree of which will move

        if (!is_root(p)) {
            if (t[g].ch[0] == p) t[g].ch[0] = x;
            else if (t[g].ch[1] == p) t[g].ch[1] = x;
        }
        t[x].p = g;

        t[x].ch[dirx ^ 1] = p;
        t[p].p = x;

        t[p].ch[dirx] = b;
        if (b) t[b].p = p;

        pull(p);
        pull(x);
    }

    void splay(int x) {
        push_all(x);
        while (!is_root(x)) {
            int p = t[x].p;
            int g = t[p].p;
            if (!is_root(p)) {
                bool zigzig = ((t[p].ch[0] == x) == (t[g].ch[0] == p));
                if (zigzig) rotate(p);
                else rotate(x);
            }
            rotate(x);
        }
    }

    // ===== Link Cut Tree core =====

    // makes path (root ... x) red (preffered); x becomes root of its aux splay
    int expose(int x) {
        int last = 0;
        for (int y = x; y != 0; y = t[y].p) {
            splay(y);
            t[y].ch[1] = last;
            if (last) t[last].p = y;
            pull(y);
            last = y;
        }
        splay(x);
        return last;
    }

    int access(int x) {
        return expose(x);
    }

    void evert(int x) {
        expose(x);
        apply_rev(x);
    }

    void makeroot(int x) {
        evert(x);
    }

    int findroot(int x) {
        expose(x);
        while (t[x].ch[0]) {
            push(x);
            x = t[x].ch[0];
        }
        splay(x);
        return x;
    }

    bool connected(int u, int v) {
        if (u == v) return true;
        return findroot(u) == findroot(v);
    }

    // after split(u, v), the whole path u-v is exactly the aux splay rooted at v
    bool split(int u, int v) {
        if (!connected(u, v)) return false;
        evert(u);
        expose(v);
        return true;
    }

    bool link(int u, int v) {
        evert(u);
        if (findroot(v) == u) return false; // alrady in one tree
        t[u].p = v;
        return true;
    }

    bool cut(int u, int v) {
        evert(u);
        expose(v);

        // if edge (u, v) exists directly, then u is left child of v
        // and u has no right child
        if (t[v].ch[0] != u || t[u].ch[1] != 0) return false;

        t[v].ch[0] = 0;
        t[u].p = 0;
        pull(v);
        return true;
    }

    // ===== values / queries =====

    ll get_value(int x) {
        expose(x);
        return t[x].val;
    }

    void set_value(int x, ll val) {
        expose(x);
        t[x].val = val;
        pull(x);
    }

    void update_value(int x, ll delta) {
        expose(x);
        t[x].val += delta;
        pull(x);
    }

    bool path_add(int u, int v, ll delta) {
        if (!split(u, v)) return false;
        apply_add(v, delta);
        return true;
    }

    ll path_sum(int u, int v) {
        bool ok = split(u, v);
        assert(ok);
        return t[v].sum;
    }

    // LCA for rooted interpretation of current represented tree
    // works when u, v are connected
    int lca(int u, int v) {
        if (!connected(u, v)) return 0;
        expose(u);
        return expose(v);
    }

};
