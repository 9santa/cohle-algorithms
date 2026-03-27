#include <benchmark/benchmark.h>
#include <numeric>   // std::gcd
#include <random>
#include <vector>

// -------------------- manual gcd --------------------
int gcd_manual(int a, int b) {
    while (b != 0) {
        int t = a % b;
        a = b;
        b = t;
    }
    return a < 0 ? -a : a;
}

// -------------------- shared random data --------------------
static std::vector<int> xs;
static std::vector<int> ys;

static void PrepareData(std::size_t N) {
    static bool initialized = false;
    static std::size_t currentN = 0;

    if (initialized && currentN == N) return;

    xs.resize(N);
    ys.resize(N);

    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(1, 1'000'000);

    for (std::size_t i = 0; i < N; ++i) {
        xs[i] = dist(rng);
        ys[i] = dist(rng);
    }

    initialized = true;
    currentN = N;
}

// -------------------- benchmarks --------------------
static void BM_std_gcd(benchmark::State& state) {
    const std::size_t N = static_cast<std::size_t>(state.range(0));
    PrepareData(N);

    for (auto _ : state) {
        int acc = 0;
        for (std::size_t i = 0; i < N; ++i) {
            acc ^= std::gcd(xs[i], ys[i]);
        }
        benchmark::DoNotOptimize(acc);
    }

    // report number of gcd calls
    state.SetItemsProcessed(state.iterations() * N);
}

static void BM_manual_gcd(benchmark::State& state) {
    const std::size_t N = static_cast<std::size_t>(state.range(0));
    PrepareData(N);

    for (auto _ : state) {
        int acc = 0;
        for (std::size_t i = 0; i < N; ++i) {
            acc ^= gcd_manual(xs[i], ys[i]);
        }
        benchmark::DoNotOptimize(acc);
    }

    state.SetItemsProcessed(state.iterations() * N);
}

// Try a few different batch sizes:
BENCHMARK(BM_std_gcd)->Arg(1 << 10)->Arg(1 << 14)->Arg(1 << 18);
BENCHMARK(BM_manual_gcd)->Arg(1 << 10)->Arg(1 << 14)->Arg(1 << 18);

BENCHMARK_MAIN();

/*
BM_std_gcd/1024            8998 ns         8980 ns        77922 items_per_second=114.029M/s
BM_std_gcd/16384         690333 ns       688887 ns         1017 items_per_second=23.7833M/s
BM_std_gcd/262144      11170919 ns     11144355 ns           63 items_per_second=23.5226M/s
BM_manual_gcd/1024        16268 ns        16237 ns        43006 items_per_second=63.0671M/s
BM_manual_gcd/16384      544592 ns       543526 ns         1287 items_per_second=30.1439M/s
BM_manual_gcd/262144    8890426 ns      8868274 ns           79 items_per_second=29.5598M/s

Google benchmark shows something interesting depending on batch sizes
*/
