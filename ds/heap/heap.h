#pragma once
#include <bits/stdc++.h>
using namespace std;

constexpr int MAXN = 100;

/** Fixed-capacity binary min-heap. Space: O(MAXN). */
struct heap {
    int h[MAXN];
    int n = 0;

    /** Returns the minimum element. Time: O(1). */
    int get_min() const {
        return h[0];
    }

    /** Inserts x into the heap. Time: O(log n). */
    void insert(int x) {
        h[n++] = x;
        // sift-up
        int i = n-1;
        while (i > 0 && h[i] < h[(i-1)/2]) {
            std::swap(h[i], h[(i-1)/2]);
            i = (i-1)/2;
        }
    }

    /** Removes the minimum element. Time: O(log n). */
    void remove_min() {
        // sift-down
        std::swap(h[0], h[--n]);
        int i = 0;
        while (2*i+1 < n) {
            int j = 2*i+1;
            if (2*i+2 < n && h[2*i+2] < h[j]) {
                j = 2*i+2;
            }
            if (h[i] <= h[j]) break;
            else {
                std::swap(h[i], h[j]);
                i = j;
            }
        }
    }
};
