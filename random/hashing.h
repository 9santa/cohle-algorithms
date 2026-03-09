
#include <chrono>
struct splitmix_hash {
    static u64 splitmix64(u64 x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    static u64 fixed_random() {
        static const u64 R = (u64)chrono::steady_clock::now().time_since_epoch().count();
        return R;
    }

    // hash combine mixing for 64-bit
    static u64 combine(u64 h, u64 x) {
        x = splitmix64(x + fixed_random());
        h ^= x + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        return h;
    }

    // Hash any integral type (int, long long, size_t, u64, ...)
    template<class T>
    requires std::is_integral_v<T>
    size_t operator()(T x) const {
        using U = std::make_unsigned_t<T>;
        return (size_t)splitmix64((u64)(U)x + fixed_random());
    }

    // Hash pair
    template<class A, class B>
    size_t operator()(const pair<A, B>& p) const {
        u64 R = fixed_random();
        u64 h = splitmix64(R);
        h = combine(h, (*this)(p.first));
        h = combine(h, (*this)(p.second));
        return (size_t)h;
    }

    // Hash std::array
    template<class T, size_t N>
    size_t operator()(const array<T, N>& a) const {
        u64 R = fixed_random();
        u64 h = splitmix64(R ^ (u64)N);
        for (const auto& el : a) {
            h = combine(h, (*this)(el));
        }
        return (size_t)h;
    }

    // Hash std::vector
    template<class T>
    size_t operator()(const vector<T>& v) const {
        u64 R = fixed_random();
        u64 h = splitmix64(R ^ (u64)v.size());
        for (const auto& el : v) {
            h = combine(h, (*this)(el));
        }
        return (size_t)h;
    }
};
