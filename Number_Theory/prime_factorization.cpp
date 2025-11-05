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


int main(void)
{
    clock_t z = clock();
    ios::sync_with_stdio(0);
    cin.tie(0);

    i64 n; cin >> n;
    vector<i64> primes = prime_factorization(n);
    for(auto el : primes) cout << el << " ";



	cerr << "\nRun Time : " << ((double)(clock() - z) / CLOCKS_PER_SEC) << '\n';

    return 0;
}
