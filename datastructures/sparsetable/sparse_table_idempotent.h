
// Templated version for different idempotent operations
template<typename T, typename Compare = std::less<T>>
class GenericSparseTable {
private:
    int n, K;
    std::vector<int> table;
    std::vector<T> &a;
    Compare comp;
    T identity;

public:
    GenericSparseTable(std::vector<T> &arr, Compare cmp = Compare(), T id = T()) : a(arr), comp(cmp), identity(id) {
        n = (int)a.size();
        if (n == 0) return;
        K = 32 - __builtin_clz(n);
        table.resize(K * n);

        // Initialize first level
        for (int i = 0; i < n; i++) {
            table[i] = i;
        }

        // Build the table
        for(int j = 1; j < K; j++) {
            int step = 1 << (j - 1);
            for(int i = 0; i + (1 << j) <= n; i++) {
                auto x = table[(j-1) * n + i];
                auto y = table[(j-1) * n + i + step];
                table[j*n + i] = comp(a[x], a[y]) ? x : y;
            }
        }
    }

    int query_index(int l, int r) {
        int len = r - l + 1;
        int j = 31 - __builtin_clz(len);
        auto x = table[j*n + l];
        auto y = table[j*n + r - (1 << j) + 1];
        return comp(a[x], a[y]) ? x : y;
    }

    // Query [l, r] (0-indexed, inclusive)
    T query(int l, int r) {
        if (l < 0) l = 0;
        if (r >= n) r = n - 1;
        if (l > r) return identity;
        return a[query_index(l, r)];
    }
};

