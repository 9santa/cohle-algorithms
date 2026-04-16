#pragma once
#include "core.h"

namespace monotonic_stack {

// For each element, find the first greater element to its right.
template<typename T>
vector<T> next_greater_array(const vector<T>& a) {
    int n = (int)a.size();
    vector<T> nge(n, -1);
    stack<T> stk;

    for (int i = 0; i < n; i++) {
        while (!stk.empty() && a[stk.top()] < a[i]) {
            nge[stk.top()] = i; // for elements instead of indices: ans[idx] = a[i]
            stk.pop();
        }
        stk.push(i);
    }
    return nge;
}

// For each element, find the first smaller element to its right.
template<typename T>
vector<T> next_smaller_array(const vector<T>& a) {
    int n = (int)a.size();
    vector<T> nse(n, -1);
    stack<T> stk;

    for (int i = 0; i < n; i++) {
        while (!stk.empty() && a[stk.top()] > a[i]) {
            nse[stk.top()] = i;
            stk.pop();
        }
        stk.push(i);
    }
    return nse;
}

// For each element, find the first smaller element to its left.
template<typename T>
vector<T> prev_smaller_array(const vector<T>& a) {
    int n = (int)a.size();
    vector<T> pse(n, -1);
    stack<T> stk;

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

// For each element, find the first greater element to its left.
template<typename T>
vector<T> prev_greater_array(const vector<T>& a) {
    int n = (int)a.size();
    vector<T> pge(n, -1);
    stack<T> stk;

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
