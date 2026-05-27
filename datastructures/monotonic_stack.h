#pragma once
#include "core.h"

namespace monotonic_stack {

/** Returns index of the first greater element to the right, or n. Time: O(n). */
template<typename T>
vector<int> next_greater_index(const vector<T>& a) {
    int n = (int)a.size();
    vector<int> nge(n, n);
    stack<int> stk;

    for (int i = 0; i < n; i++) {
        while (!stk.empty() && a[stk.top()] < a[i]) {
            nge[stk.top()] = i; // for elements instead of indices: ans[idx] = a[i]
            stk.pop();
        }
        stk.push(i);
    }
    return nge;
}

/** Returns index of the first smaller element to the right, or n. Time: O(n). */
template<typename T>
vector<int> next_smaller_array(const vector<T>& a) {
    int n = (int)a.size();
    vector<int> nse(n, n);
    stack<int> stk;

    for (int i = 0; i < n; i++) {
        while (!stk.empty() && a[stk.top()] > a[i]) {
            nse[stk.top()] = i;
            stk.pop();
        }
        stk.push(i);
    }
    return nse;
}

/** Returns index of the first smaller element to the left, or -1. Time: O(n). */
template<typename T>
vector<int> prev_smaller_array(const vector<T>& a) {
    int n = (int)a.size();
    vector<int> pse(n, -1);
    stack<int> stk;

    for (int i = 0; i < n; i++) {
        while (!stk.empty() && a[stk.top()] >= a[i]) {
            stk.pop();
        }
        if (!stk.empty()) pse[i] = stk.top();
        else pse[i] = -1;
        stk.push(i);
    }
    return pse;
}

/** Returns index of the first greater element to the left, or -1. Time: O(n). */
template<typename T>
vector<int> prev_greater_array(const vector<T>& a) {
    int n = (int)a.size();
    vector<int> pge(n, -1);
    stack<int> stk;

    for (int i = 0; i < n; i++) {
        while (!stk.empty() && a[stk.top()] <= a[i]) {
            stk.pop();
        }
        if (!stk.empty()) pge[i] = stk.top();
        else pge[i] = -1;
        stk.push(i);
    }
    return pge;
}

} // namespace monotonic_stack
