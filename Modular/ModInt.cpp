#include <bits/stdc++.h>
using namespace std;
using ll = long long;

template<ll MOD>
struct Mint {
    ll x;

    Mint(ll v = 0) {
        if (v < 0) v = v % MOD + MOD;
        if (v >= MOD) v %= MOD;
        x = v;
    }

    Mint operator+(const Mint& other) const {
        ll r = x + other.x;
        if (r >= MOD) r -= MOD;
        return r;
    }

    Mint operator-(const Mint& other) const {
        ll r = x - other.x;
        if (r < 0) r += MOD;
        return r;
    }

    Mint operator*(const Mint& other) const {
        return (x * other.x) % MOD;
    }

    Mint pow(ll p) const {
        Mint r = 1, a = x;
        while (p) {
            if (p&1) r = r*a;
            a = a * a;
            p >>= 1;
        }
        return r;
    }

    Mint inv() const { return pow(MOD - 2); }

    Mint operator/(const Mint& other) const { return (*this) * other.inv(); }
};
