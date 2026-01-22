#include "../header.h"


// Recursive top-to-bottom
class SegmentTreeRecursive {
private:
    int n;  // Size of the input array
    int size;   // Smallest power of 2 >= n
    vector<long long> tree;   // Segment tree

    // Set value at index i (in the source array)
    void set(int i, long long val, int x, int xl, int xr) {
        if(xl == xr-1) {    // leaf node
            tree[x] = val;
            return;
        }

        int xm = (xl + xr) / 2;
        if(i < xm) {
            set(i, val, 2*x+1, xl, xm);  // process left child subtree
        }
        else {
            set(i, val, 2*x+2, xm, xr);  // process right child subtree
        }

        tree[x] = tree[2*x+1] + tree[2*x+2];    // update parent (root of the subtree)
    }

    // Sum query in range [l, r)
    long long sum_query(int l, int r, int x, int xl, int xr) {
        if(xl >= r || xr <= l) return 0;    // no overlap
        if(xl >= l && xr <= r) return tree[x];  // full overlap
        int xm = (xl + xr) / 2;
        long long suml = sum_query(l, r, 2*x+1, xl, xm);
        long long sumr = sum_query(l, r, 2*x+2, xm, xr);

        return suml + sumr;
    }
public:
    SegmentTreeRecursive(const vector<int>& arr) {
        n = (int)arr.size();
        if(n == 0) {
            size = 1;
            tree.assign(2, 0);
            return;
        }

        size = 1;
        // Дополняем до степени двойки >= n
        while(size < n) size *= 2;
        tree.assign(2*size, 0);

        // Initialize leaves (source array elements)
        for(int i = 0; i < n; i++) {
            tree[size+i-1] = arr[i];
        }

        // Internal nodes (for sum)
        for(int i = size-2; i >= 0; i--) {
            tree[i] = tree[2*i+1] + tree[2*i+2];
        }

        // Printing tree for debug
        for(int i = 0; i < 2*size; i++) {
            cout << tree[i] << " ";
        }
    }

    // Set public wrapper
    void set(int i, long long val) {
        if(i < 0 || i >= n) return; // out of bounds
        set(i, val, 0, 0, size);
    }

    // Sum query public wrapper
    long long sum(int l, int r) {
        if(l < 0 || r > n || l > r) return 0; // correct half-interval check
        return sum_query(l, r, 0, 0, size);
    }

    // Searching for the first element greater than a given 'x' and a given range [l, r]
    int get_first(int v, int tl, int tr, int l, int r, int x) {
        if(tl > r || tr < l) return -1;
        if(tree[v] <= x) return -1;

        if (tl == tr) return tl;

        int tm = tl + (tr-tl)/2;
        int left = get_first(2*v, tl, tm, l, r, x);
        if(left != -1) return left;
        return get_first(2*v+1, tm+1, tr, l ,r, x);
    }
};

// Iterative bottom-to-top
class SegmentTreeIterative {
private:
    int n;
    int size;
    vector<long long> tree;

public:
    SegmentTreeIterative(const vector<int>& arr) {
        n = (int)arr.size();
        size = 1;
        if(n == 0) {
            tree.assign(2, 0);
            return;
        }

        while(size < n) size *= 2;
        tree.assign(2*size, 0);

        for(int i = 0; i < n; i++) {
            tree[size+i] = arr[i];
        }

        for(int i = size-1; i > 0; i--) {
            tree[i] = tree[2*i] + tree[2*i+1];
        }

        // Printing tree for debug
        for(int i = 0; i < 2*size; i++) {
            cout << tree[i] << " ";
        }
    }

    void set(int i, long long val) {
        int x = i + size;
        tree[x] = val;  // update value at the correct index

        // climb up
        x /= 2;
        while(x > 0) {
            tree[x] = tree[2*x] + tree[2*x+1];    // update all of the needed parents
            x /= 2;
        }
    }

    // [l, r)
    long long sum(int l, int r) {
        l = l + size;  // included
        r = r + size;  // not included

        long long ans = 0;
        while(l < r) {
            if(l % 2 == 1) ans += tree[l++];
            if(r % 2 == 1) ans += tree[--r];
            l /= 2;
            r /= 2;
        }
        return ans;
    }
};

// Segment Tree With A Proper Tree Structure
class Node {
public:
    long long val;
    Node* left;
    Node* right;
    int l, r; // segment range [l, r)

    Node(int _l, int _r) : val(0), left(nullptr), right(nullptr), l(_l), r(_r) {}
};

class SegmentTreeProper {
public:
    Node* root;
private:
    Node* build(const vector<int>& arr, int l, int r) {
        Node* node = new Node(l, r);
        if(r - l == 1) {
            if(l < (int)arr.size()) node->val = arr[l];
            return node;
        }
        int m = l + (r-l)/2;
        node->left = build(arr, l, m);
        node->right = build(arr, m, r);
        node->val = node->left->val + node->right->val;
        return node;
    }

    void set(Node* node, int i, long long val) {
        if(node->r - node->l == 1) {
            node->val = val;
            return;
        }
        int m = node->l + (node->r - node->l)/2;
        if(i < m) set(node->left, i, val);
        else set(node->right, i, val);
        node->val = node->left->val + node->right->val;
    }

    // [ql, qr)
    long long sum(Node* node, int xl, int xr) {
        if(!node || xl >= node->r || xr <= node->l) return 0;
        if(xl <= node->l && xr >= node->r) return node->val;
        return sum(node->left, xl, xr) + sum(node->right, xl, xr);
    }
public:
    SegmentTreeProper(const vector<int>& arr) {
        root = build(arr, 0, arr.size());
    }

    static void convert_to_array(Node* node, vector<long long>& tree, int i) {
        if(!node) return;
        if(i >= (int)tree.size()) tree.resize(i+1, 0);

        tree[i] = node->val;
        convert_to_array(node->left, tree, 2*i+1);
        convert_to_array(node->right, tree, 2*i+2);
    }


    void set(int i, long long val) {
        set(root, i, val);
    }

    long long sum(int l, int r) {
        return sum(root, l, r);
    }
};



int main(void)
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    vector<int> arr = {5, 2, 4, 1, 2, 8, 3, 1};
    SegmentTreeRecursive seg1(arr);
    cout << "\nAbove is the segment tree\n";

    SegmentTreeIterative seg2(arr);
    cout << "\nAbove is the segment tree\n";

    SegmentTreeProper seg3(arr);
    vector<long long> tree;
    SegmentTreeProper::convert_to_array(seg3.root, tree, 0);
    for(int i = 0; i < (int)tree.size(); i++) {
        cout << tree[i] << " ";
    }
    cout << "\nAbove is the segment tree\n";

    return 0;
}
