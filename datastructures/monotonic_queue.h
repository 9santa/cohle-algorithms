#pragma once
#include "core.h"

namespace monotonic_queue {

/** Returns maximum of every window of length k. Time: O(n). Space: O(k). */
template<typename T>
vector<T> sliding_window_max(const vector<T>& a, int k) {
    int n = (int)a.size();
    vector<T> ans;
    ans.reserve(n-k+1);
    deque<int> dq;

    for (int i = 0; i < n; i++) {
        while (!dq.empty() && a[dq.back()] <= a[i]) {
            dq.pop_back();
        }
        dq.push_back(i);
        if (dq.front() <= i - k) dq.pop_front();
        if (i >= k - 1) ans.push_back(a[dq.front()]);
    }
    return ans;
}

/** Returns minimum of every window of length k. Time: O(n). Space: O(k). */
template<typename T>
vector<T> sliding_window_min(const vector<T>& a, int k) {
    int n = (int)a.size();
    vector<T> ans;
    ans.reserve(n-k+1);
    deque<int> dq;

    for (int i = 0; i < n; i++) {
        while (!dq.empty() && a[dq.back()] >= a[i]) {
            dq.pop_back();
        }
        dq.push_back(i);
        if (dq.front() <= i - k) dq.pop_front();
        if (i >= k - 1) ans.push_back(a[dq.front()]);
    }
    return ans;
}

} // namespace monotonic_queue
