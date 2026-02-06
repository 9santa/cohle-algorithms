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

    // O(n) Fenwick initialization
    void init(int n, const vector<int>& nums) {
        for (int i = 1; i <= n; i++) {
            tree[i] += nums[i];
            if (i + LSB(i) <= n) {
                tree[i + LSB(i)] += tree[i];
            }
        }
    }

    void update(int index, int value) {
        index++;
        while(index < (int)tree.size()) {
            tree[index] += value;
            index += LSB(index);
        }
    }

    // Get prefix sum [0, index]
    int sum_query(int index) {
        index++;
        int sum = 0;
        while(index > 0) {
            sum += tree[index];
            index -= LSB(index);
        }
        return sum;
    }

    // Get range sum [l, r] (same as prefix sum formula)
    int range_sum_query(int l, int r) {
        return sum_query(r) - sum_query(l-1);
    }
};


// testing example
int main(void) {
    vector<int> nums = {1, 3, 5, 7, 9, 11};
    FenwickTree ft(nums);

    cout << "Prefix sum [0, 3]: " << ft.sum_query(3) << "\n";        // 1+3+5+7 = 16
    cout << "Range sum [2, 4]: " << ft.range_sum_query(2, 4) << "\n"; // 5+7+9 = 21

    ft.update(3, 2);  // add 2 to element at index 3 (7+2 = 9)
    cout << "After update, prefix sum [0, 3]: " << ft.sum_query(3) << "\n";  // 1+3+5+9 = 18



    return 0;
}
