
/** Fenwick tree for point additions and prefix/range sum queries. Space: O(n). */
template<class T>
struct Fenwick {
    int n;
    vector<T> fw;

    /** Creates an empty tree with the given number of elements. */
    Fenwick(int _n) { n = _n+1; fw.assign(n, 0); }

    /** Returns the least significant set bit of i. */
    static int lsb(int i) { return i & -i; }

    /** Builds the tree from a 0-indexed array. Time: O(n). */
    void build(const vector<T>& a) {
        n = sz(a) + 1;
        fw.assign(n, 0);
        for (int i = 1; i < n; i++) {
            fw[i] += a[i-1];
            if (i + lsb(i) <= sz(a)) {
                fw[i + lsb(i)] += fw[i];
            }
        }
    }

    /** Adds val to a[i]. Time: O(log n). */
    void update(int i, T val) {
        for (++i; i < n; i += lsb(i)) {
            fw[i] += val;
        }
    }

    /** Returns the prefix sum over [0, i]. Time: O(log n). */
    T get(int i) {
        T res = 0;
        for (++i; i > 0; i -= lsb(i)) {
            res += fw[i];
        }
        return res;
    }

    /** Returns the range sum over [l, r]. Time: O(log n). */
    T getRange(int l, int r) {
        return get(r) - get(l-1);
    }

    /** Returns the first index i with prefix sum >= x, assuming x is reachable. Time: O(log n). */
    int lower_bound(T x) {
        int pos = 0;
        T sum = 0;
        for (int i = 31-__builtin_clz(n); i >= 0; i--) {
            int nxt = pos + (1 << i);
            if (nxt < n && sum + fw[nxt] < x) {
                sum += fw[nxt];
                pos = nxt;
            }
        }
        return pos;
    }
};
