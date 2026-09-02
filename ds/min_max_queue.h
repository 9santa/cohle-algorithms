#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Queue supporting amortized O(1) push/pop and O(1) maximum queries. Space: O(n). */
template<typename T>
struct MaxQueue {
    queue<T> q;
    deque<T> dq; // decreasing

    /** Pushes x to the back. Amortized time: O(1). */
    void push(T x) {
        q.push(x);
        while (!dq.empty() && dq.back() < x) dq.pop_back();
        dq.push_back(x);
    }

    /** Removes the front element if present. Amortized time: O(1). */
    void pop() {
        if (q.empty()) return;
        if (q.front() == dq.front()) dq.pop_front();
        q.pop();
    }

    /** Returns the current maximum. Time: O(1). */
    T getMax() const {
        return dq.front();
    }
};

/** Queue supporting amortized O(1) push/pop and O(1) minimum queries. Space: O(n). */
template<typename T>
struct MinQueue {
    queue<T> q;
    deque<T> dq;

    /** Pushes x to the back. Amortized time: O(1). */
    void push(T x) {
        q.push(x);
        while (!dq.empty() && dq.back() > x) dq.pop_back();
        dq.push_back(x);
    }

    /** Removes the front element if present. Amortized time: O(1). */
    void pop() {
        if (q.empty()) return;
        if (q.front() == dq.front()) dq.pop_front();
        q.pop();
    }

    /** Returns the current minimum. Time: O(1). */
    T getMin() const {
        return dq.front();
    }
};
