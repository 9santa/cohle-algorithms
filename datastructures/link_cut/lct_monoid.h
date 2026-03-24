#pragma once
#include "../../header.h"

template<class Node>
struct LCT_Monoid {
    using np = Node*;
    using X = typename Node::X;
    using VX = typename Node::VX;

    int n;
    vector<Node> t;

    LCT_Monoid(int n_ = 0) { init(n_); }

    void init(int n_) {
        n = n_;
        t.reserve(n);
        F0R(i, n) t[i] = Node(i);
    }

    np operator[](int v) { return &t[v]; }

    bool is_root(np x) {
        return !x->p || (x->p->l != x && x->p->r != x);
    }

    void pull(np x) {
        if (x) x->update();
    }

    void push(np x) {
        if (x) x->push();
    }

    void push_to(np x) {
        vector<np> stk;
        for (np y = x; ; y = y->p) {
            stk.push_back(y);
            if (is_root(y)) break;
        }
        for (int i = sz(stk)-1; i >= 0; i--) push(stk[i]);
    }

    void rotate(np x) {
        np p = x->p;
        np g = p->p;
        bool dirx = (p->r == x); // is x right child of p?
        np b = dirx ? x->l : x->r;

        if (is_root(p)) {
            if (g->l == p) g->l = x;
            else if (g->r == p) g->r = x;
        }
        x->p = g;

        if (dirx) {
            x->l = p;
            p->p = x;
            p->r = b;
            if (b) b->p = p;
        } else {
            x->r = p;
            p->p = x;
            p->l = b;
            if (b) b->p = p;
        }

        pull(p);
        pull(x);
    }

    void splay(np x) {
        push_to(x);
        while (!is_root(x)) {
            np p = x->p;
            np g = p->p;
            if (!is_root(p)) {
                bool zigzig = ((p->l == x) == (g->l == p));
                if (zigzig) rotate(p);
                else rotate(x);
            }
            rotate(x);
        }
    }

    // makes the path (represented tree root ... x) preffered
    // returns last accessed node (useful for lca)
    np expose(np x) {
        np last = nullptr;
        for (np y = x; y; y = y->p) {
            splay(y);

            np old_r = y->r;

            // old preffered child becomes virtual
            if (old_r) y->add_light(old_r);

            // new preffered child stops being virtual
            if (last) y->erase_light(last);

            y->r = last;
            if (last) last->p = y;
            // old_r stays with parent y as a virtual child
            // its p should remain y

            pull(y);
            last = y;
        }
        splay(x);
        return last;
    }

    np access(np x) { return expose(x); }

    void evert(np x) {
        expose(x);
        x->reverse();
    }

    void makeroot(np x) {
        evert(x);
    }

    np findroot(np x) {
        expose(x);
        while (x->l) {
            x->push();
            x = x->l;
        }
        splay(x);
        return x;
    }

    bool connected(np a, np b) {
        if (a == b) return true;
        return findroot(a) == findroot(b);
    }

    // after split(a, b), the whole path a->b is exactly the aux tree rooted at b
    bool split(np a, np b) {
        if (!connected(a, b)) return false;
        makeroot(a);
        expose(b);
        return true;
    }

    bool link(np a, np b) {
        makeroot(a);
        if (findroot(b) == a) return false; // already connected

        // a becomes a virtual child of b
        expose(b);
        a->p = b;
        b->add_light(a);
        pull(b);
        return true;
    }

    bool cut(np a, np b) {
        makeroot(a);
        expose(b);

        // if edge a-b exists directly, then a is left child of b
        // and a has no right child
        if (b->l != a || a->r) return false;

        b->l = nullptr;
        a->p = nullptr;
        pull(b);
        return true;
    }

    np lca(np a, np b) {
        if (!connected(a, b)) return nullptr;
        expose(a);
        return expose(b);
    }

    void set(np x, const VX& v) {
        expose(x);
        x->set(v);
        pull(x);
    }

    VX get_vertex(np x) {
        expose(x);
        return x->vx;
    }

    // aggregate on path a ... b
    X prod_path(np a, np b) {
        split(a, b);
        return b->x;
    }

    // aggregate on path b .. a
    X prod_path_rev(np a, np b) {
        split(a, b);
        return b->rx;
    }
};
