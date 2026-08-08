#include "core.h"


struct LiChaoTree {
    static constexpr ll INF = (1LL << 62);

    struct Line {
        ll k, b; // y = k*x + b

        Line (ll k = 0, ll b = INF) : k(k), b(b) {}

        i128 eval(ll x) const {
            return (i128)k * x + b;
        }
    };

    struct Node {
        Line line;
        Node* left;
        Node* right;

        explicit Node(Line line_) : line(line_), left(nullptr), right(nullptr) {}
    };

    Node* root = nullptr;
    ll XL, XR;

    LiChaoTree(ll XL, ll XR) : XL(XL), XR(XR) {}

    void add_line(Line nw) { add_line(root, XL, XR, nw); }

    void add_line(ll k, ll b) { add_line(Line(k, b)); }

    i128 query128(ll x) const {
        return query(root, XL, XL, x);
    }

    ll query(ll x) const {
        i128 ans = query128(x);

        if (ans >= (i128)INF) return INF;

        return (ll)ans;
    }

private:
    void add_line(Node*& v, ll l, ll r, Line nw) {
        if (!v) {
            v = new Node(nw);
            return;
        }

        ll mid = l + (r - l) / 2;

        // Compare the new line with the line currently stored at this node
        bool better_left = nw.eval(l) < v->line.eval(l);
        bool better_mid = nw.eval(mid) < v->line.eval(mid);

        /*
           The line which is better at mid is stored at this node.
           If nw is better at mid:
            swap(nw, node->line)

           Now "nw" is the worse line at mid.
           It can still become better on exactly one side.
        */
        if (better_mid) {
            swap(nw, v->line);
        }

        if (l == r) return;

        /*
           If comparison at l and mid was different,
           the two lines cross somewhere on the left side,
           otherwise on the right side.
        */
        if (better_left != better_mid)
            add_line(v->left, l, mid, nw);
        else
            add_line(v->right, mid+1, r, nw);
    }

    i128 query(Node* v, ll l, ll r, ll x) const {
        if (!v) return (i128)INF;

        i128 ans = v->line.eval(x);

        // Reached a leaf
        if (l == r) return ans;

        ll mid = l + (r - l) / 2;

        if (x <= mid)
            ans = min(ans, query(v->left, l, mid, x));
        else
            ans = min(ans, query(v->right, mid+1, r, x));

        return ans;
    }
};
