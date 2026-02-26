#pragma once
#include "core.h"

namespace nt {

// compile-time sized sieve
template<int SZ>
struct StaticSieve {
    bitset<SZ> is_prime;
    vi primes;

    StaticSieve() {
        is_prime.set();
        if (SZ > 0) is_prime[0] = 0;
        if (SZ > 1) is_prime[1] = 0;
        for (int i = 4; i < SZ; i += 2) is_prime[i] = 0;

        for (int i = 3; 1LL * i * i < SZ; i += 2) {
            if (is_prime[i]) {
                for (int j = i * i; j < SZ; j += i * 2) is_prime[j] = 0;
            }
        }

        primes.clear();
        if (SZ > 2) primes.pb(2);
        for (int i = 3; i < SZ; i += 2) if (is_prime[i]) primes.pb(i);
        if (SZ == 2) primes = {2};
    }
};

// linear sieve: primes + lp (least prime)
struct LinearSieve {
    int n = 0;
    vi primes;
    vi lp; // lp[x] = least prime dividing x

    LinearSieve() {}
    LinearSieve(int _n) { init(_n); }

    void init(int _n) {
        n = _n;
        primes.clear();
        lp.assign(n+1, 0);
        for (int i = 2; i <= n; i++) {
            if (lp[i] == 0) {
                lp[i] = i;
                primes.pb(i);
            }
            for (auto p : primes) {
                if (p > lp[i] || 1LL * i * p > n) break;
                lp[i * p] = p;
            }
        }
    }

    bool is_prime_int(int x) const {
        return x >= 2 && x <= n && lp[x] == x;
    }
};

// segmented sieve. less memory + cache friendly. counts number of primes <= n
inline int count_primes(int n) {
    if (n < 2) return 0;
    const int S = 10000; // block size
    vector<int> primes;
    int nsqrt = sqrt(n);
    vector<char> is_prime(nsqrt+2, true);

    for (int i = 2; i <= nsqrt; i++) {
        if (is_prime[i]) {
            primes.push_back(i);
            for (int j = i*i; j <= nsqrt; j += i)
                is_prime[j] = false;
        }
    }

    int res = 0;
    vector<char> block(S);

    for (int k = 0; k * S <= n; k++) {
        fill(block.begin(), block.end(), true);
        int start = k * S;

        // mark multiples of primes in the current block
        for (int p : primes) {
            int start_idx = (start + p - 1) / p;
            int j = max(start_idx, p) * p - start;
            for (; j < S; j += p)
                block[j] = false;
        }

        if (k == 0) block[0] = block[1] = false;

        // count primes in the block
        for (int i = 0; i < S && start + i <= n; i++) {
            if (block[i]) res++;
        }
    }

    return res;
}

} // namespace nt
