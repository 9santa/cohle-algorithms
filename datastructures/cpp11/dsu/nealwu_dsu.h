#include <bits/stdc++.h>

using namespace std;

/** Compact DSU with component size, min, and max tracking. Space: O(n). */
struct DSU {
    vector<int> data;
    vector<int> min_el;
    vector<int> max_el;

    /** Initializes n singleton sets. Time: O(n). */
    explicit DSU(int n) {
        data.assign(n, -1);
        min_el.resize(n);
        max_el.resize(n);
        for (int i = 0; i < n; i++) {
            min_el[i] = max_el[i] = i;
        }
    }

    /** Returns the representative of x with path compression. Amortized time: O(alpha n). */
    int find(int x) {
        return data[x] < 0 ? x : data[x] = find(data[x]);
    }

    /** Merges the sets containing x and y. Amortized time: O(alpha n). */
    void union_sets(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) return;

        if (data[x] > data[y]) swap(x, y);

        data[x] += data[y];
        data[y] = x;

        min_el[x] = min(min_el[x], min_el[y]);
        max_el[x] = max(max_el[x], max_el[y]);
    }

    /** Returns the size of the set containing x. Amortized time: O(alpha n). */
    int set_size(int x) {
        return (-data[find(x)]);
    }
};
