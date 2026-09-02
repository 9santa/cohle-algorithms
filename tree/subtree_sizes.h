#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Returns subtree sizes when the tree is rooted at root. Time: O(n). */
vector<int> subtree_sizes(const vector<vector<int>>& g, int root = 0) {
    int n = (int)g.size();
    vector<int> parent(n, -1), order;
    order.reserve(n);
    parent[root] = root;

    vector<int> st = {root};
    while (!st.empty()) {
        int v = st.back();
        st.pop_back();
        order.push_back(v);
        for (int to : g[v]) {
            if (to == parent[v]) continue;
            parent[to] = v;
            st.push_back(to);
        }
    }

    vector<int> sub(n, 1);
    for (int i = n - 1; i > 0; --i) {
        int v = order[i];
        sub[parent[v]] += sub[v];
    }
    return sub;
}
