
// f(x) = max(x, a) + b
/*
Composition: f(x) = max(x + a, c), g(x) = max(x + d, f)
h(x) = g(f(x)) = max(max(x+a, c) + d, f)
h.first = a + d
h.second = max(c + d, f)
*/
template<typename E>
struct Monoid_Add_Chmax {
    using value_type = pair<E, E>; // first -> how much to add, second -> lower bound after adding
    using X = value_type;
    static X op(X x, X y) {
        auto& [a, c] = x;
        auto& [d, f] = y;
        a = a + d;
        c = (c == -infty<E> ? c : c + d);
        c = max(c, f);
        return {a, c};
    }
    static E eval(X f, E a) {
        return max(a + f.first, f.second);
    }
    static X add(E a) { return {a, -infty<E>}; }
    static X chmax(E a) { return {0, a}; }
    static constexpr X id() { return {0, -infty<E>}; }
    static constexpr bool commute = false;
};
