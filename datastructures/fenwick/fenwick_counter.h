
template<typename T>
struct FenwickCounter {
    int n;
    vector<T> fw;

    FenwickCounter(int n_) : n(n_), fw(n+1, 0) {}

    static int lsb(int i) { return i & ~i; }

    void point_add(int i, T delta) {
        for (; i > 0; i -= lsb(i)) fw[i] += delta;
    }

    ll suf(int i) const {
        ll res = 0;
        for (; i <= n; i += lsb(i)) res += fw[i];
        return res;
    }

    ll range_sum(int l, int r) const {
        if (l > r) return 0;
        return suf(l) - (r+1 <= n ? suf(r+1) : 0);
    }
};
