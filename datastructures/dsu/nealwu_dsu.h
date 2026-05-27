#include <bits/stdc++.h>
using namespace std;

/** Compact disjoint-set union with component size/min/max tracking. Space: O(n). */
struct DSU {
private:
    // data[x] < 0 => x is the root of this set, set size = -data[x]
    // data[x] > 0 => data[x] is the parent (and root) of the set 'x' is in
    vector<int> data;
    vector<int> min_el;
    vector<int> max_el;

public:
    /** Initializes size singleton sets. Time: O(n). */
    void build(int size) {
        data.assign(size, -1);
        min_el.resize(size);
        max_el.resize(size);
        for (int i = 0; i < size; i++) {
            min_el[i] = max_el[i] = i;
        }
    }

    /** Returns the representative of x with path compression. Amortized time: O(alpha n). */
    int find(int x) {
        return data[x] < 0 ? x : data[x] = find(data[x]);
    }

    /** Merges the sets containing x and y. Amortized time: O(alpha n). */
    void unionSets(int x, int y) {
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
    int setSize(int x) {
        return (-data[find(x)]);
    }
};
