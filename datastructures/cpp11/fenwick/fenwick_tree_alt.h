#include <bits/stdc++.h>

using namespace std;

/** Zero-indexed Fenwick tree for point assignments and range sum queries. Space: O(n). */
class FenwickTree {
private:
    vector<int> tree;
    vector<int> nums;

public:
    // FenwickTree(int size) : tree(size, 0) {}

    /** Builds the tree from a 0-indexed array. Time: O(n log n). */
    FenwickTree(const vector<int>& arr) {
        nums = arr;
        tree.assign(arr.size(), 0);

        for(int i = 0; i < (int)arr.size(); i++) {
            update(i, arr[i]);
        }
    }

    /** Adds delta to nums[index]. Time: O(log n). */
    void add(int index, int delta) {
        int j = index;
        while(j < (int)tree.size()) {
            tree[j] += delta;
            j = j | (j + 1);
        }
    }

    /** Sets nums[index] to value. Time: O(log n). */
    void update(int index, int value) {
        int diff = value - nums[index];
        nums[index] = value;
        add(index, diff);
    }

    /** Returns the prefix sum over [0, index]. Time: O(log n). */
    int sum_query(int index) {
        int j = index;
        int sum = 0;
        while(j >= 0) {
            sum += tree[j];
            j = (j & (j+1)) - 1;
        }
        return sum;
    }

    /** Returns the range sum over [l, r]. Time: O(log n). */
    int range_sum_query(int l, int r) {
        if(l == 0) return sum_query(r);
        return sum_query(r) - sum_query(l-1);
    }
};
