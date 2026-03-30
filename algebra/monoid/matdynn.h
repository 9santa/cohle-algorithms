#include "../../math/matrix_dynamic.h"

template<int MAXN, class ModInt>
struct Monoid_MatDyn {
    using value_type = MatDyn<MAXN, ModInt>;
    using X = value_type;
    int n = 0;
    Monoid_MatDyn() = default;
    explicit Monoid_MatDyn(int n_) : n(n_) {}
    static X op(const X& x, const X& y) { return x * y; }
    X id() const { return X::identity(n); }
};
