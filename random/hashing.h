
struct splitmix_hash {
    static u64 splitmix64(u64 x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }

    static u64 fixed_random() {
        static const u64 R = (u64)chrono::steady_clock::now().time_since_epoch().count();
        return R;
    }

    // hash combine mixing for 64-bit
    static u64 combine(u64 h, u64 x) {
        x = splitmix64(x);
        h ^= x + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        return h;
    }

    // Hash any integral type (int, long long, size_t, u64, ...)
    template<class T>
    requires std::is_integral_v<T>
    size_t operator()(T x) const {
        u64 R = fixed_random();
        return (size_t)splitmix64((u64)x + R);
    }

    // Hash pair
    template<class A, class B>
    size_t operator()(const pair<A, B>& p) const {
        u64 R = fixed_random();
        u64 h = splitmix64(R);
        h = combine(h, (u64)splitmix_hash{}(p.first));
        h = combine(h, (u64)splitmix_hash{}(p.second));
        return (size_t)h;
    }

    // Hash std::array
    template<class T, size_t N>
    size_t operator()(const array<T, N>& a) const {
        u64 R = fixed_random();
        u64 h = splitmix64(R ^ (u64)N);
        for (const auto& el : a) {
            h = combine(h, (u64)splitmix_hash{}(el));
        }
        return (size_t)h;
    }

    // Hash std::vector
    template<class T>
    size_t operator()(const vector<T>& v) const {
        u64 R = fixed_random();
        u64 h = splitmix64(R ^ (u64)v.size());
        for (const auto& el : v) {
            h = combine(h, (u64)splitmix_hash{}(el));
        }
        return (size_t)h;
    }

    size_t operator()(u64 x) const {
        static const u64 STATIC_RANDOM = chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + STATIC_RANDOM);
    }
};
