#include "../header.h"

// Simple algorithm for prime factorization. Any number can be represented as a product of primes
vector<i64> prime_factorization(i64 n) {
    vector<i64> primes;
    for (i64 i = 2; i < sqrt(n); i++) {
        while (n % i == 0) {
            n /= i;
            primes.push_back(i);
        }
    }

    primes.push_back(n);
    return primes;
}

// returns pair {prime, power}
vector<pair<ll, ll>> factorize(ll n) {
    vector<pair<ll, ll>> primes;
    for (ll i = 2; i*i <= n; i++) {
        ll exp = 0;
        while (n%i == 0) {
            exp++;
            n /= i;
        }
        if (exp > 0) primes.pb({i, exp});
    }
    if (n > 1) primes.pb({n, 1});
    return primes;
}

// Computes prime factors for all numbers up to MAXN
constexpr int MAXN = 2e5+2;
vector<int> pfac[MAXN+1];
void compute_pfac() {
    for (int i = 2; i <= MAXN; i++) {
        if (!pfac[i].empty()) continue;
        for (int j = i; j <= MAXN; j += i) {
            pfac[j].push_back(i); 
        } 
    }
}
