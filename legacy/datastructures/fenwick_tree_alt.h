#include <bits/stdc++.h>

using namespace std;

// Operations: O(log n)
// Space: O(n)
class FenwickTree {
private:
    vector<int> tree;
    vector<int> nums;

public:
    // FenwickTree(int size) : tree(size, 0) {}

    // Construct Fenwick Tree
    FenwickTree(const vector<int>& arr) {
        nums = arr;
        tree.assign(arr.size(), 0);

        for(int i = 0; i < (int)arr.size(); i++) {
            update(i, arr[i]);
        }
    }

    void add(int index, int delta) {
        int j = index;
        while(j < (int)tree.size()) {
            tree[j] += delta;
            j = j | (j + 1);
        }
    }

    // Update value at index
    void update(int index, int value) {
        int diff = value - nums[index];
        nums[index] = value;
        add(index, diff);
    }
    // Get prefix sum [0, index]
    int sum_query(int index) {
        int j = index;
        int sum = 0;
        while(j >= 0) {
            sum += tree[j];
            j = (j & (j+1)) - 1;
        }
        return sum;
    }

    // Get range sum [l, r] (same as prefix sum formula)
    int range_sum_query(int l, int r) {
        if(l == 0) return sum_query(r);
        return sum_query(r) - sum_query(l-1);
    }
};

