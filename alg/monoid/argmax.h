
// Typical usage: RMQ
template<typename E>
struct ArgMax {
    using X = pair<E, int>; // (value, index)
    static bool op(const X& a, const X& b) {
        if (a.first != b.first) return a.first > b.first;
        return a.second < b.second;
    }
    static X id() { return {std::numeric_limits<E>::lowest(), INT_MAX}; }
};
