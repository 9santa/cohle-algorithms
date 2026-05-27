#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Fenwick tree for point additions and prefix/range sum queries. Space: O(n). */
template<typename T>
struct Fenwick {
    vector<T> fw;
    int n;

    /** Returns the least significant set bit of i. */
    int lsb(int i) const { return i & -i; }

    /** Builds the tree from a 0-indexed array. Time: O(n). */
    void build(const vector<T>& a) {
        n = (int)a.size() + 1; // internal 1-indexed
        fw.assign(n, 0);
        for (int i = 1; i < n; i++) {
            fw[i] += a[i-1];
            if (i + lsb(i) < n) {
                fw[i + lsb(i)] += fw[i];
            }
        }
    }

    /** Adds val to a[i]. Time: O(log n). */
    void addPoint(int i, T val) {
        for (++i; i < n; i += lsb(i)) {
            fw[i] += val;
        }
    }

    /** Returns the prefix sum over [0, i]. Time: O(log n). */
    T prefixSum(int i) const {
        T res = 0;
        for (++i; i > 0; i -= lsb(i)) {
            res += fw[i];
        }
        return res;
    }

    /** Returns the range sum over [l, r]. Time: O(log n). */
    T rangeSum(int l, int r) const {
        if (l > r) return 0;
        return prefixSum(r) - prefixSum(l-1);
    }

    /** Returns the first index i with prefixSum(i) >= target, or -1 if none exists. Time: O(log n). */
    int lowerBound(T target) const {
        if (target <= 0) return 0;
        int idx = 0;
        T curSum = 0;
        for (int step = 1 << __lg(n-1); step > 0; step >>= 1) {
            int next = idx + step;
            if (next < n && curSum + fw[next] < target) {
                idx = next;
                curSum += fw[next];
            }

        }
        // idx is the last position with prefix sum < target
        // answer is idx (1-indexed) + 1, converted to 0-indexed => idx
        if (idx == n-1) return -1; // target > total sum
        return idx;             // 0-indexed answer
    }
};
