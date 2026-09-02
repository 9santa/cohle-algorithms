
/** Fenwick tree arranged for suffix and range frequency sums. Space: O(n). */
template<typename T>
struct FenwickCounter {
    int n;
    vector<T> fw;

    /** Creates an empty counter over indices [1, n]. */
    FenwickCounter(int n_) : n(n_), fw(n+1, 0) {}

    static int lsb(int i) { return i & -i; }

    /** Adds delta at index i. Time: O(log n). */
    void point_add(int i, T delta) {
        for (; i > 0; i -= lsb(i)) fw[i] += delta;
    }

    /** Returns the suffix sum over [i, n]. Time: O(log n). */
    ll suf(int i) const {
        ll res = 0;
        for (; i <= n; i += lsb(i)) res += fw[i];
        return res;
    }

    /** Returns the range sum over [l, r]. Time: O(log n). */
    ll range_sum(int l, int r) const {
        if (l > r) return 0;
        return suf(l) - (r+1 <= n ? suf(r+1) : 0);
    }
};
