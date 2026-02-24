#include <bits/stdc++.h>

using namespace std;

// standard binary search. returns the element if found, otherwise INT_MIN
int my_binary_search(vector<int>& arr, int target) {
    int n = (int)arr.size();
    sort(arr.begin(), arr.end());

    int l = -1, r = n;
    while(l <= r) {
        int m = l + (r-l)/2;
        if(arr[m] < target) {
            l = m + 1;
        } else if(arr[m] > target) {
            r = m - 1;
        } else return arr[m];
    }

    return INT_MIN;
}

// return a[i] >= target
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

// return a[i] <= target
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
    return arr[l];
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
