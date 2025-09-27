#include <bits/stdc++.h>
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


// O(sqrt(n))
int phi(int n) {
    int result = n;
    for (int i = 2; i*i <= n; i++) {
        if (n % i == 0) {
            while (n % i == 0) n /= i;
            result -= result / i;
        }
    }

    if (n > 1) result -= result / n;
    return result;
}

// O(n log log n)
void phi_1_to_n(int n) {
    vector<int> phi(n+1);
    for (int i = 0; i <= n; i++) {
        phi[i] = i;
    }

    for (int i = 2; i <= n; i++) {
        if (phi[i] == i) {
            for (int j = i; j <= n; j += i) {
                phi[j] -= phi[j] / i;
            }
        }
    }
}


