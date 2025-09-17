// Disjoin-Set-Union
class DSU {
private:
    vector<int> parent;
    vector<int> rank;

public:
    // Constructor 
    explicit DSU(int size) {
        parent.resize(size);
        rank.assign(size, 0);

        // each node is its own parent at the start
        for (int i = 0; i < size; i++) { 
            parent[i] = i;
            rank[i] = 0;
        }
    }

    // Find operation: Go up the tree to the root of a component (set)
    int find(int x) {
        // using path compression
        if(parent[x] == x) {
            return x;
        }
        // moving pointers to the root of this set
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
    }

    bool isConnected(int x, int y) {
        return find(x) == find(y);
    }
};


class Solution {
public:
    vector<int> findRedundantConnection(vector<vector<int>>& edges) {
        int V = (int)edges.size();
        DSU dsu(V+1);
        
        for(auto &e : edges) {
            if(dsu.isConnected(e[0], e[1])) {
                return e;
            }
            dsu.unionSets(e[0], e[1]);
        }

        return {};
    }
};
