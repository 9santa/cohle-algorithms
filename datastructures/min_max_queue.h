#pragma once
#include "core.h"

/* A min queue / max queue is a queue that can return the current minimum
or maximum in O(1) while supporting push/pop in amortized O(1) */

/* Max queue: keep a deque in decreasing order
- front = current maximum
- when pushing 'x', remove all smaller elements from the back
- when popping from the main queue, also pop from deque front if it matches */
template<typename T>
struct MaxQueue {
    queue<T> q;
    deque<T> dq; // decreasing

    void push(T x) {
        q.push(x);
        while (!dq.empty() && dq.back() < x) dq.pop_back();
        dq.push_back(x);
    }

    void pop() {
        if (q.empty()) return;
        if (q.front() == dq.front()) dq.pop_front();
        q.pop();
    }

    T getMax() const {
        return dq.front();
    }
};

template<typename T>
struct MinQueue {
    queue<T> q;
    deque<T> dq;

    void push(T x) {
        q.push(x);
        while (!dq.empty() && dq.back() > x) dq.pop_back();
        dq.push_back(x);
    }

    void pop() {
        if (q.empty()) return;
        if (q.front() == dq.front()) dq.pop_front();
        q.pop();
    }

    T getMin() const {
        return dq.front();
    }
};

