
template<typename E>
struct Monoid_Sum {
    using value_type = E;
    using X = value_type;

    static constexpr X op(const X& x, const X& y) {
        if constexpr (std::is_arithmetic_v<E>) {
            return x + y;
        } else {
            X res = x;
            for (int i = 0; i < sz(x); i++) {
                res[i] += y[i];
            }
            return res;
        }
    }

    static constexpr X id() {
        if constexpr (std::is_arithmetic_v<E>) {
            return E(0);
        } else {
            X res{};
            for (int i = 0; i < sz(res); i++) res[i] = 0;
            return res;
        }
    }
};
