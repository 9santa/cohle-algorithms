#include "../../header.h"

/* Sparse Table for associative Monoid
   Query: O(log n) with binary decomp
   Preserves order: res = op(res, block), so non-commutative is fine
   Memory: O(n log n), without padding to power-of-two */
template<class Monoid>
struct SparseTable_Monoid_Decomp {
    using X = typename Monoid::value_type;
    int n = 0; // original size
    int LOG = 0; // number of levels
    vector<vector<X>> st; // st[k][i] = prod of length 2^k starting at i
    explicit SparseTable_Monoid_Decomp(const vector<X>& a) { build(a); }

    // Build from array
    void build(const vector<X>& a) {
        n = (int)a.size();
        if (n == 0) {
            LOG = 0;
            st.clear();
            return;
        }
        LOG = 32 - __builtin_clz(n);
        st.assign(LOG, {});
        st[0] = a;
        // level k stores segments of length 2^k
        // so it has size m = n - 2^k + 1
        for (int k = 1; k < LOG; k++) {
            int len = 1 << k;
            int half = len >> 1;
            int m = n - len + 1;
            st[k].resize(max(0, m));
            for (int i = 0; i < m; i++) {
                st[k][i] = Monoid::op(st[k-1][i], st[k-1][i+half]);
            }
        }
    }

    // Query [l, r). O(log n)
    X prod(int l, int r) const {
        if (l < 0) l = 0;
        if (r > n) r = n;
        if (l >= r) return Monoid::id();

        X res = Monoid::id();
        int len = r - l;
        while (len > 0) {
            int k = 31 - __builtin_clz(len); // largest k: 2^k <= len
            res = Monoid::op(res, st[k][l]);
            l += 1 << k;
            len -= 1 << k;
        }
        return res;
    }
};

