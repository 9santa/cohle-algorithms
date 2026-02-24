#include <bits/stdc++.h>

using namespace std;

vector<int> findMinHeightTrees(int n, vector<vector<int>>& edges) {
    if(n == 1) return {0}; // Single node tree (the root)

    // Build adjacency list
    vector<vector<int>> tree(n);
    for(auto &e : edges) {
        tree[e[0]].push_back(e[1]);
        tree[e[1]].push_back(e[0]);
    }

    // Collect leaves
    vector<int> leaves;
    for(int i = 0; i < n; i++) {
        if(tree[i].size() == 1) leaves.push_back(i);
    }

    int rem = n;
    while(rem > 2) {
        rem -= leaves.size();
        vector<int> new_leaves;
        for(auto leaf : leaves) {
            int neighbor = tree[leaf][0];

            auto &neighbors = tree[neighbor];
            neighbors.erase(remove(neighbors.begin(), neighbors.end(), leaf), neighbors.end());

            if(neighbors.size() == 1) new_leaves.push_back(neighbor);
        }
        leaves = new_leaves;
    }
    return leaves;
}


