
template<typename E, int K>
struct Monoid_Add_Array {
    using value_type = array<E, K>;
    using X = value_type;
    static X op(X x, X y) {
        for (int i = 0; i < K; i++) {
            x[i] += y[i];
        }
        return x;
    }
    static constexpr X id() { return X{}; }
    static constexpr X inverse(X x) {
        for (auto& v : x) v = -v;
        return x;
    }
    static constexpr X multiply(X x, ll b) {
        for (auto& v : x) v *= E(b);
        return x;
    }
    static constexpr bool commute = 1;
};
