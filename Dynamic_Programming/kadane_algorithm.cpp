#include <bits/stdc++.h>

using namespace std;

// Basic Kadane's algorithm for Maximum Subarray Sum
// Time: O(n), Space: O(1)
int maxSubArraySum(vector<int>& nums) {
    int max_so_far = nums[0];
    int max_ending_here = nums[0];

    for(int i = 1; i < (int)nums.size(); i++) {
        max_ending_here = max(nums[i], max_ending_here + nums[i]);
        max_so_far = max(max_so_far, max_ending_here);
    }

    return max_so_far;
}


// With indices, return the actual subarray
vector<int> maxSubArrayWithIndices(vector<int>& nums) {
    int max_so_far = nums[0];
    int max_ending_here = nums[0];
    int start = 0, end = 0;
    int temp_start = 0;

    for(int i = 1; i < (int)nums.size(); i++) {
        if(nums[i] > max_ending_here + nums[i]) {
            max_ending_here = nums[i];
            temp_start = i;
        } else {
            max_ending_here += nums[i];
        }

        if(max_ending_here > max_so_far) {
            max_so_far = max_ending_here;
            start = temp_start;
            end = i;
        }
    }

    return vector<int>(nums.begin() + start, nums.begin() + end + 1);
}

// Maximum/Minimum Product Subarray (Kadane's variant)
pair<int, int> max_min_product(vector<int>& nums) {
    int max_so_far = nums[0];
    int min_so_far = nums[0];
    pair<int, int> result = {nums[0], nums[0]};
    
    for(int i = 1; i < (int)nums.size(); i++) {
        int temp_max = max({nums[i], max_so_far * nums[i], min_so_far * nums[i]});
        min_so_far = min({nums[i], max_so_far * nums[i], min_so_far * nums[i]});
        max_so_far = temp_max;

        result.first = max(result.first, max_so_far);
        result.second = min(result.second, min_so_far);
    }

    return result;
}



// test example
int main(void) {

    vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    
    vector<int> ans = maxSubArrayWithIndices(nums);

    for(auto el : ans) cout << el << " ";


    return 0;
}
