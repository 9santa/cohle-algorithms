#include "../header.h"

// Cyclic replacements approach in-place. O(1) space
void rotate1(vector<int>& nums, int k) {
    int n = static_cast<int>(nums.size());

    if(k > n) {
        k = k % n;
    }
    if(k == 0) return;

    int count = 0;
    for(int start = 0; count < n; start++) {
        int current = start;
        int prev = nums[start];

        do {
            int next = (current + k) % n;
            int temp = nums[next];
            nums[next] = prev;
            prev = temp;
            current = next;
            count++;
        } while (start != current);
    }
}

// Manual approach with O(n) space
void rotate2(vector<int>& nums, int k) {
    int n = static_cast<int>(nums.size());
    vector<int> rotated = nums;

    if(k > n) {
        k = k % n;
    }

    for(int i = 0; i < n; i++) {
        rotated[i] = nums[(i - k + n) % n];
    }

    nums = rotated;
}


// pseudocode of how std::rotate might be implemented
// could be triple reverse method (GCC, Clang)
template<class ForwardIt>
ForwardIt rotate(ForwardIt first, ForwardIt middle, ForwardIt last) {
    if (first == middle) return last;
    if (middle == last) return first;

    // Reverse the entire range
    std::reverse(first, last);

    // Find the new middle point after reversal
    auto new_middle = first;
    std::advance(new_middle, std::distance(middle, last));

    // Reverse the two segments
    std::reverse(first, new_middle);
    std::reverse(new_middle, last);

    return new_middle;
}


