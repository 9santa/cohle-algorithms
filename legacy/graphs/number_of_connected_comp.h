class NumberOfSCCs {
private:
    vector<int> parent;
    vector<int> rank;
    int count;
public:
    int find(int x) {
        if(parent[x] == x) return x;
        return (parent[x] = find(parent[x]));
    }

    void unionSets(int x, int y) {
        int root_x = find(x);
        int root_y = find(y);

        if(root_x == root_y) return;

        if(rank[root_x] > rank[root_y]) swap(root_x, root_y);
        parent[root_x] = root_y;
        if(rank[root_x] == rank[root_y]) {
            rank[root_y]++;
        }
        count--;
    }

    int findCircleNum(vector<vector<int>>& isConnected) {
        int n = (int)isConnected.size();
        parent.resize(n+1);
        rank.assign(n+1, 0);
        count = n;

        for(int i = 1; i <= n; i++) {
            parent[i] = i;
            rank[i] = 0;
        }

        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                if(isConnected[i][j] == 1) {
                    unionSets(i, j);
                }
            }
        }

        return count;
    }
};
