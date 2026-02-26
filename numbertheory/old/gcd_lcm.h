#include "prime_factorization.h"

// GCD and LCM are assosiative, so we can do: gcd(a,b,c,d) = gcd(a, gcd(b, gcd(c, d)))
i64 gcd(i64 a, i64 b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

ll lcm(ll a, ll b) {
    return a / gcd(a, b) * b; // (a*b) / gcd(a, b);
}

// function to check if any 2 numbers in the array have gcd(a[i], a[j]) > 1 (i.e. they are not coprime)
bool exists_non_coprime_pair(const vi& a) {
    unordered_map<int, int> cnt;
    for (auto x : a) {
        for (auto p : pfac[x]) {
            if (cnt[p] > 0) return true;
            cnt[p]++;
        }
    }
    return false;
}

// Count pairs in the array with gcd = 1...MAXA
constexpr int MAXA = 200005;
struct count_pair_with_gcd {
    vector<int> a;
    int n;
    int cnt[MAXA+1]; // cnt[g] = number of elements divisible by g
    ll exactgcd[MAXA+1]; // exactgcd[g] = number of pairs with gcd exactly g

    void compute_pairs() {
        memset(cnt, 0, sizeof(cnt));
        memset(exactgcd, 0, sizeof(exactgcd));

        // count multiples
        for (auto x : a) {
            for (int g = 1; g * g <= x; g++) {
                if (x % g == 0) {
                    cnt[g]++;
                    if (g != x/g) cnt[x/g]++;
                }
            }
        }

        // inclusion-exclusion: from largest to smallest
        for (int g = MAXA; g >= 1; g--) {
            if (cnt[g] >= 2) {
                exactgcd[g] = 1LL * cnt[g] * (cnt[g] - 1)/2;
                for (int k = 2*g; k <= MAXA; k += g)
                    exactgcd[g] -= exactgcd[k];
            }
        }
    }
};
