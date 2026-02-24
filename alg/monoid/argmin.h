
// Typical usage: RMQ
template<typename E>
struct ArgMin {
    using X = pair<E, int>; // (value, index)
    static bool op(const X& a, const X& b) {
        return a < b; // lexicographic => leftmost on ties
    }
    static X id() { return {std::numeric_limits<E>::max(), INT_MAX}; }
};
