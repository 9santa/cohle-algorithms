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
vector<int> prime_factorization(int n) {
    vector<int> primes;
    for (int i = 2; i < sqrt(n); i++) {
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

    vector<int> primes = prime_factorization(376);
    for(auto el : primes) cout << el << " ";



	cerr << "\nRun Time : " << ((double)(clock() - z) / CLOCKS_PER_SEC) << '\n';

    return 0;
}
