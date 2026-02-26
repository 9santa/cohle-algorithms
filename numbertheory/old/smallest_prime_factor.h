#include <bits/stdc++.h>
#include <cmath>
#include <ctime>

#define pb push_back
#define eb emplace_back
#define all(a) (a).begin(), (a).end()

using namespace std;

using i64 = long long;
using u64 = unsigned long long;
using u32 = unsigned int;
using u128 = unsigned __int128;
using i128 = __int128;
using ld = long double;
using pii = pair<int, int>;


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

// This is faster than standard prime factorization which is O(sqrt(n))
// 1. We build an array spf up to n. spf[i] will store the smallest prime that divides i
//    This is done with a standard Sieve of Eratosthenes O(n log log n)
// 2. Ultra-fast factorization. All prime factors are precomputed, you just look them up. O(log k), where k is the factorized number
void smallest_prime_factor(i64 n) {
    vector<int> spf(n+1);
    std::iota(spf.begin(), spf.end(), 0);   // fills with 0,1,2, ..., n

    for (int i = 2; i*i <= n; i++) {
        if (spf[i] == i) { // i is a prime
            for (int j = i*i; j <= n; j += i) {
                if (spf[j] == j) { // if spf[j] is not set yet (to not override it with bigger prime)
                    spf[j] = i;
                }
            }
        }
    }
}
