#pragma once
#include "core.h"
#include "spf.h"

namespace nt {

inline bool exists_non_coprime_pair(const vi& a, const SPF& spf) {
    unordered_set<int> seen;
    seen.reserve(a.size() * 2);

    for (auto x : a) {
        auto ps = spf.distinct_primes(x);
        for (auto p : ps) {
            if (seen.count(p)) return true;
            seen.insert(p);
        }
    }
    return false;
}

// counts pairs with gcd exactly g for g=1..maxa
struct GcdPairsCounter {
    int maxa = 0;
    vi freq;
    vi cnt;
    vl exact; // exact[g] = #pairs with gcd exactly = g

    GcdPairsCounter() {}
    GcdPairsCounter(int _maxa) { init(_maxa); }

    void init(int _maxa) {
        maxa = _maxa;
        freq.assign(maxa+1, 0);
        cnt.assign(maxa+1, 0);
        exact.assign(maxa+1, 0);
    }

    void add_values(const vi& a) {
        for (auto x : a) {
            if (0 <= x && x <= maxa) freq[x]++;
        }
    }

    void compute() {
        fill(all(cnt), 0);
        fill(all(exact), 0);

        for (int g = 1; g <= maxa; g++) {
            for (int k = g; k <= maxa; k += g) cnt[g] += freq[k];
        }

        for (int g = maxa; g >= 1; g--) {
            ll c = cnt[g];
            if (c >= 2) exact[g] = c * (c-1) / 2;
            for (int k = g + g; k <= maxa; k += g) exact[g] -= exact[k];
        }
    }
};

} // namespace nt
