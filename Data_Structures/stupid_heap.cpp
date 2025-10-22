#include <algorithm>
#include <climits>

constexpr int MAXN = 100;

// The simplest & slowest & dumbest heap implementation
struct heap_v1 {
private:
    int h[MAXN];
    int n = 0;  // current size of the heap

public:
    // insert an elemenet at the end. O(1)
    void insert(int x) {
        h[n++] = x;
    }

    // get minimum element in the heap. O(n)
    int get_min() const {
        int res = INT_MAX;
        for (int i = 0; i < n; i++) {
            res = std::min(res, h[i]);
        }
        return res;
    }

    // remove min of the heap. O(n)
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
struct heap_v2 {
private:
    int h[MAXN];
    int n = 0;
    int m;

public:
    // O(1)
    int get_min() const {
        return h[m];
    }

    // O(n)
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
    void insert(int x) {
        h[n++] = x;
        if (x < h[m]) m = n-1;
    }
};

// heap with fast minimum element deletion
struct heap_v3 {
private:
    int h[MAXN];
    int n = 0;

public:
    // O(1)
    void remove_min() {
        n--;    // careful no out of bounds check here
    }

    // O(1)
    int get_min() {
        return h[n--];
    }

    // sift-left (1-run of insertion sort). O(n)
    void insert(int x) {
        h[n++] = x;
        int j = n-1;
        while (j > 0 && h[j] > h[j-1]) {
            std::swap(h[j], h[j-1]);
            j--;
        }
    }
};
