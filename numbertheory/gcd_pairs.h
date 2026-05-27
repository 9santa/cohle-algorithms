#pragma once
#include "core.h"
#include "spf.h"

namespace nt {

/**
 * Returns whether any pair in a shares a prime factor.
 * @param spf Precomputed SPF table covering all values in a.
 * Time: O(total number of prime factors). Space: O(number of seen primes).
 */
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

/** Counts unordered pairs by exact gcd value for g in [1, maxa]. Space: O(maxa). */
struct GcdPairsCounter {
    int maxa = 0;
    vi freq;
    vi cnt;
    vl exact; // exact[g] = #pairs with gcd exactly = g

    GcdPairsCounter() {}
    GcdPairsCounter(int _maxa) { init(_maxa); }

    /** Initializes arrays up to _maxa. Time: O(maxa). */
    void init(int _maxa) {
        maxa = _maxa;
        freq.assign(maxa+1, 0);
        cnt.assign(maxa+1, 0);
        exact.assign(maxa+1, 0);
    }

    /** Adds values into the frequency table. Time: O(a.size()). */
    void add_values(const vi& a) {
        for (auto x : a) {
            if (0 <= x && x <= maxa) freq[x]++;
        }
    }

    /** Computes exact[g] = number of pairs with gcd exactly g. Time: O(maxa log maxa). */
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
