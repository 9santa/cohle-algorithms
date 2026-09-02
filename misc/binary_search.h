#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Sorts arr and returns target if found, otherwise INT_MIN. Time: O(n log n). */
int my_binary_search(vector<int>& arr, int target) {
    int n = (int)arr.size();
    sort(arr.begin(), arr.end());

    int l = -1, r = n;
    while(l+1 < r) {
        int m = l + (r-l)/2;
        if(arr[m] < target) {
            l = m;
        } else if(arr[m] > target) {
            r = m;
        } else return arr[m];
    }

    return INT_MIN;
}

/** Sorts arr and returns the first value >= target. Time: O(n log n). */
int my_lower_bound(vector<int>& arr, int target) {
    int n = (int)arr.size();
    sort(arr.begin(), arr.end());

    int l = -1, r = n;
    while (l+1 < r) {
        int m = l + (r-l)/2;
        if(arr[m] >= target) {
            r = m;
        } else {
            l = m;
        }
    }
    return arr[r];
}

/** Sorts arr and returns the last value > target. Time: O(n log n). */
int my_upper_bound(vector<int>& arr, int target) {
    int n = (int)arr.size();
    sort(arr.begin(), arr.end());

    int l = -1, r = n;
    while (l+1 < r) {
        int m = l + (r-l)/2;
        if(arr[m] > target) {
            r = m;
        } else {
            l = m;
        }
    }
    return arr[r];
}

signed main(void)
{
    clock_t start = clock();
    ios::sync_with_stdio(0);
    cin.tie(0);

    vector<int> v = {213, 1, 435, 12, 234, 4, 5, 6, 1, 94, 85};
    cout << my_lower_bound(v, 95) << "\n";
    cout << my_binary_search(v, 94) << "\n";
    cout << my_upper_bound(v, 95) << "\n";


    return 0;
}
