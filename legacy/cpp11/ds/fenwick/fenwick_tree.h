#include <bits/stdc++.h>

using namespace std;

/** Fenwick tree for point additions and prefix/range sum queries. Space: O(n). */
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

    /** Builds the tree from a 1-indexed array. Time: O(n). */
    void init(int n, const vector<int>& nums) {
        for (int i = 1; i <= n; i++) {
            tree[i] += nums[i];
            if (i + LSB(i) <= n) {
                tree[i + LSB(i)] += tree[i];
            }
        }
    }

    /** Adds value to nums[index]. Time: O(log n). */
    void update(int index, int value) {
        index++;
        while(index < (int)tree.size()) {
            tree[index] += value;
            index += LSB(index);
        }
    }

    /** Returns the prefix sum over [0, index]. Time: O(log n). */
    int sum_query(int index) {
        index++;
        int sum = 0;
        while(index > 0) {
            sum += tree[index];
            index -= LSB(index);
        }
        return sum;
    }

    /** Returns the range sum over [l, r]. Time: O(log n). */
    int range_sum_query(int l, int r) {
        return sum_query(r) - sum_query(l-1);
    }
};
