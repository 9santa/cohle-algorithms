#include "../../header.h"

template<class Monoid>
struct DisjointSparseTable {
    using MX = Monoid;
    using X = typename MX::value_type;

    int n = 0;
    int LOG = 0; // number of levelsj
    vector<vector<X>> data; // data[k][i] either ending or stating at 'i', on level 'k'

    explicit DisjointSparseTable(const vector<X>& a) { build(a); }

    template<class F>
    DisjointSparseTable(int n_, F f) { build(n_, f); }

    // Build from vector
    void build(const vector<X>& a) {
        build((int)a.size(), [&](int i) -> X { return a[i]; });
    }

    // Build from function f(i). O(n log n)
    template<class F>
    void build(int n_, F f) {
        n = n_;
        if (n == 0) { LOG = 0; data.clear(); return; }
        LOG = (n <= 1) ? 0 : 32 - __builtin_clz((unsigned)(n-1));
        data.assign(LOG, vector<X>(n));

        // level 0 holds the original array values
        for (int i = 0; i < n; i++) data[0][i] = f(i);
        // build higher levels. each level k uses block size B = 2^k
        for (int k = 1; k < LOG; k++) {
            const int B = 1 << k;
            data[k] = data[0]; // start with init values, then overwrite inside each block with prefix/suffix data
            for (int mid = B; mid < n; mid += B << 1) {
                int L = mid - B;
                int R = min(n, mid + B);
                // left half: compute suffixes ending at mid-1
                // data[k][i] = a[i] op ... op a[mid-1]
                for (int i = mid-2; i >= L; i--) {
                    data[k][i] = MX::op(data[0][i], data[k][i+1]);
                }

                // right half: compute prefixes starting at mid
                // data[k][i] = a[mid] op ... op a[i]
                if (mid < R) {
                    for (int i = mid + 1; i < R; i++) {
                        data[k][i] = MX::op(data[k][i-1], data[0][i]);
                    }
                }
            }
        }
    }

    // Query [l, r). O(1)
    X prod(int l, int r) const {
        if (l >= r) return MX::id();
        if (r - l == 1) return data[0][l];
        int a = l;
        int b = r - 1;
        // k = msb of (a^b): highest bit where they differ
        int k = 31 - __builtin_clz((unsigned)(a ^ b));
        return MX::op(data[k][a], data[k][b]);
    }
};
