
/** Fenwick tree supporting range additions and range sum queries. Space: O(n). */
template<class T>
struct FenwickRR {
    int n;
    vector<T> fw1, fw2;

    /** Creates an empty tree with the given number of elements. */
    FenwickRR(int _n) { n = _n+1; fw1.assign(n, 0); fw2.assign(n, 0); }

    /** Returns the least significant set bit of i. */
    static int lsb(int i) { return i & -i; }

    /** Adds val to one internal Fenwick tree at index i. Time: O(log n). */
    void point_add(vector<T>& fw, int i, T val) {
        for (++i; i < n; i += lsb(i)) fw[i] += val;
    }

    /** Returns an internal prefix sum up to i. Time: O(log n). */
    T point_query(const vector<T>& fw, int i) const {
        T res = 0;
        for (++i; i > 0; i -= lsb(i)) res += fw[i];
        return res;
    }

    /** Adds val to every element in [l, r]. Time: O(log n). */
    void range_add(int l, int r, T val) {
        point_add(fw1, l, val); point_add(fw1, r+1, -val);
        point_add(fw2, l, val*(l-1)); point_add(fw2, r+1, -val*r);
    }

    /** Returns the prefix sum over [0, i]. Time: O(log n). */
    T prefix(int i) const {
        if (i < 0) return 0;
        return point_query(fw1, i) * (T)(i+1) - point_query(fw2, i);
    }

    /** Returns the range sum over [l, r]. Time: O(log n). */
    T range_query(int l, int r) const {
        if (l > r) return 0;
        return prefix(r) - prefix(l-1);
    }
};
