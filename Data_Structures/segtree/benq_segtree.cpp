
// segment tree for associative operation 'cmb'. O(log n) query, O(n) preprocessing
template<typename T>
struct SegTree {
    const T ID{}; T cmb(T a, T b) { return a+b; }   // ID - identity (neutral) element
    int n; V<T> seg;
    void init(int _n) {
        for (n = 1; n < _n; n *= 2);    // closest (minimal) 2^n >= _n
        seg.assign(2*n,ID); }
    void pull(int p) { seg[p] = cmb(seg[2*p], seg[2*p+1]); }
    void upd(int p, T val) { // set val at position p
        seg[p += n] = val; for (p /= 2; p; p /= 2) pull(p); }
    T query(int l, int r) { // 0-indexed, inclusive
        T ra = ID, rb = ID;
        for (l += n, r += n+1; l < r; l/=2, r/=2) {
            if (l&1) ra = cmb(ra,seg[l++]);
            if (r&1) rb = cmb(seg[--r],rb);
        }
        return cmb(ra,rb);
    }
};

