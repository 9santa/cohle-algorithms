#include <algorithm>
#include <bits/stdc++.h>
#include <climits>
#include <vector>

using namespace std;

// O(n^2) DP algorithm
int longestIncreasingSubsequence(vector<int>& nums) {
    if(nums.empty()) return 0;

    int n = static_cast<int>(nums.size());
    vector<int> dp(n, 1);    // dp[i] = length of LIS ending at index i

    for(int i = 1; i < n; i++) {
        for(int j = 0; j < i; j++) {
            if(nums[i] > nums[j]) {
                dp[i] = max(dp[i], dp[j] + 1);
            }
        }
    }

    return *max_element(dp.begin(), dp.end());
}

// O(n log n) Binary Search algorithm
int longestIncreasingSubsequence1(vector<int>& nums) {
    vector<int> tail;

    for(int num : nums) {
        // find the first element in tail that is >= num
        auto it = lower_bound(tail.begin(), tail.end(), num);

        if(it == tail.end()) {
            // num is larger than all elements, extend the sequence
            tail.push_back(num);
        } else {
            // replace the first element >= num
            *it = num;
        }
    }

    return tail.size();
}

// More intuitive O(n log n) Binary Search algorithm
int lengthOfLIS(vector<int>& nums) {
    int n = (int)nums.size();

    vector<int> seq;
    seq.push_back(nums[0]);
    for(int i = 1; i < n; i++) {
        if(nums[i] > seq.back()) seq.push_back(nums[i]);
        else {
            int idx = lower_bound(seq.begin(), seq.end(), nums[i]) - seq.begin();
            seq[idx] = nums[i];
        }
    }

    return (int)seq.size();
}

/**
  * @brief Finds one of the Longest Increasing Subsequences (LIS) in O(n log n).
  * The function calculates dp[i] (length of LIS ending at nums[i])
  * in O(n log n) using 'tails' array and binary search.
  * It then reconstructs the sequence in O(n) time by iterating backwards
  * from the last element of the LIS found.
  *
  * @param nums The input array.
  * @return vector<int> The reconstrucred LIS.
  */
vector<int> LIS(vector<int>& nums) {
    int n = (int)nums.size();
    vector<int> dp(n, 1);
    vector<int> tails(n, INT_MAX);
    tails[0] = INT_MIN;

    for (int i = 0; i < n; i++) {
        int num = nums[i];
        auto it = lower_bound(tails.begin(), tails.end(), num);

        if (it != tails.end()) {
            *it = num;
            dp[i] = it - tails.begin();
        }
    }

    vector<int> seq;
    int dp_max = max_element(dp.begin(), dp.end()) - dp.begin();
    seq.push_back(nums[dp_max]);
    for (int i = dp_max - 1; i >= 0; i--) {
         if (nums[i] < seq.back() && dp[i] == dp[dp_max] - 1) {
            seq.push_back(nums[i]);
            dp_max = i;
         }
    }

    reverse(seq.begin(), seq.end());
    return seq;
}

signed main(void) {

    vector<int> nums = {17, 3, 9, 4, 5, 8};
    vector<int> lis = LIS(nums);

    for (auto& el : lis) cout << el << " "; // 3 4 5 8

    return 0;
}
