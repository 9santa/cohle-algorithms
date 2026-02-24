
// f(x) = min(x, a) + b
/*
Composition: f(x) = min(x + a, c), g(x) = min(x + d, f)
h(x) = g(f(x)) = min(min(x+a, c) + d, f)
h.first = a + d
h.second = min(c + d, f)
*/
template<typename E>
struct Monoid_Add_Chmin {
    using value_type = pair<E, E>; // first -> how much to add, second -> lower bound after adding
    using X = value_type;
    static X op(X x, X y) {
        auto& [a, c] = x;
        auto& [d, f] = y;
        a = a + d;
        c = (c == infty<E> ? c : c + d);
        c = min(c, f);
        return {a, c};
    }
    static E eval(X f, E a) {
        return min(a + f.first, f.second);
    }
    static X add(E a) { return {a, infty<E>}; }
    static X chmin(E a) { return {0, a}; }
    static constexpr X id() { return {0, infty<E>}; }
    static constexpr bool commute = false;
};
