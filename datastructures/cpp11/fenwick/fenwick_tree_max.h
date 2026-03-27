#include <bits/stdc++.h>

using namespace std;

// Operations: O(log n)
// Space: O(n)
class FenwickTree {
private:
    vector<int> tree;

    int LSB(int i) {
        return i & -i;
    }

public:
    FenwickTree(int size) {
        tree.resize(size+1, 0);
    }

    // Construct Fenwick Tree
    FenwickTree(const vector<int>& nums) {
        int n = (int)nums.size();
        tree.resize(n+1, 0);

        for(int i = 0; i < n; i++) {
            update(i, nums[i]);
        }
    }

    void update(int index, int value) {
        index++;
        while(index <= (int)tree.size()) {
            tree[index] = max(tree[index], value);
            index += LSB(index);
        }
    }

    // Get prefix sum [0, index]
    int max_query(int index) {
        index++;
        int sum = 0;
        while(index > 0) {
            sum = max(sum, tree[index]);
            index -= LSB(index);
        }
        return sum;
    }

    // Get range sum [l, r] (same as prefix sum formula)
    // int range_sum_query(int l, int r) {
    //     return sum_query(r) - sum_query(l-1);
    // }
};


// testing example
int main(void) {
    vector<int> nums = {1, 3, 5, 7, 9, 11};
    FenwickTree ft(nums);




    return 0;
}
