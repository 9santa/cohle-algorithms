#include <limits>
#include <numeric>
#include <iostream>
#include <random>
#include <vector>
#include <chrono>

using namespace std;
using ll = long long;

ll gcdll(ll a, ll b) {
    while (b != 0) {
        ll t = a % b;
        a = b;
        b = t;
    }
    return a < 0 ? -a : a;
}


template<typename F>
ll benchmark(F f, const std::vector<ll>& xs, const std::vector<ll>& ys, ll repeat = 10) {
    using namespace std::chrono;

    ll best_ns = std::numeric_limits<ll>::max();
    volatile ll sink = 0;

    for (int r = 0; r < repeat; r++) {
        auto start = high_resolution_clock::now();
        for (size_t i = 0; i < xs.size(); i++) {
            sink ^= f(xs[i], ys[i]);
        }
        auto end = high_resolution_clock::now();
        ll ns = duration_cast<nanoseconds>(end - start).count();
        if (ns < best_ns) best_ns = ns;
    }

    if (sink == 123456789) std::cerr << "ignore: " << sink << "\n";

    return best_ns;
}

signed main() {
    const ll N = 1'000'000;

    // Generate random inputs
    std::mt19937_64 rng(12345);
    std::uniform_int_distribution<ll> dist(1, 1'000'000'000'000);

    std::vector<ll> xs(N), ys(N);

    for (size_t i = 0; i < N; i++) {
        xs[i] = dist(rng);
        ys[i] = dist(rng);
    }

    // Warm up (optional, helps with caches / JIT-like effects)
    volatile ll warmup_sink = 0;
    for (size_t i = 0; i < N; i++) {
        warmup_sink ^= std::gcd(xs[i], ys[i]);
    }
    for (size_t i = 0; i < N; i++) {
        warmup_sink ^= gcdll(xs[i], ys[i]);
    }

    // Benchmark std::gcd
    auto ns_std = benchmark([](ll a, ll b)
                    {return std::gcd(a, b); }, xs, ys);

    // Benchmark manual gcd
    auto ns_manual = benchmark([](ll a, ll b)
                    {return gcdll(a, b); }, xs, ys);

    std::cout << "Total time std::gcd   : " << ns_std << " ns\n";
    std::cout << "Total time manual gcd : " << ns_manual << " ns\n";
    std::cout << "std::gcd per call     : " << (double)ns_std / xs.size() << " ns\n";
    std::cout << "manual gcd per call     : " << (double)ns_manual / xs.size() << " ns\n";

    /*
        Total time std::gcd   : 81639627 ns
        Total time manual gcd : 66890298 ns
        std::gcd per call     : 81.6396 ns
        manual gcd per call     : 66.8903 ns
    */

}
