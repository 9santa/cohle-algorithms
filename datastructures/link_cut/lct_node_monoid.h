
// Node abstraction from LCT
// Node will work with templated monoid
template<class Monoid>
struct LCT_Node_Monoid {
    using np = LCT_Node_Monoid*;
    using X = typename Monoid::value_type;

    np l = nullptr, r = nullptr, p = nullptr;
    int idx = -1;
    int siz = 1;            // size of aux splay subtree
    bool rev = false;

    X vx = Monoid::id();    // value of this vertex
    X x = Monoid::id();     // aggregate in inorder: left -> self -> right
    X rx = Monoid::id();    // aggregate in reversed inorder: right -> self -> left

    LCT_Node_Monoid() = default;

    // update node from left and right children
    void update() {
        siz = 1;
        x = vx, rx = vx;
        if (l) {
            siz += l->siz;
            x = Monoid::op(l->x, x);
            rx = Monoid::op(rx, l->rx);
        }
        if (r) {
            siz += r->siz;
            x = Monoid::op(x, r->x);
            rx = Monoid::op(r->rx, rx);
        }
    }

    void reverse() {
        rev ^= 1;
        swap(l, r);
        swap(x, rx);
    }

    void push() {
        if (!rev) return;
        if (l) l->reverse();
        if (r) r->reverse();
        rev = false;
    }

    void set(const X& v) { vx = v; }

    void add_light(np) {}
    void erase_light(np) {}
    void change_light(np a, np b) {
        if (a) erase_light(a);
        if (b) add_light(b);
    }
};
