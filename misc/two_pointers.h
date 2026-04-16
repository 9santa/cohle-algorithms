#pragma once
#include "../header.h"

/* 1. Opposite-direction pointers.
Used on sorted arrays, palindrome checks, pair sum, etc. */
void opposite_direction_pointers(const vector<int>& a, int target) {
    int l = 0, r = (int)a.size() - 1;
    while (l < r) {
        int sum = a[l] + a[r];
        if (sum == target) {
            cout << l << " " << r << "\n";
            break;
        } else if (sum < target) l++;
        else r--;
    }
}

/* 2. Same-direction sliding window.
Used for subarrays. Typically:
 - longest/shortest valid subarray
 - count subarrays
 - sum/freqeuncy constrains */
void same_direction(const vector<int>& a) {
    int n = (int)a.size();
    int l = 0;
    for (int r = 0; r < n; r++) {
        // include a[r]
        while (/* window is valid*/ 1) {
            // remove a[l]
            l++;
        }

        // now window [l..r] is valid
    }
}

/* 3. Longest subarray with sum <= or >= k.
Works the array is non-negative */
void longest_subbaray_with_sum(const vector<int>& a, int k) {
    int l = 0;
    long long sum = 0;
    int best = 0;

    for (int r = 0; r < (int)a.size(); r++) {
        sum += a[r];
        while (sum > k) {
            sum -= a[l];
            l++;
        }
        best = max(best, r - l + 1);
    }
    cout << best << "\n";
}

/* 4. Count subarrays with at most k distinct values. */
long long at_most_k_distinct(const vector<int>& a, int k) {
    unordered_map<int, int> freq;
    int l = 0;
    long long ans = 0;

    for (int r = 0; r < (int)a.size(); r++) {
        freq[a[r]]++;

        while ((int)freq.size() > k) {
            freq[a[l]]--;
            if (freq[a[l]] == 0) freq.erase(a[l]);
            l++;
        }
        // all subarrays ending at 'r' with start from 'l' to 'r' are valid
        ans += (r - l + 1);
    }
    return ans;
}

/* 5. Longest substring / subarray with all unique elements */
int longest_substring_all_unique(const string& s) {
    vector<int> freq(256, 0);
    int l = 0, best = 0;

    for (int r = 0; r < (int)s.size(); r++) {
        freq[s[r]]++;

        while (freq[s[r]] > 1) {
            freq[s[l]]--;
            l++;
        }

        best = max(best, r - l + 1);
    }
    return best;
}

/* 6. Merge-like two pointers on two arrays. */

/* 7. Deduplicate sorted array. */
