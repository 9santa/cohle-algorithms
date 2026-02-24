
namespace io {
    inline void setIn(std::string s) { freopen(s.c_str(), "r", stdin); }
    inline void setOut(std::string s) { freopen(s.c_str(), "w", stdout); }
    inline void setIO(std::string s = "") {
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);
        if (s.length()) {
            setIn(s + ".in"), setOut(s + ".out");
        }
    }

    // input
    inline void re(double &x) {
        std::string t;
        std::cin >> t;
        x = std::stod(t);
    }
    inline void re(long double &x) {
        std::string t;
        std::cin >> t;
        x = std::stold(t);
    }
    template <typename T> inline void re(T &x) { std::cin >> x; }
    template <typename Arg, typename... Args> void re(Arg &fi, Args &...rest) {
        re(fi);
        re(rest...);
    }
    template <typename T>
    std::istream &operator>>(std::istream &is, std::vector<T> &a) {
        F0R(i, sz(a)) is >> a[i];
        return is;
    }

    // output
    template <typename T> inline void print(const std::vector<T> &a) {
        for (const auto &x : a)
            std::cout << x << " ";
        std::cout << "\n";
    }
    template <typename T> inline void print(const std::set<T> &a) {
        for (const auto &x : a)
            std::cout << x << " ";
        std::cout << "\n";
    }
    template <typename T> inline void print(const std::unordered_set<T> &a) {
        for (const auto &x : a)
            std::cout << x << " ";
        std::cout << "\n";
    }
    template <typename T> inline void print(T &&x) { std::cout << x << "\n"; }
    template <typename T, typename... S> void print(T &&x, S &&...y) {
        std::cout << x << " ";
        print(std::forward<S>(y)...);
    }
} // namespace io
