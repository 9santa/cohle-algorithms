#include <bits/stdc++.h>

using namespace std;

/** Fenwick tree for prefix maximum queries with monotone point updates. Space: O(n). */
class FenwickTree {
private:
    vector<int> tree;

    /** Returns the least significant set bit of i. */
    int LSB(int i) {
        return i & -i;
    }

public:
    /** Creates an empty tree with the given number of elements. */
    FenwickTree(int size) {
        tree.resize(size+1, 0);
    }

    /** Builds the tree from a 0-indexed array. Time: O(n log n). */
    FenwickTree(const vector<int>& nums) {
        int n = (int)nums.size();
        tree.resize(n+1, 0);

        for(int i = 0; i < n; i++) {
            update(i, nums[i]);
        }
    }

    /** Updates nums[index] to max(nums[index], value). Time: O(log n). */
    void update(int index, int value) {
        index++;
        while(index <= (int)tree.size()) {
            tree[index] = max(tree[index], value);
            index += LSB(index);
        }
    }

    /** Returns the maximum value over [0, index]. Time: O(log n). */
    int max_query(int index) {
        index++;
        int sum = 0;
        while(index > 0) {
            sum = max(sum, tree[index]);
            index -= LSB(index);
        }
        return sum;
    }

    // Range maximum queries are not supported by this Fenwick variant.
    // int range_sum_query(int l, int r) {
    //     return sum_query(r) - sum_query(l-1);
    // }
};
