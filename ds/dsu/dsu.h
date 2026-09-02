#pragma once
#include <bits/stdc++.h>
using namespace std;

using u64 = uint64_t;

/** Disjoint-set union with component size, min, and max tracking. Space: O(n). */
struct DSU {
private:
    vector<u64> parent;
    vector<u64> rank;
    vector<u64> set_size;
    vector<u64> min_el;
    vector<u64> max_el;
    u64 set_count;

public:
    /** Initializes n singleton sets. Time: O(n). */
    void build(u64 size) {
        parent.resize(size);
        rank.assign(size, 0);
        min_el.assign(size, 0);
        max_el.assign(size, 0);
        set_size.assign(size, 1);
        set_count = size;

        // each node is its own parent at the beginning
        for (u64 i = 0; i < size; i++) {
            parent[i] = i;
            rank[i] = 0;
            set_size[i] = 1;
            min_el[i] = i;
            max_el[i] = i;
        }
    }

    /** Returns the representative of x with path compression. Amortized time: O(alpha n). */
    u64 find(u64 x) {
        // path compression
        if (parent[x] == x) return x;
        // moving pointers upwards to the root of this set
        return (parent[x] = find(parent[x]));
    }

    /** Merges the sets containing x and y. Amortized time: O(alpha n). */
    void unionSets(u64 x, u64 y) {
        u64 root_x = find(x);
        u64 root_y = find(y);
        if (root_x == root_y) return; // already in the same set

        // smaller rank set to larger rank set
        if (rank[root_x] > rank[root_y]) {
            std::swap(root_x, root_y);
        }
        parent[root_x] = root_y;
        if (rank[root_x] == rank[root_y]) rank[root_y]++;
        set_size[root_y] += set_size[root_x];

        min_el[root_y] = std::min(min_el[root_x], min_el[root_y]);
        max_el[root_y] = std::max(max_el[root_x], max_el[root_y]);
        set_count--;
    }

    /** Returns whether x and y are in the same set. Amortized time: O(alpha n). */
    bool isConnected(u64 x, u64 y) {
        return find(x) == find(y);
    }

    /** Returns the parent chain from x to its representative. Time: O(tree height). */
    vector<u64> getParents(u64 x) {
        vector<u64> res;
        while (parent[x] != x) { // while not at the root of this set
            res.push_back(x);
            x = parent[x];
        }
        res.push_back(x);
        return res;
    }

    /** Returns the size of the set containing x. Amortized time: O(alpha n). */
    u64 size(u64 x) { return set_size[find(x)]; }

    /** Returns the minimum index in the set containing x. Amortized time: O(alpha n). */
    u64 getMin(u64 x) { return min_el[find(x)]; }

    /** Returns the maximum index in the set containing x. Amortized time: O(alpha n). */
    u64 getMax(u64 x) { return max_el[find(x)]; }
};
