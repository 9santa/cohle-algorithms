#include <bits/stdc++.h>
#include <cstdint>

#define pb push_back
#define eb emplace_back
#define all(a) (a).begin(), (a).end()

using namespace std;

using ll = long long;
using ull = unsigned long long;
using ld = long double;
using pii = pair<int, int>;

// Disjoin-Set-Union
class DSU {
private:
    vector<uint64_t> parent;
    vector<uint64_t> rank;
    vector<uint64_t> setSize;
    vector<uint64_t> maxElement;
    vector<uint64_t> minElement;
    uint64_t count; // number of disjoint sets

public:
    // Constructor 
    explicit DSU(uint64_t size) {
        parent.resize(size);
        rank.assign(size, 0);
        maxElement.assign(size, 0);
        minElement.assign(size, 0);
        setSize.assign(size, 0);
        count = size;

        // each node is its own parent at the start
        for (uint64_t i = 0; i < size; i++) { 
            parent[i] = i;
            rank[i] = 0;
            setSize[i] = 1;
            maxElement[i] = i;
            minElement[i] = i;
        }
    }

    // Find operation: Go up the tree to the root of a component (set)
    int find(uint64_t x) {
        // using path compression
        if(parent[x] == x) {
            return x;
        }
        // moving pointers to the root of this set (union)
        return (parent[x] = find(parent[x]));
    }

    // Union smaller set to larger
    void unionSets(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);

        if (rootX == rootY) return; // already in the same set
        
        // Smaller rank set to larger rank set
        if(rank[rootX] > rank[rootY]) {
            std::swap(rootX, rootY);
        }
        parent[rootX] = rootY;
        if(rank[rootX] == rank[rootY]) {
            rank[rootY]++;
        }
        setSize[rootY] += setSize[rootX];

        maxElement[rootY] = std::max(maxElement[x], maxElement[y]);
        minElement[rootY] = std::min(minElement[x], minElement[y]);

        count--;
    }

    bool isConnected(int x, int y) {
        return find(x) == find(y);
    }

    // function returns a vector of all the elements from i to root of this union
    vector<uint64_t> getParents(uint64_t i) {
        vector<uint64_t> ans;
        while(parent[i] != i) {
            ans.push_back(i);
            i = parent[i];
        }
        ans.push_back(i);
        return ans;
    }

    uint64_t size(uint64_t i) { return setSize[find(i)]; }

    uint64_t get_max(uint64_t i) { return maxElement[find(i)]; }

    uint64_t get_min(uint64_t i) { return minElement[find(i)]; }
};

static void test1() {
    uint64_t n = 10;
    DSU d(n+1);
    d.unionSets(2, 1);  // performs union operation on 1 and 2
    d.unionSets(1, 4);
    d.unionSets(8, 1);
    d.unionSets(3, 5);
    d.unionSets(5, 6);
    d.unionSets(5, 7);
    d.unionSets(9, 10);
    d.unionSets(2, 10);

    vector<uint64_t> ans = {7, 5};
    for(uint64_t i = 0; i < ans.size(); i++) {
        assert(d.getParents(7).at(i) == ans[i]);
    }
    cout << "***1st test passed!***\n";
}

