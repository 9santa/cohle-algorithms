#include "../../header.h"

template<class Monoid>
struct SegTree {
    using X = typename Monoid::value_type;
    V<X> dat;
    int n, log, size;

    SegTree() {}
    SegTree(int n) { build(n); }
    template<typename F>
    SegTree(int n, F f) {
        build(n, f);
    }
    SegTree(const V<X>& v) { build(v); }

    void build(int m) {
        build(m, [](int i) -> X { return Monoid::id(); });
    }
    void build(const V<X>& v) {
        build(sz(v), [&](int i) -> X { return v[i]; });
    }
    template<typename F>
    void build(int m, F f) {
        n = m, log = 1;
        while ((1 << log) < n) log++;
        size = 1 << log;
        dat.assign(size << 1, Monoid::id());
        F0R(i, n) dat[size + i] = f(i);
        ROF(i, 1, size) update(i);
    }

    X get(int i) const { return dat[size+i]; }
    V<X> get_all() const { return {dat.begin() + size, dat.begin() + size + n}; }

    void update(int i) { dat[i] = Monoid::op(dat[2*i], dat[2*i+1]); }
    void set(int i, const X& x) {
        assert(i < n);
        dat[i += size] = x;
        while (i >>= 1) update(i);
    }

    // [L, R)
    X prod(int L, int R) {
        assert(0 <= L && L <= R && R <= n);
        X vl = Monoid::id(), vr = Monoid::id();
        L += size, R += size;
        while (L < R) {
            if (L & 1) vl = Monoid::op(vl, dat[L++]);
            if (R & 1) vr = Monoid::op(dat[--R], vr);
            L >>= 1, R >>= 1;
        }
        return Monoid::op(vl, vr);
    }
};
