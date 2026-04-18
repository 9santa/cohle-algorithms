#include <bits/stdc++.h>
using namespace std;

// Efficient & minimized DSU implementation stolen from Neal Wu
// Don't know who came up with this, but super cute
struct DSU {
private:
    // data[x] < 0 => x is the root of this set, set size = -data[x]
    // data[x] > 0 => data[x] is the parent (and root) of the set 'x' is in
    vector<int> data;
    vector<int> min_el;
    vector<int> max_el;

public:
    // usually want to pass n+1 for construction
    void build(int size) {
        data.assign(size, -1);
        min_el.resize(size);
        max_el.resize(size);
        for (int i = 0; i < size; i++) {
            min_el[i] = max_el[i] = i;
        }
    }

    int find(int x) {
        return data[x] < 0 ? x : data[x] = find(data[x]);
    }

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

    int setSize(int x) {
        return (-data[find(x)]);
    }
};
