#include "../../math/matrix_dynamic.h"

/** Matrix multiplication monoid for dynamic matrices. Space: O(1). */
template<int MAXN, class ModInt>
struct Monoid_MatDyn {
    using value_type = MatDyn<MAXN, ModInt>;
    using X = value_type;
    int n = 0;
    Monoid_MatDyn() = default;
    explicit Monoid_MatDyn(int n_) : n(n_) {}
    /** Multiplies two matrices. Time: O(n^3). */
    static X op(const X& x, const X& y) { return x * y; }
    /** Returns the identity matrix for the active dimension. Time: O(n). */
    X id() const { return X::identity(n); }
};
