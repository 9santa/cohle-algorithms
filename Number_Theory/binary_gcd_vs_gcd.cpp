#include <bits/stdc++.h>
using namespace std;
using namespace chrono;

using i64 = long long;

i64 gcd(i64 a, i64 b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

i64 bgcd(i64 a, i64 b) {
    if (!a || !b) return a | b;
    unsigned shift = __builtin_ctz(a | b);
    a >>= __builtin_ctz(a);
    do {
        b >>= __builtin_ctz(b);
        if (a > b) swap(a, b);
        b -= a;
    } while (b);
    return a << shift;
}

int main() {
    const int N = 10'000'000; // number of random pairs
    vector<pair<i64, i64>> test(N);
    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<i64> dist(1, 1e18);

    for (auto &p : test) p = {dist(rng), dist(rng)};

    i64 result = 0;

    // Benchmark Euclidean
    auto start = high_resolution_clock::now();
    for (auto [a, b] : test) result ^= gcd(a, b); // xor to prevent optimization
    auto end = high_resolution_clock::now();
    cout << "Euclidean GCD: "
         << duration_cast<milliseconds>(end - start).count() << " ms, result=" << result << "\n";

    result = 0;

    // Benchmark Binary GCD
    start = high_resolution_clock::now();
    for (auto [a, b] : test) result ^= bgcd(a, b);
    end = high_resolution_clock::now();
    cout << "Binary GCD: "
         << duration_cast<milliseconds>(end - start).count() << " ms, result=" << result << "\n";
}
