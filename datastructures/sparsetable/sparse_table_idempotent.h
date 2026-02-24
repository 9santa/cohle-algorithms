#include "../../header.h"

// Templated version for different idempotent operations
// Monoid::op(x, y) -> bool (true if x is strictly better)!!!
template<class Monoid>
struct SparseTable {
    using MX = Monoid;
    using X = typename MX::value_type;
    int n, K;
    std::vector<int> table; // size K*n, stores indices
    const std::vector<X>* a = nullptr; // bind to original array (rebindable)

    SparseTable() = default;
    SparseTable(const vector<X>& arr) { build(arr); }

    void build(const std::vector<X> &arr) {
        a = &arr;
        n = (int)arr.size();
        if (n == 0) {K = 0; table.clear(); a = nullptr;}
        K = 32 - __builtin_clz(n); // ceil(log2(n))
        table.resize(K * n);

        // Initialize first level
        for (int i = 0; i < n; i++) table[i] = i;

        // Build the table
        for(int j = 1; j < K; j++) {
            int step = 1 << (j - 1);
            for(int i = 0; i + (1 << j) <= n; i++) {
                auto x = table[(j-1) * n + i];
                auto y = table[(j-1) * n + i + step];
                table[j*n + i] = MX::op((*a)[x], (*a)[y]) ? x : y;
            }
        }
    }

    int query_index(int l, int r) const {
        assert(0 <= l && l <= r && r < n);
        int len = r - l + 1;
        int j = 31 - __builtin_clz(len);
        auto x = table[j*n + l];
        auto y = table[j*n + r - (1 << j) + 1];
        return MX::op((*a)[x], (*a)[y]) ? x : y;
    }

    // Query [l, r] (0-indexed, inclusive)
    X query(int l, int r) const {
        if (n == 0 || l > r) return MX::id();
        if (l < 0) l = 0;
        if (r >= n) r = n - 1;
        return (*a)[query_index(l, r)];
    }
};

