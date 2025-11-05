#include <algorithm>
#include <bits/stdc++.h>
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
