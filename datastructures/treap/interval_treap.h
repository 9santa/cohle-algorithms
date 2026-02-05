#include "../../header.h"
#include "../node_pool.h"
#include <bit>

// Interval tree with treap balancing
// BST key: L (segment start)
// Each node stores interval [L, R) and augmented mxR = max R in subtree
// Ops Time complexity: O(log n)
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

    void reset() { root = nullptr; pool.reset_keep_memory(); }

    // Insert assumes unique L
    void insert(Coord L, Coord R) {
        assert(!comp(R, L) && "require L <= R");
        root = ins(root, pool.create(L, R, splitmix64()));
    }

    bool erase_by_L(const Coord& L) {
        bool erased = false;
        root = era(root, L, erased);
        return erased;
    }

    // Returns pointer to an interval containing x, or nullptr
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

    // Check if tree contains coordinate
    bool contains_point(const Coord& x) const { return find_containing(x) != nullptr; }

    // Any overlap with query [L, R): overlap if node.L < R && L < node.R
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
        return any_overlap_rec(t->R, L, R);
    }
};
