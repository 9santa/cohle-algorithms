#include <bits/stdc++.h>
#include <vector>

using namespace std;

int longestStrictlyIncreasingSubarray(vector<int>& nums) {
    if(nums.empty()) return 0;

    int max_len = 1, cur_len = 1;
    for(int i = 1; i < (int)nums.size(); i++) {
        if(nums[i] > nums[i-1]) {
            cur_len++;
        } else {
            max_len = max(max_len, cur_len);
            cur_len = 1;
        }
    }

    max_len = max(max_len, cur_len);
    return max_len;
}

int longestNonDecreasingSubarray(vector<int>& nums) {
    if(nums.empty()) return 0;

    int max_len = 1, cur_len = 1;
    for(int i = 1; i < (int)nums.size(); i++) {
        if(nums[i] >= nums[i-1]) {
            cur_len++;
        } else {
            max_len = max(max_len, cur_len);
            cur_len = 1;
        }
    }

    max_len = max(max_len, cur_len);
    return max_len;
}
