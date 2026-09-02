#include <algorithm>
#include <climits>

constexpr int MAXN = 100;

// The simplest & slowest & dumbest heap implementation
/** Unordered fixed-capacity heap with O(1) insert and O(n) minimum operations. */
struct heap_v1 {
private:
    int h[MAXN];
    int n = 0;  // current size of the heap

public:
    // insert an elemenet at the end. O(1)
    /** Inserts x. Time: O(1). */
    void insert(int x) {
        h[n++] = x;
    }

    // get minimum element in the heap. O(n)
    /** Returns the minimum element. Time: O(n). */
    int get_min() const {
        int res = INT_MAX;
        for (int i = 0; i < n; i++) {
            res = std::min(res, h[i]);
        }
        return res;
    }

    // remove min of the heap. O(n)
    /** Removes the minimum element. Time: O(n). */
    void remove_min() {
        int j = 0;
        for (int i = 1; i < n; i++) {
            if (h[j] > h[i]) j = i;
        }
        std::swap(h[j], h[n-1]);
        n--;
    }
};

// heap with storing the min element
/** Unordered heap that stores the current minimum index. Space: O(MAXN). */
struct heap_v2 {
private:
    int h[MAXN];
    int n = 0;
    int m = 0;

public:
    // O(1)
    /** Returns the minimum element. Time: O(1). */
    int get_min() const {
        return h[m];
    }

    // O(n)
    /** Removes the minimum element. Time: O(n). */
    void remove_min() {
        std::swap(h[m], h[n-1]);
        n--;
        // update new minimum
        m = 0;
        for (int i = 1; i < n; i++) {
            if (h[i] < h[m]) m = i;
        }
    }

    // O(1)
    /** Inserts x. Time: O(1). */
    void insert(int x) {
        h[n++] = x;
        if (x < h[m]) m = n-1;
    }
};

// heap with fast minimum element deletion
/** Sorted-array heap variant with O(1) minimum operations and O(n) insert. */
struct heap_v3 {
private:
    int h[MAXN];
    int n = 0;

public:
    // O(1)
    /** Removes the minimum element. Time: O(1). */
    void remove_min() {
        n--;    // careful no out of bounds check here
    }

    // O(1)
    /** Returns the minimum element. Time: O(1). */
    int get_min() {
        return h[--n];
    }

    // sift-left (1-run of insertion sort). O(n)
    /** Inserts x while maintaining order. Time: O(n). */
    void insert(int x) {
        h[n++] = x;
        int j = n-1;
        while (j > 0 && h[j] > h[j-1]) {
            std::swap(h[j], h[j-1]);
            j--;
        }
    }
};
