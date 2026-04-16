#pragma once
#include "core.h"

/* A monotonic queue is usually a deque that stays sorted.
Useful for sliding window min/max.
 - for sliding window max, keep a decreasing deque
 - front always has the maximum
 - remove smaller elements from the back
 - remove out-of-window elements from the fron
*/

namespace monotonic_queue {

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
