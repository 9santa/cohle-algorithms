#include <bits/stdc++.h>
using namespace std;
using ll = long long;

constexpr int N = 1'000'000;
int mu[N+1];
int primes[N+1], pcnt = 0;
bool comp[N+1];

void mobius_sieve() {
    mu[1] = 1;

    for (int i = 2; i <= N; i++) {
        if (!comp[i]) {
            primes[pcnt++] = i;
            mu[i] = -1; // prime -> -1
        }
        for (int j = 0; j < pcnt; j++) {
            int p = primes[j];
            ll x = 1LL * p * i;
            if (x > N) break;

            comp[x] = true;

            if (i % p == 0) {
                mu[x] = 0;  // prime square -> 0
                break;
            } else mu[x] = -mu[i];
        }
    }
}

ll mobius(ll n, vector<ll>& g) {
    ll res = 0;
    for (ll d = 1; d*d <= n; d++) {
        if (n%d == 0) {
            res += mu[d] * g[n/d];
            if (d*d != n) res += mu[n/d] * g[d];
        }
    }
    return res;
}
