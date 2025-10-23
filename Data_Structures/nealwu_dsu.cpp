#include <bits/stdc++.h>

using namespace std;

// efficient & minimized DSU implementation stolen from Neal Wu
struct DSU {
    vector<int> data;
    vector<int> min_el;
    vector<int> max_el;

    // usually want to pass n+1 to constructor
    explicit DSU(int n) {
        data.assign(n, -1);
        min_el.resize(n);
        max_el.resize(n);
        for (int i = 0; i < n; i++) {
            min_el[i] = max_el[i] = i;
        }
    }

    int find(int x) {
        return data[x] < 0 ? x : data[x] = find(data[x]);
    }

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

    int set_size(int x) {
        return (-data[find(x)]);
    }
};
