#include "../../header.h"
#include "../../alg/acted_monoid/sum_add.h"

// Lazy segment tree over an acted monoid
// Range updates via ActedMonoid::act(), range query via Monoid_X::op()
template<typename ActedMonoid>
struct LazySegtree {
    using AM = ActedMonoid;
    using MX = typename AM::Monoid_X;
    using MA = typename AM::Monoid_A;
    using X = typename MX::value_type;
    using A = typename MA::value_type;
    int n, log, size;   // n = array size, size = actual segtree size
    V<X> data;
    V<A> lazy;

    LazySegtree() {}
    LazySegtree(int n) { build(n); }
    template<typename F>
    LazySegtree(int n, F f) { build(n, f); }
    LazySegtree(const V<X>& v) { build(v); }

    // build with identity element since we don't have the array yet
    void build(int m) {
        build(m, [](int i) -> X {return MX::id(); });
    }
    // build from array
    void build(const V<X>& v) {
        build(sz(v), [&v](int i) -> X { return v[i]; });
    }
    template<typename F>
    // build with a custom function F
    void build(int m, F f) {
        n = m, log = 1;
        while ((1<<log) < n) log++;
        size = 1 << log;
        data.assign(size<<1, MX::id());
        lazy.assign(size, MA::id());
        F0R(i, n) data[size+i] = f(i);
        for (int i = size-1; i >= 1; i--) update(i);
    }

    void update(int p) { data[p] = MX::op(data[p<<1], data[p<<1|1]); }

    // set leaf p = x, push stored lazy before assignment, update parents after assignment
    void set(int p, X x) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = log; i >= 1; i--) push(p>>i);
        data[p] = x;
        for (int i = 1; i <= log; i++) update(p>>i);
    }
    // if MX = multiplicative Monoid
    void multiply(int p, X x) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = log; i >= 1; i--) push(p>>i);
        data[p] = MX::op(data[p], x);
        for (int i = 1; i <= log; i++) update(p>>i);
    }

    // get value at index p, push stored lazy if it exists before returning the value
    X get(int p) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = log; i >= 1; i--) push(p>>i);
        return data[p];
    }

    V<X> get_all() {
        FOR(i, 1, size) { push(i); }
        return {data.begin() + size, data.begin() + size + n};
    }

    // [L, R) range query (Monoid::op operation)
    X prod(int l, int r) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return MX::id();
        l += size, r += size;
        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r-1) >> i);
        }
        X xl = MX::id(), xr = MX::id();
        while (l < r) {
            if (l&1) xl = MX::op(xl, data[l++]);
            if (r&1) xr = MX::op(data[--r], xr);
            l >>= 1, r >>= 1;
        }
        return MX::op(xl, xr);
    }

    // whole tree
    X prod_all() { return data[1]; }

    void apply(int l, int r, A a) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return;
        l += size, r += size;
        // push top-down along the two boundary root-to-leaf paths
        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r-1) >> i);
        }
        int l2 = l, r2 = r;
        // apply the A act operation
        while (l < r) {
            if (l&1) apply_at(l++, a);
            if (r&1) apply_at(--r, a);
            l >>= 1, r >>= 1;
        }
        l = l2, r = r2;
        // recompute affected ancestors on boundary paths (those whose children changed)
        for (int i = 1; i <= log; i++) {
            if (((l >> i) << i) != l) update(l>>i);
            if (((r >> i) << i) != r) update((r-1) >> i);
        }
    }

    template<typename F>
    int max_right(const F check, int l) {
        assert(0 <= l && l <= n);
        assert(check(MX::id()));
        if (l == n) return n;
        l += size;
        for (int i = log; i >= 1; i--) push(l>>i);
        X sm = MX::id();
        do {
            while (l%2 == 0) l >>= 1;
            if (!check(MX::op(sm, data[l]))) {
                while (l < size) {
                    push(l);
                    l = 2*l;
                    if (check(MX::op(sm, data[l]))) { sm = MX::op(sm, data[l++]); }
                }
                return l - size;
            }
            sm = MX::op(sm, data[l++]);
        } while ((l & -l) != l);
        return n;
    }

    template<typename F>
    int min_left(const F check, int r) {
        assert(0 <= r && r <= n);
        assert(check(MX::id()));
        if (r == 0) return 0;
        r += size;
        for (int i = log; i >= 1; i++) push((r-1) >> i);
        X sm = MX::id();
        do {
            r--;
            while (r > 1 && (r%2)) r >>= 1;
            if (!check(MX::op(sm, data[r]))) {
                while (r < size) {
                    push(r);
                    r = r<<1|1;
                    if (check(MX::op(sm, data[r]))) { sm = MX::op(sm, data[r--]); }
                }
                return r + 1 - size;
            }
            sm = MX::op(sm, data[r]);
        } while ((r & -r) != r);
        return 0;
    }

private:
    // apply act() at node p, if it's internal node -> store lazy tag
    void apply_at(int p, A a) {
        ll _sz = 1 << (log - topbit(p));
        data[p] = AM::act(data[p], a, _sz);
        if (p < size) lazy[p] = MA::op(lazy[p], a);
    }

    // propagate p's pending lazy action to its children and clear it
    void push(int p) {
        if (lazy[p] == MA::id()) return;
        apply_at(p<<1, lazy[p]), apply_at(p<<1|1, lazy[p]);
        lazy[p] = MA::id();
    }
};
